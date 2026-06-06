//
// Created by DohyunKim on 26. 6. 6..
//

#include "bfs_teg.h"

#include <cassert>

// TEG vertex encoding: id(node, t) = node*(H+1) + t.

BfsTeg::BfsTeg()
    : teg(nullptr), builtH(-1),
      visitedGen(nullptr), pred(nullptr), q(nullptr), scratchSize(0), gen(0) {}

BfsTeg::~BfsTeg() {
    delete teg;
    delete[] visitedGen;
    delete[] pred;
    delete[] q;
}

// Allocate scratch once (or grow). visitedGen starts at 0; gen starts at 0 too,
// so the first search uses gen=1 (see findPath) — no stamp can falsely match.
void BfsTeg::ensureScratch(int size) {
    if (size <= scratchSize) return;
    delete[] visitedGen;
    delete[] pred;
    delete[] q;
    visitedGen = new int[size];
    pred       = new int[size];
    q          = new int[size];
    for (int i = 0; i < size; ++i) visitedGen[i] = 0;   // once
    scratchSize = size;
}

// Build the TEG from the layout only (no reservation). Vertices node*(H+1)+t,
// edges: wait (u,t)->(u,t+1) and move (u,t)->(v,t+1) for each base edge u->v.
// Reservation is NOT consulted here — it is applied at search time so the TEG
// can be reused across agents.
void BfsTeg::buildTEG(const Graph& graph, int H) {
    int n = graph.size();
    int stride = H + 1;
    delete teg;
    teg = new TimeExpandedGraph(n * stride, H);
    for (int u = 0; u < n; ++u) {
        for (int t = 0; t < H; ++t) {     // t == H has no outgoing edges
            int from = u * stride + t;
            teg->addEdge(from, u * stride + (t + 1));          // wait
            for (int i = 0; i < graph.degree(u); ++i) {
                int v = graph.neighbor(u, i);
                teg->addEdge(from, v * stride + (t + 1));       // move
            }
        }
    }
    builtH = H;
}

Path BfsTeg::findPath(int start, int goal,
                      const Graph& graph,
                      const ReservationTable& reservation,
                      int H) {
    assert(H >= 0 && "BfsTeg::findPath: negative horizon");
    int stride = H + 1;
    int Vt = graph.size() * stride;

    // Start blocked at t=0 → no path.
    if (!reservation.isFree(start, 0)) {
        return Path(0);
    }

    // Build the TEG once (or rebuild if horizon changed); reuse across agents.
    if (teg == nullptr || builtH != H) {
        buildTEG(graph, H);
    }

    // Scratch allocated once; bump generation instead of clearing (O(1) reset).
    ensureScratch(Vt);
    ++gen;                    // marks from previous searches are now stale

    int qHead = 0, qTail = 0;
    int s = start * stride + 0;
    visitedGen[s] = gen;
    pred[s] = -1;             // start has no predecessor
    q[qTail++] = s;

    int goalVertex = -1;
    while (qHead < qTail) {
        int cur = q[qHead++];
        if (cur / stride == goal) {
            goalVertex = cur;
            break;
        }
        for (int i = 0; i < teg->degree(cur); ++i) {
            int nxt = teg->neighbor(cur, i);
            if (visitedGen[nxt] == gen) continue;       // visited this search
            // reservation absorbed at search time: skip occupied (node,t)
            int nNode = nxt / stride;
            int nTime = nxt % stride;
            if (!reservation.isFree(nNode, nTime)) continue;
            visitedGen[nxt] = gen;
            pred[nxt] = cur;
            q[qTail++] = nxt;
        }
    }

    Path result(0);
    if (goalVertex != -1) {
        result = Path(H + 1);
        int arriveT = goalVertex % stride;
        int cur = goalVertex;
        while (cur != -1) {
            result.set(cur % stride, cur / stride);
            cur = pred[cur];
        }
        for (int t = arriveT + 1; t <= H; ++t) {
            result.set(t, goal);
        }
    }
    return result;
}
