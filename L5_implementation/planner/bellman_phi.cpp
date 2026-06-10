//
// Created by DohyunKim on 26. 6. 7..
//

#include "bellman_phi.h"

#include "../data_structure/queue.h"

#include <cassert>
#include <climits>
#include <utility>

static const int INF = INT_MAX;

BellmanPhi::BellmanPhi()
    : earliest(nullptr), pred(nullptr), seen(nullptr), inQueue(nullptr),
      pool(nullptr), pathBuf(nullptr), capacity(0), gen(0) {}

BellmanPhi::~BellmanPhi() {
    delete[] earliest;
    delete[] pred;
    delete[] seen;
    delete[] inQueue;
    delete[] pool;
    delete[] pathBuf;
}

void BellmanPhi::ensureCapacity(int n) {
    if (n <= capacity) return;
    delete[] earliest;
    delete[] pred;
    delete[] seen;
    delete[] inQueue;
    delete[] pool;
    delete[] pathBuf;
    earliest = new int[n];
    pred     = new int[n];
    seen     = new int[n];
    inQueue  = new bool[n];
    pool     = new NodeRef[n];
    pathBuf  = new Step[n];
    for (int v = 0; v < n; ++v) { seen[v] = 0; inQueue[v] = false; pool[v].id = v; }
    capacity = n;
}

// SPFA relaxation from (src @ t0) to `goal`, NOT capped by H. earliest[]/pred[] are
// valid only where seen[v]==gen. Returns arrival time at goal (INF if unreachable).
int BellmanPhi::relaxToGoal(int src, int t0, int goal,
                            const Graph& graph, const ReservationTable& reservation) {
    ++gen;
    seen[src] = gen; earliest[src] = t0; pred[src] = -1; inQueue[src] = true;
    Queue q;
    q.enqueue(&pool[src]);
    while (!q.isEmpty()) {
        int u = static_cast<NodeRef*>(q.dequeue())->id;
        inQueue[u] = false;
        int tu = earliest[u];
        int deg = graph.degree(u);
        for (int i = 0; i < deg; ++i) {
            int v = graph.neighbor(u, i);
            int arrive = reservation.phi(v, tu + 1);   // 1 step + avoid v's occupancy
            if (seen[v] != gen || arrive < earliest[v]) {
                seen[v] = gen; earliest[v] = arrive; pred[v] = u;
                if (!inQueue[v]) { q.enqueue(&pool[v]); inQueue[v] = true; }
            }
        }
    }
    return (seen[goal] == gen) ? earliest[goal] : INF;
}

Path BellmanPhi::findPath(int start,
                          const int* goals, int goalCount, int dwell,
                          const Graph& graph,
                          const ReservationTable& reservation,
                          int H) {
    assert(H >= 0 && dwell >= 0 && "BellmanPhi::findPath: bad H/dwell");
    ensureCapacity(graph.size());

    Path path(H + 1);

    // ── Occupancy-fill invariant ────────────────────────────────────────────────
    // EVERY cell this path writes — a move, a wait, a dwell, or a tail idle — is an
    // occupancy and MUST be reservation-free. relaxToGoal/phi only guarantee the
    // ARRIVAL instant at each node is free; they do NOT guarantee that the span a
    // node is HELD (while waiting for the next node to clear, or dwelling at a goal,
    // or idling at horizon) stays free, because a higher-priority agent may occupy
    // this node at a later step. So we write the planned route forward cell-by-cell
    // and TRUNCATE at the first occupied cell (a partial path — same contract as an
    // unreachable goal). Writing past an occupied cell is exactly what caused the
    // multi-step-stay collisions. `fill()` advances `t` (next free write time) and
    // returns false the moment it must stop; on false we return the partial path.
    int cur = start;
    int t = 0;

    auto fillUntil = [&](int node, int until) -> bool {
        // Write `node` for steps [t, until), each verified free. Stops (returns false)
        // at the first occupied step or at H+1, truncating the path there.
        for (; t < until; ++t) {
            if (t > H) return false;                       // horizon filled
            if (!reservation.isFree(node, t)) return false;// blocked: truncate here
            path.set(t, node);
        }
        return true;
    };

    // Earliest instant the start node is enterable (the agent is parked there from
    // t=0). If start is occupied at t=0 by a higher-priority agent, it cannot even
    // hold its current position → empty path (start blocked), same as before.
    int leave = reservation.phi(cur, 0);
    if (leave != 0) return Path(0);            // start not free at t=0 → blocked
    // (leave == 0: start free at t=0; the per-node hold loop below occupies start
    //  from t=0 over its wait span, all cells verified by fillUntil.)
    t = 0;

    // Visit each goal in turn: relax cur->goal, walk pred, dwell, continue.
    for (int k = 0; k < goalCount; ++k) {
        int goal = goals[k];
        int arriveT = relaxToGoal(cur, t, goal, graph, reservation);
        if (arriveT == INF) break;             // unreachable (not in strong graph)

        // Reconstruct the node sequence cur..goal (pred chain) into `pathBuf`,
        // reversed to forward order, with each node's planned ENTER time.
        // We then write forward, holding each node from its enter time until the
        // next node's enter time (that hold is the wait span — now verified).
        int len = 0;
        for (int node = goal; ; node = pred[node]) {
            pathBuf[len].node  = node;
            pathBuf[len].enter = (node == cur) ? t : earliest[node];
            ++len;
            if (node == cur) break;
        }
        // pathBuf is goal->cur; walk it backwards to get cur->goal forward order.
        bool truncated = false;
        for (int idx = len - 1; idx >= 1 && !truncated; --idx) {
            int node      = pathBuf[idx].node;
            int nextEnter = pathBuf[idx - 1].enter;   // when we step onto the next node
            // Hold `node` over [enter, nextEnter): the wait/transit span. Verify it.
            if (!fillUntil(node, nextEnter)) truncated = true;
        }
        if (truncated) return path;            // a wait span hit occupancy → partial

        // Dwell at the goal: hold [arriveT, arriveT+1+dwell). phiWindow already chose
        // arriveT so this window is free, but verify per cell (defensive; also stops
        // cleanly at H). The goal's enter time is pathBuf[0].enter == arriveT.
        // Re-place arrival via phiWindow so the whole hold is free.
        int need = 1 + dwell;
        int holdStart = reservation.phiWindow(goal, arriveT, need);
        // If the dwell window is later than the bare arrival, the extra wait happens
        // AT THE GOAL itself (we already arrived); just hold from arriveT and let the
        // per-cell check write the free run. (Any blocked cell truncates.)
        (void)holdStart;
        if (!fillUntil(goal, arriveT + need)) return path;  // dwell blocked → partial

        cur = goal;
        t = arriveT + need;                    // depart after the hold window
        if (t > H) return path;                // horizon filled
    }

    // Goal queue exhausted before H: idle at cur — but only over free cells.
    fillUntil(cur, H + 1);
    return path;
}
