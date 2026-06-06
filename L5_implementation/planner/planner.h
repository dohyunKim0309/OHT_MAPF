//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_PLANNER_H
#define OHT_MAPF_PLANNER_H

#include "../data_structure/graph.h"
#include "reservation_table/reservation_table.h"

// Path: a fixed-length (H+1) sequence of node indices over time.
// path.at(t) = the node occupied at time t. Staying put = same node repeated;
// after reaching the goal the rest is filled with the goal node (idle).
// RAII: owns its int[] buffer and frees it in the destructor.
// An empty path (length 0) means "no path found within the horizon".
class Path {
public:
    Path();                      // empty path (length 0); lets callers make a Path[]
    explicit Path(int length);   // length = H+1; allocates int[length]
    ~Path();

    // movable (so findPath can return by value cheaply), non-copyable
    Path(Path&& other) noexcept;
    Path& operator=(Path&& other) noexcept;
    Path(const Path&) = delete;
    Path& operator=(const Path&) = delete;

    int at(int t) const;             // node at time t
    void set(int t, int node);       // builder-side write
    int length() const;
    bool empty() const;

private:
    int* nodes;   // nodes[t] = node index at time t
    int len;      // = H+1 (0 means empty)
};

// PathFinder: strategy interface for single-agent, time-constrained path finding.
// PrioritizedPlanning (PP) holds a PathFinder* and calls findPath without
// knowing which algorithm it is.
// Concrete: BfsTeg (TEG + BFS), BellmanPhi (phi Bellman-Ford).
// Contract: L3_interface/domain/planner/README.md (#PathFinder 추상 계약)
class PathFinder {
public:
    virtual ~PathFinder() {}

    // Find a time-constrained shortest path for one agent.
    //  start, goal : original node indices (< graph.originalSize())
    //  graph       : unweighted expanded layout
    //  reservation : read-only; occupied (node,time) cells to avoid
    //  H           : horizon (time steps 0..H)
    // Returns a length-(H+1) Path, or an empty Path if none within H.
    // Side-effect-free: reads reservation, does NOT record into it (PP does).
    virtual Path findPath(int start, int goal,
                          const Graph& graph,
                          const ReservationTable& reservation,
                          int H) = 0;
};

#endif //OHT_MAPF_PLANNER_H
