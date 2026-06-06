//
// Created by DohyunKim on 26. 6. 6..
//

#include "graph.h"

#include <cassert>

// ── Graph ───────────────────────────────────────────────────────────────

Graph::Graph(int n, int originalCount) : n(n) {
    assert(n >= 0 && "Graph: vertex count must be non-negative");
    this->originalCount = (originalCount < 0) ? n : originalCount;
    assert(this->originalCount <= n && "Graph: originalCount exceeds n");
    deg = new int[n];
    cap = new int[n];
    adj = new int*[n];
    for (int v = 0; v < n; ++v) {
        deg[v] = 0;
        cap[v] = 0;
        adj[v] = nullptr;   // allocate lazily on first edge
    }
}

Graph::~Graph() {
    for (int v = 0; v < n; ++v) {
        delete[] adj[v];
    }
    delete[] adj;
    delete[] deg;
    delete[] cap;
}

// Double the capacity of adj[v] (start at 4).
void Graph::growRow(int v) {
    int newCap = (cap[v] == 0) ? 4 : cap[v] * 2;
    int* row = new int[newCap];
    for (int i = 0; i < deg[v]; ++i) {
        row[i] = adj[v][i];
    }
    delete[] adj[v];
    adj[v] = row;
    cap[v] = newCap;
}

void Graph::addEdge(int from, int to) {
    assert(from >= 0 && from < n && "Graph::addEdge from out of range");
    assert(to   >= 0 && to   < n && "Graph::addEdge to out of range");
    if (deg[from] == cap[from]) {
        growRow(from);
    }
    adj[from][deg[from]] = to;
    ++deg[from];
}

int Graph::degree(int v) const {
    assert(v >= 0 && v < n && "Graph::degree out of range");
    return deg[v];
}

int Graph::neighbor(int v, int i) const {
    assert(v >= 0 && v < n && "Graph::neighbor vertex out of range");
    assert(i >= 0 && i < deg[v] && "Graph::neighbor index out of range");
    return adj[v][i];
}

int Graph::size() const {
    return n;
}

int Graph::originalSize() const {
    return originalCount;
}

bool Graph::isOriginal(int v) const {
    return v >= 0 && v < originalCount;
}

// ── TimeExpandedGraph ───────────────────────────────────────────────────

// Base Graph holds the unweighted layout; horizon adds the time axis.
TimeExpandedGraph::TimeExpandedGraph(int n, int horizon)
    : Graph(n), horizon(horizon) {
    assert(horizon >= 0 && "TimeExpandedGraph: horizon must be non-negative");
}

int TimeExpandedGraph::getHorizon() const {
    return horizon;
}
