//
// Created by DohyunKim on 26. 6. 7..
//

#ifndef OHT_MAPF_BELLMAN_PHI_H
#define OHT_MAPF_BELLMAN_PHI_H

#include "pathfinding.h"   // PathFinder, NodeRef

// BellmanPhi: single-agent time-constrained shortest path WITHOUT time-expanding
// the graph. It relaxes earliest-arrival times over the (unweighted expanded)
// layout, SPFA-style, and avoids reservations at runtime via the interval query
// ReservationTable::phi. Same input as BfsTeg (expanded graph + reservation);
// the only difference is how time is handled — phi avoidance vs TEG expansion.
// Space is O(V) (no V*(H+1) time copy), the key contrast with BfsTeg.
// Contract:  L3_interface/domain/planner/bellman_phi.md
// Mechanism: L4_mechanism/bellman_phi.md
class BellmanPhi : public PathFinder {
public:
    BellmanPhi();
    ~BellmanPhi() override;

    Path findPath(int start, int goal,
                  const Graph& graph,
                  const ReservationTable& reservation,
                  int H) override;

private:
    // Reusable scratch, (re)allocated when the node count grows. Sized to V.
    int*     earliest;   // earliest[v] = earliest reachable time at v (INF = unset)
    int*     pred;       // pred[v] = previous node on the relaxing path (-1 = none)
    bool*    inQueue;    // dedup flag for SPFA
    NodeRef* pool;       // pool[v] = a NodeRef wrapping v, enqueued by pointer
    int      capacity;   // size of the above arrays

    void ensureCapacity(int n);
};

#endif //OHT_MAPF_BELLMAN_PHI_H
