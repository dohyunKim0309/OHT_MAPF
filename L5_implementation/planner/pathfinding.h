//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_PLANNER_H
#define OHT_MAPF_PLANNER_H

#include "../data_structure/graph.h"
#include "../data_structure/data.h"
#include "reservation_table/reservation_table.h"

// NodeRef: a one-cell Data wrapper holding an integer vertex id, so plain ints
// can ride the (Data*-based) Queue during a search. No operator< (the search
// queues are FIFO and never compare). A PathFinder owns a reusable pool of these.
// Used by both BfsTeg (TEG vertex ids) and BellmanPhi (layout node ids).
class NodeRef : public Data {
public:
    NodeRef() : id(-1) {}
    int id;
};

// Path: a fixed-length (H+1) sequence of node indices over time.
// path.at(t) = the node occupied at time t. Staying put = same node repeated
// (a wait, or a work-dwell at a reached goal). The path visits a SEQUENCE of
// goals: at each goal the agent dwells `dwell` steps (models an OHT loading/
// unloading) and then heads for the next goal, so the agent never idles forever.
// RAII: owns its int[] buffer and frees it in the destructor.
// An empty path (length 0) means "the agent cannot even leave its start".
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

// PathFinder: strategy interface for single-agent, time-constrained path finding
// over a SEQUENCE of goals. PrioritizedPlanning (PP) holds a PathFinder* and calls
// findPath without knowing which algorithm it is.
// Concrete: BfsTeg (TEG + BFS), BellmanPhi (phi Bellman-Ford).
// Contract: L3_interface/domain/planner/{planner,bfs_teg,bellman_phi}.md
class PathFinder {
public:
    virtual ~PathFinder() {}

    // Plan one agent's route through an ordered list of goals and return its
    // first H+1 steps.
    //  start              : current node (original index < graph.originalSize())
    //  goals, goalCount   : the next goals to visit, IN ORDER. The agent heads for
    //                       goals[0]; on reaching it, dwells `dwell` steps (work),
    //                       then heads for goals[1], and so on.
    //  dwell              : work-dwell steps spent at each reached goal (>= 0).
    //  graph              : unweighted expanded layout
    //  reservation        : read-only; occupied (node,time) cells to avoid
    //  H                  : horizon = how many steps to RETURN (receding window)
    // Returns a length-(H+1) Path filling times 0..H with where the agent is:
    // moving toward the current goal, or dwelling at a just-reached goal. Because
    // the agent always moves on after a finite dwell, it NEVER idles forever — so a
    // lower-priority agent's reserved cells are always finite intervals that
    // higher-priority agents (planned earlier) can be made to avoid. If the goal
    // list is exhausted before H (goals too few/close), the tail idles at the last
    // goal. An empty Path (length 0) means only that the agent cannot leave start.
    // The caller (Environment) must pass enough goals to cover H; leftover goals
    // beyond step H are simply not used this round (replanned next round).
    // Search reach is an implementation choice: BellmanPhi plans to each goal with
    // no horizon cap (cost O(V)); BfsTeg is bounded by its materialized H-step TEG.
    // Side-effect-free: reads reservation, does NOT record into it (PP does).
    virtual Path findPath(int start,
                          const int* goals, int goalCount, int dwell,
                          const Graph& graph,
                          const ReservationTable& reservation,
                          int H) = 0;
};

#endif //OHT_MAPF_PLANNER_H
