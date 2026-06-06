//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_GRAPH_H
#define OHT_MAPF_GRAPH_H

// Graph: a directed, unweighted graph (every edge has length 1).
// Layout distances are absorbed into virtual nodes during preprocessing
// (data/edges_expanded.txt), so the graph itself stores no weights.
// Vertices are integer indices 0..n-1; an edge is just a neighbor index.
// Unlike the other data structures, Graph is NOT a Data* container — it
// holds plain ints, so no polymorphism / heap-per-element. (Documented
// exception to "every structure is a Data container".)
// Design: L3_interface/data_structure/graph.md
class Graph {
public:
    // n vertices total, no edges. originalCount = number of original (non-virtual)
    // nodes; indices [0, originalCount) are original, [originalCount, n) are
    // virtual nodes inserted by the unweighted expansion. Default originalCount
    // = n (no virtual nodes), e.g. for a TimeExpandedGraph.
    explicit Graph(int n, int originalCount = -1);

    // Release adjacency storage.
    virtual ~Graph();

    // Add a directed unit edge from -> to.
    void addEdge(int from, int to);

    // Number of outgoing neighbors of vertex v.
    int degree(int v) const;

    // The i-th outgoing neighbor of vertex v (0 <= i < degree(v)).
    int neighbor(int v, int i) const;

    // Number of vertices (original + virtual).
    int size() const;

    // Number of original nodes. New goals are drawn from [0, originalSize());
    // virtual nodes (edge midpoints) cannot be goals.
    int originalSize() const;

    // Is v an original (non-virtual) node?
    bool isOriginal(int v) const;

protected:
    int n;             // number of vertices
    int originalCount; // first originalCount indices are original nodes
    int* deg;          // deg[v] = current out-degree of v
    int* cap;          // cap[v] = capacity of adj[v]
    int** adj;         // adj[v] = array of neighbor indices (length deg[v])

private:
    // Grow adj[v] (double its capacity) when full.
    void growRow(int v);
};

// TimeExpandedGraph: a Graph viewed over discrete time (TEG).
// A vertex becomes (node, time). Transitions per step:
//   move : (u, t) -> (v, t+1)  when base edge u->v exists
//   wait : (u, t) -> (u, t+1)
// Time runs 0..horizon. Occupied (node, time) cells are removed (a
// reservation filter) so search above it is plain BFS that ignores
// reservations. Whether the TEG is materialized explicitly, and how
// occupancy is supplied, is decided in the planner branch.
// Design: L3_interface/data_structure/graph.md
class TimeExpandedGraph : public Graph {
public:
    TimeExpandedGraph(int n, int horizon);

    int getHorizon() const;

private:
    int horizon;      // time layers 0..horizon
};

#endif //OHT_MAPF_GRAPH_H
