//
// Created by DohyunKim on 26. 6. 7..
//

#include "bellman_phi.h"

#include "../data_structure/queue.h"

#include <cassert>
#include <climits>

static const int INF = INT_MAX;

BellmanPhi::BellmanPhi()
    : earliest(nullptr), pred(nullptr), inQueue(nullptr), pool(nullptr),
      capacity(0) {}

BellmanPhi::~BellmanPhi() {
    delete[] earliest;
    delete[] pred;
    delete[] inQueue;
    delete[] pool;
}

void BellmanPhi::ensureCapacity(int n) {
    if (n <= capacity) return;
    delete[] earliest;
    delete[] pred;
    delete[] inQueue;
    delete[] pool;
    earliest = new int[n];
    pred     = new int[n];
    inQueue  = new bool[n];
    pool     = new NodeRef[n];
    for (int v = 0; v < n; ++v) pool[v].id = v;   // fixed wrappers, reused
    capacity = n;
}

Path BellmanPhi::findPath(int start, int goal,
                          const Graph& graph,
                          const ReservationTable& reservation,
                          int H) {
    assert(H >= 0 && "BellmanPhi::findPath: negative horizon");
    int n = graph.size();
    ensureCapacity(n);

    for (int v = 0; v < n; ++v) {
        earliest[v] = INF;
        pred[v] = -1;
        inQueue[v] = false;
    }

    // Earliest enterable time at the start node.
    int s0 = reservation.phi(start, 0);
    if (s0 > H) return Path(0);          // blocked from the very start
    earliest[start] = s0;

    Queue q;
    q.enqueue(&pool[start]);
    inQueue[start] = true;

    while (!q.isEmpty()) {
        int u = static_cast<NodeRef*>(q.dequeue())->id;
        inQueue[u] = false;
        int tu = earliest[u];
        if (tu + 1 > H) continue;        // cannot take even one more step

        int deg = graph.degree(u);
        for (int i = 0; i < deg; ++i) {
            int v = graph.neighbor(u, i);
            int arrive = reservation.phi(v, tu + 1);   // 1 step + avoid v's occupancy
            if (arrive <= H && arrive < earliest[v]) {
                earliest[v] = arrive;
                pred[v] = u;
                if (!inQueue[v]) {
                    q.enqueue(&pool[v]);
                    inQueue[v] = true;
                }
            }
        }
    }

    if (earliest[goal] > H) return Path(0);   // unreachable within horizon

    // ── Reconstruct the time-indexed path (filling waits) ──────────────────
    Path path(H + 1);
    int arriveT = earliest[goal];
    path.set(arriveT, goal);

    // Walk pred goal->start. Each node p is occupied during [t_p, t_child):
    // it sits at p until the single step that lands on the child at t_child.
    int cur = goal;
    int childTime = arriveT;
    while (pred[cur] != -1) {
        int p = pred[cur];
        int tp = earliest[p];
        for (int t = tp; t < childTime; ++t) path.set(t, p);
        cur = p;
        childTime = tp;
    }

    // Departure wait: times 0 .. t_start-1 sit at the start node.
    for (int t = 0; t < earliest[start]; ++t) path.set(t, start);

    // Idle after arrival.
    for (int t = arriveT + 1; t <= H; ++t) path.set(t, goal);

    return path;
}
