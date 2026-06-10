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

    Path findPath(int start,
                  const int* goals, int goalCount, int dwell,
                  const Graph& graph,
                  const ReservationTable& reservation,
                  int H) override;

private:
    // One reconstructed step: a node and the time the agent ENTERS it.
    struct Step { int node; int enter; };

    // Reusable scratch, (re)allocated when the node count grows. Sized to V.
    // Per-segment reset via a generation stamp (gen) — no O(V) clear each segment.
    int*     earliest;   // earliest[v] = earliest reachable time at v (this gen)
    int*     pred;       // pred[v] = previous node on the relaxing path
    int*     seen;       // seen[v] == gen  ⇒  earliest[v]/pred[v] valid this segment
    bool*    inQueue;    // dedup flag for SPFA (also gen-guarded)
    NodeRef* pool;       // pool[v] = a NodeRef wrapping v, enqueued by pointer
    Step*    pathBuf;    // reconstruction buffer (one segment's node chain), sized V
    int      capacity;   // size of the above arrays
    int      gen;        // current relaxation generation

    void ensureCapacity(int n);
    // Relax from (src @ t0) to `goal`, no horizon cap. Fills earliest/pred for this
    // generation; returns the arrival time at goal (INF if truly unreachable).
    int relaxToGoal(int src, int t0, int goal,
                    const Graph& graph, const ReservationTable& reservation);
};

#endif //OHT_MAPF_BELLMAN_PHI_H
