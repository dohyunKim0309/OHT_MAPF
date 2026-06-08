// Verification: BellmanPhi vs BfsTeg must agree on reachability and earliest
// arrival time, and BellmanPhi paths must be valid (adjacent-or-wait, and avoid
// reserved cells). Not part of the product build — compiled standalone.
#include "planner/bfs_teg.h"
#include "planner/bellman_phi.h"
#include "planner/reservation_table/reservation_table.h"
#include "data_structure/graph.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

static Graph* load(const std::string& path, int orig) {
    std::ifstream in(path);
    if (!in) { std::fprintf(stderr, "cannot open %s\n", path.c_str()); return nullptr; }
    int n, m; in >> n >> m;
    Graph* g = new Graph(n, orig);
    for (int e = 0; e < m; ++e) { int a, b; in >> a >> b; g->addEdge(a, b); }
    return g;
}

// arrival time of a path = first index whose node == goal, else -1
static int arrivalTime(const Path& p, int goal) {
    if (p.empty()) return -1;
    for (int t = 0; t < p.length(); ++t) if (p.at(t) == goal) return t;
    return -1;
}

// validity: each step is wait (same node) or a real edge; and no node is on a
// reserved cell at its time.
static bool validPath(const Path& p, const Graph& g, const ReservationTable& rt) {
    if (p.empty()) return true;
    for (int t = 0; t + 1 < p.length(); ++t) {
        int u = p.at(t), v = p.at(t + 1);
        if (u == v) { /* wait */ }
        else {
            bool edge = false;
            for (int i = 0; i < g.degree(u); ++i) if (g.neighbor(u, i) == v) { edge = true; break; }
            if (!edge) return false;
        }
        if (!rt.isFree(p.at(t), t)) return false;
    }
    return rt.isFree(p.at(p.length() - 1), p.length() - 1);
}

int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "data/edges_expanded.txt";
    int H = argc > 2 ? std::atoi(argv[2]) : 600;
    int trials = argc > 3 ? std::atoi(argv[3]) : 200;
    Graph* g = load(path, 2858);
    if (!g) return 1;
    std::srand(42);

    BfsTeg bfs; BellmanPhi phi;
    int agree = 0, bothReach = 0, mismatch = 0, invalid = 0;

    for (int k = 0; k < trials; ++k) {
        int s = std::rand() % g->originalSize();
        int go = std::rand() % g->originalSize();
        // build a small random reservation to make phi/isFree do real work
        ReservationTable rt(g->size());
        for (int r = 0; r < 30; ++r) {
            int x = std::rand() % g->size();
            int t0 = std::rand() % (H > 1 ? H : 1);
            rt.reserve(x, t0, t0 + 1 + std::rand() % 3);
        }
        Path pa = bfs.findPath(s, go, *g, rt, H);
        Path pb = phi.findPath(s, go, *g, rt, H);
        int ta = arrivalTime(pa, go), tb = arrivalTime(pb, go);
        bool ra = !pa.empty(), rb = !pb.empty();
        if (ra == rb) ++agree; else ++mismatch;
        if (ra && rb) {
            ++bothReach;
            if (ta != tb) { ++mismatch; if (mismatch <= 5)
                std::printf("  arrival mismatch s=%d g=%d  bfs=%d phi=%d\n", s, go, ta, tb); }
        }
        if (!validPath(pb, *g, rt)) { ++invalid; if (invalid <= 5)
            std::printf("  INVALID phi path s=%d g=%d\n", s, go); }
    }

    std::printf("trials=%d  reachability-agree=%d  both-reached=%d  mismatches=%d  invalid-phi=%d\n",
                trials, agree, bothReach, mismatch, invalid);
    std::printf("%s\n", (mismatch == 0 && invalid == 0) ? "PASS" : "FAIL");
    delete g;
    return (mismatch == 0 && invalid == 0) ? 0 : 1;
}
