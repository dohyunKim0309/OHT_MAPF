//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_BFS_TEG_H
#define OHT_MAPF_BFS_TEG_H

#include "planner.h"

// BfsTeg: single-agent time-constrained shortest path via a Time-Expanded
// Graph + plain BFS.
// The TEG (vertices (node,time) = node*(H+1)+t, edges = move/wait) depends only
// on the layout + horizon, NOT on reservations, so it is built ONCE and reused
// across all agents in a PP round (held as a member). Reservations are absorbed
// at SEARCH time: BFS skips any (node,t) that is occupied (isFree == false).
// First arrival at the goal is earliest (unweighted). Non-owning of the
// borrowed graph/reservation; owns its TEG (RAII member).
// Contract:  L3_interface/domain/planner/bfs_teg.md
// Mechanism: L4_mechanism/bfs_teg.md
class BfsTeg : public PathFinder {
public:
    BfsTeg();
    ~BfsTeg() override;

    Path findPath(int start, int goal,
                  const Graph& graph,
                  const ReservationTable& reservation,
                  int H) override;

private:
    TimeExpandedGraph* teg;   // built once (lazy), reused; owned by this
    int builtH;               // horizon the current teg was built for (-1 = none)

    // Search scratch, allocated once (size = TEG vertex count) and reused.
    // visitedGen[v] == gen means "visited in the current search" — bumping gen
    // invalidates all marks in O(1) instead of clearing the whole array.
    int* visitedGen;          // generation stamp per vertex
    int* pred;                // predecessor vertexId (valid only if visited this gen)
    int* q;                   // BFS queue buffer
    int  scratchSize;         // size of the above arrays (= Vt)
    int  gen;                 // current search generation

    // Build the TEG for this graph/horizon (layout only, reservation-free).
    void buildTEG(const Graph& graph, int H);
    // Ensure scratch arrays are at least `size`; (re)allocate if needed.
    void ensureScratch(int size);
};

#endif //OHT_MAPF_BFS_TEG_H
