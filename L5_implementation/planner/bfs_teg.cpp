//
// Created by DohyunKim on 26. 6. 6..
//

#include "bfs_teg.h"

#include "../data_structure/queue.h"

#include <cassert>
#include <thread>
#include <vector>

// TEG vertex encoding: id(node, t) = node*(H+1) + t.

BfsTeg::BfsTeg()
    : teg(nullptr), builtH(-1),
      visitedGen(nullptr), pred(nullptr), qpool(nullptr), scratchSize(0), gen(0) {}

BfsTeg::~BfsTeg() {
    delete teg;
    delete[] visitedGen;
    delete[] pred;
    delete[] qpool;
}

// Allocate scratch once (or grow). visitedGen starts at 0; gen starts at 0 too,
// so the first search uses gen=1 (see findPath) — no stamp can falsely match.
void BfsTeg::ensureScratch(int size) {
    if (size <= scratchSize) return;
    delete[] visitedGen;
    delete[] pred;
    delete[] qpool;
    visitedGen = new int[size];
    pred       = new int[size];
    qpool      = new NodeRef[size];
    for (int i = 0; i < size; ++i) { visitedGen[i] = 0; qpool[i].id = i; }  // once
    scratchSize = size;
}

// Build the TEG from the layout only (no reservation). Vertices node*(H+1)+t,
// edges: wait (u,t)->(u,t+1) and move (u,t)->(v,t+1) for each base edge u->v.
// Reservation is NOT consulted here — it is applied at search time so the TEG
// can be reused across agents.
//
// PARALLEL: this is the one heavy step (the V*(H+1) vertices), and it is trivially
// parallel — addEdge(from, .) only writes row `from = u*stride+t`, so partitioning
// the outer `u` loop gives each thread a DISJOINT set of rows (no shared writes, no
// locks). We split `u` across hardware threads. This changes only HOW the same TEG
// is built, not the algorithm or any data structure.
void BfsTeg::buildTEG(const Graph& graph, int H) {
    int n = graph.size();
    int stride = H + 1;
    delete teg;
    teg = new TimeExpandedGraph(n * stride, H);

    // One worker fills rows for u in [u0, u1).
    auto fillRange = [&](int u0, int u1) {
        for (int u = u0; u < u1; ++u) {
            int deg = graph.degree(u);
            for (int t = 0; t < H; ++t) {     // t == H has no outgoing edges
                int from = u * stride + t;
                teg->addEdge(from, u * stride + (t + 1));        // wait
                for (int i = 0; i < deg; ++i) {
                    int v = graph.neighbor(u, i);
                    teg->addEdge(from, v * stride + (t + 1));     // move
                }
            }
        }
    };

    unsigned hw = std::thread::hardware_concurrency();
    int workers = (hw == 0) ? 1 : static_cast<int>(hw);
    if (workers > n) workers = (n > 0) ? n : 1;
    if (workers <= 1) {
        fillRange(0, n);                       // single-threaded fallback
    } else {
        std::vector<std::thread> pool;
        pool.reserve(workers);
        int chunk = (n + workers - 1) / workers;
        for (int w = 0; w < workers; ++w) {
            int u0 = w * chunk, u1 = (u0 + chunk < n) ? u0 + chunk : n;
            if (u0 >= u1) break;
            pool.emplace_back(fillRange, u0, u1);
        }
        for (auto& th : pool) th.join();
    }
    builtH = H;
}

// BFS over the (reused) TEG from space-time vertex `srcVertex` to the first vertex
// whose node == goal. Returns that goal vertex id, or -1 if unreachable within H.
// Uses the generation stamp set by the caller (does NOT bump gen) so a multi-segment
// search keeps one visited map across segments? No — each segment is independent, so
// the caller bumps gen per segment. Here we just expand from srcVertex.
int BfsTeg::bfsToGoal(int srcVertex, int goal, const ReservationTable& reservation,
                      int stride) {
    Queue q;
    visitedGen[srcVertex] = gen;
    pred[srcVertex] = -1;
    q.enqueue(&qpool[srcVertex]);
    while (!q.isEmpty()) {
        int cur = static_cast<NodeRef*>(q.dequeue())->id;
        if (cur / stride == goal) return cur;          // first arrival = earliest
        for (int i = 0; i < teg->degree(cur); ++i) {
            int nxt = teg->neighbor(cur, i);
            if (visitedGen[nxt] == gen) continue;
            int nNode = nxt / stride, nTime = nxt % stride;
            if (!reservation.isFree(nNode, nTime)) continue;   // skip occupied (node,t)
            visitedGen[nxt] = gen;
            pred[nxt] = cur;
            q.enqueue(&qpool[nxt]);
        }
    }
    return -1;
}

Path BfsTeg::findPath(int start,
                      const int* goals, int goalCount, int dwell,
                      const Graph& graph,
                      const ReservationTable& reservation,
                      int H) {
    assert(H >= 0 && dwell >= 0 && "BfsTeg::findPath: bad H/dwell");
    int stride = H + 1;

    if (!reservation.isFree(start, 0)) return Path(0);   // start blocked at t=0

    if (teg == nullptr || builtH != H) buildTEG(graph, H);
    ensureScratch(graph.size() * stride);

    Path path(H + 1);
    int curVertex = start * stride + 0;   // (start, 0)

    for (int k = 0; k < goalCount; ++k) {
        int goal = goals[k];
        ++gen;                            // independent search per segment
        int gv = bfsToGoal(curVertex, goal, reservation, stride);
        if (gv == -1) break;              // goal not reachable within H this segment

        // Fill this segment's time-indexed positions by walking pred gv->curVertex.
        int v = gv;
        while (v != -1) {
            int node = v / stride, tm = v % stride;
            if (tm >= 0 && tm <= H) path.set(tm, node);
            if (v == curVertex) break;
            v = pred[v];
        }

        // Dwell at the goal: occupy [arriveT, arriveT+1+dwell) via wait cells, each
        // checked against the reservation (skip if any is occupied — then stop dwell
        // early; collision-free because we only write free cells).
        int arriveT = gv % stride;
        int hold = arriveT;
        for (int s = arriveT; s < arriveT + 1 + dwell && s <= H; ++s) {
            if (!reservation.isFree(goal, s)) break;     // can't hold further
            path.set(s, goal);
            hold = s;
        }

        curVertex = goal * stride + hold;   // continue from end of the hold
        int t = hold + 1;
        if (t > H) return path;             // horizon filled
        // next segment starts its BFS at (goal, hold) — but we need (goal, t) as the
        // launch so the agent steps onward at t = hold+1; bfsToGoal expands from
        // curVertex via wait/move edges, so launching at (goal, hold) is correct.
    }

    // Goal queue exhausted (or a segment was unreachable within H) before the
    // horizon — idle at the last reached node for the remaining steps. CRITICAL:
    // an idle is still an occupancy, so each idle cell MUST be reservation-free. If a
    // higher-priority agent occupies this node at some future step, we can only idle
    // up to (not into) that step; the path is then TRUNCATED there (a partial path,
    // exactly like an unreachable segment). Leaving the tail unfilled (-1) is correct:
    // recordPath skips -1 cells and PP commits only the first C<=H steps, replanning
    // the rest next round. Writing past an occupied cell is what caused collisions.
    int last = curVertex / stride, from = curVertex % stride;
    for (int s = from; s <= H; ++s) {
        if (!reservation.isFree(last, s)) break;   // stop before stepping onto occupancy
        path.set(s, last);
    }
    return path;
}
