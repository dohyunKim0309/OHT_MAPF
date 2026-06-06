//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_RESERVATION_TABLE_H
#define OHT_MAPF_RESERVATION_TABLE_H

#include "../../data_structure/data.h"
#include "../../data_structure/avl_tree.h"

// Interval: a half-open occupied time range [start, end) on one graph node.
// A concrete Data child (lives in the reservation_table module). Sorted in the
// avl_tree by start (operator<). touches/merge are Interval-only methods that
// ReservationTable calls to coalesce adjacent/overlapping ranges; the avl_tree
// itself does not know them.
// Contract: L3_interface/domain/planner/reservation_table/README.md
class Interval : public Data {
public:
    Interval(int start, int end);

    int getStart() const;
    int getEnd() const;

    // Order by start. Basis for the avl_tree's sort/search.
    bool operator<(const Data& other) const override;

    // Do the two ranges touch or overlap? ("touching" counts: [2,4)+[4,5)).
    bool touches(const Interval& other) const;

    // The merged range [min start, max end). Caller ensures touches() first.
    Interval merge(const Interval& other) const;

private:
    int start;   // inclusive
    int end;     // exclusive
};

// ReservationTable: per-node occupied-time store. Two layers:
//   outer : one avl_tree per graph node (indexed by node id)
//   inner : that node's Intervals, kept sorted & coalesced
// Owns the Interval* it creates; delegates bulk cleanup to AvlTree::clearAndDelete.
// Contract: L3_interface/domain/planner/reservation_table/README.md
class ReservationTable {
public:
    explicit ReservationTable(int nodeCount);
    ~ReservationTable();

    // Record that node x is occupied during [start, end). Coalesces with any
    // touching neighbors so the node's intervals stay disjoint & sorted.
    void reserve(int x, int start, int end);

    // Is node x free at time t? (no interval contains t) — point query (BFS+TEG).
    bool isFree(int x, int t) const;

    // Earliest time >= t at which node x is enterable — interval query (ϕ-BF).
    // If t is free, returns t; if t falls in [c,d), returns d (then re-checks).
    int phi(int x, int t) const;

    // Clear all reservations (delete every Interval), keep the table reusable.
    void clear();

    int size() const;   // node count

private:
    int n;            // number of graph nodes
    AvlTree* trees;   // trees[x] = node x's interval tree (array of n trees)
};

#endif //OHT_MAPF_RESERVATION_TABLE_H
