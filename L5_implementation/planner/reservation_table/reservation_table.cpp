//
// Created by DohyunKim on 26. 6. 6..
//

#include "reservation_table.h"

#include <cassert>

// ── Interval ──────────────────────────────────────────────────────────────

Interval::Interval(int start, int end) : start(start), end(end) {
    assert(start < end && "Interval: empty/negative range");
}

int Interval::getStart() const { return start; }
int Interval::getEnd() const { return end; }

// Order by start point.
bool Interval::operator<(const Data& other) const {
    return start < static_cast<const Interval&>(other).start;
}

// Touch or overlap: ranges meet if neither lies strictly past the other.
// [2,4) and [4,5) touch (4 == 4), so "<=" on the boundary.
bool Interval::touches(const Interval& other) const {
    return start <= other.end && other.start <= end;
}

// Merged range spans both.
Interval Interval::merge(const Interval& other) const {
    int s = (start < other.start) ? start : other.start;
    int e = (end > other.end) ? end : other.end;
    return Interval(s, e);
}

// ── ReservationTable ──────────────────────────────────────────────────────

ReservationTable::ReservationTable(int nodeCount) : n(nodeCount) {
    assert(n >= 0 && "ReservationTable: node count must be non-negative");
    trees = new AvlTree[n];   // each default-constructs to an empty tree
}

ReservationTable::~ReservationTable() {
    for (int x = 0; x < n; ++x) {
        trees[x].clearAndDelete();   // delete owned Interval* too
    }
    delete[] trees;
}

// Coalesce [start,end) into node x's tree, absorbing any touching neighbors.
void ReservationTable::reserve(int x, int start, int end) {
    assert(x >= 0 && x < n && "reserve: node index out of range");
    AvlTree& tree = trees[x];

    int s = start, e = end;

    // Absorb left neighbors while they touch.
    while (true) {
        Interval probe(s, e);
        Data* p = tree.predecessor(&probe);   // largest start < s
        if (p == nullptr) break;
        Interval* pred = static_cast<Interval*>(p);
        if (!pred->touches(Interval(s, e))) break;
        Interval merged = pred->merge(Interval(s, e));
        s = merged.getStart(); e = merged.getEnd();
        tree.remove(pred);        // detach from tree (non-owning)
        delete pred;              // we owned it
    }

    // Absorb right neighbors while they touch.
    while (true) {
        Interval probe(s, e);
        Data* nx = tree.successor(&probe);    // smallest start > s
        if (nx == nullptr) break;
        Interval* succ = static_cast<Interval*>(nx);
        if (!succ->touches(Interval(s, e))) break;
        Interval merged = succ->merge(Interval(s, e));
        s = merged.getStart(); e = merged.getEnd();
        tree.remove(succ);
        delete succ;
    }

    // There may also be an interval with the exact same start as the merged one
    // (predecessor/successor are strict). find() catches that overlap.
    {
        Interval probe(s, e);
        Data* same = tree.find(&probe);
        if (same != nullptr) {
            Interval* hit = static_cast<Interval*>(same);
            Interval merged = hit->merge(Interval(s, e));
            s = merged.getStart(); e = merged.getEnd();
            tree.remove(hit);
            delete hit;
        }
    }

    tree.insert(new Interval(s, e));   // owned by the table
}

bool ReservationTable::isFree(int x, int t) const {
    assert(x >= 0 && x < n && "isFree: node index out of range");
    // The interval that could contain t is the one with the largest start <= t.
    // predecessor(probe) gives largest start < t; also check an interval
    // starting exactly at t via successor/find on a [t, t+1) probe.
    Interval probe(t, t + 1);
    Data* p = trees[x].predecessor(&probe);
    if (p != nullptr) {
        Interval* iv = static_cast<Interval*>(p);
        if (iv->getStart() <= t && t < iv->getEnd()) return false;
    }
    // interval starting exactly at t
    Data* same = trees[x].find(&probe);
    if (same != nullptr) {
        Interval* iv = static_cast<Interval*>(same);
        if (iv->getStart() <= t && t < iv->getEnd()) return false;
    }
    return true;
}

int ReservationTable::phi(int x, int t) const {
    assert(x >= 0 && x < n && "phi: node index out of range");
    // Walk forward: if t is inside some interval [c,d), jump to d and recheck.
    // Intervals are disjoint, so this advances at most across consecutive ones.
    int cur = t;
    while (true) {
        Interval probe(cur, cur + 1);
        bool blocked = false;

        Data* p = trees[x].predecessor(&probe);
        if (p != nullptr) {
            Interval* iv = static_cast<Interval*>(p);
            if (iv->getStart() <= cur && cur < iv->getEnd()) { cur = iv->getEnd(); blocked = true; }
        }
        if (!blocked) {
            Data* same = trees[x].find(&probe);
            if (same != nullptr) {
                Interval* iv = static_cast<Interval*>(same);
                if (iv->getStart() <= cur && cur < iv->getEnd()) { cur = iv->getEnd(); blocked = true; }
            }
        }
        if (!blocked) return cur;   // cur is enterable
    }
}

void ReservationTable::clear() {
    for (int x = 0; x < n; ++x) {
        trees[x].clearAndDelete();
    }
}

int ReservationTable::size() const { return n; }
