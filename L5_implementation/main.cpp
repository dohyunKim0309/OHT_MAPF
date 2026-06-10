//
// OHT_MAPF — receding-horizon prioritized planning on a real OHT layout.
// Loads the SMAT 2022 semiconductor-fab OHT network (unweighted expanded form),
// runs the Environment with BfsTeg as the single-agent path finder, reports
// throughput, and runs a direct all-pairs collision check over one planning
// round's committed paths (the report's correctness figure).
// See L3_interface/domain/{environment,planner} and L4_mechanism/*.
//

#include "environment/environment.h"
#include "environment/agent.h"
#include "planner/prioritized_planning.h"
#include "planner/bfs_teg.h"
#include "planner/pathfinding.h"
#include "data_structure/graph.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

// Try to open `path` directly, then from a few parent directories. CLion runs
// the binary from cmake-build-debug/, so the project-root-relative "data/..."
// path is not found unless we also look one or two levels up.
static std::ifstream openLayout(const std::string& path, std::string& usedPath) {
    const std::string prefixes[] = {"", "../", "../../", "../../../"};
    for (const std::string& p : prefixes) {
        std::string candidate = p + path;
        std::ifstream in(candidate);
        if (in) { usedPath = candidate; return in; }
    }
    return std::ifstream();   // not found: caller checks
}

// Load the unweighted expanded layout from data/edges_expanded.txt.
// File format: first line "N M" (vertices, edges), then M lines "from to".
static Graph* loadExpandedGraph(const std::string& path, int originalCount) {
    std::string usedPath;
    std::ifstream in = openLayout(path, usedPath);
    if (!in) {
        std::cerr << "ERROR: cannot open " << path
                  << " (also tried ../, ../../). Run from the project root or"
                  << " pass the path as the first argument.\n";
        return nullptr;
    }
    int n, m;
    in >> n >> m;
    Graph* g = new Graph(n, originalCount);
    for (int e = 0; e < m; ++e) {
        int from, to;
        in >> from >> to;
        g->addEdge(from, to);
    }
    return g;
}

// Build a fresh agent array with UNIQUE start nodes (a shuffled prefix of the
// original nodes) under a fixed seed, so two agents never start on the same node
// (a same-start would be a trivial t=0 overlap that is a scenario artifact, not a
// planner fault). Goals are a random stream. priority = id (stable order).
// Requires agents <= originalSize().
static Agent* makeAgents(const Graph& g, int agents, unsigned seed) {
    std::srand(seed);
    int origN = g.originalSize();
    int* perm = new int[origN];
    for (int i = 0; i < origN; ++i) perm[i] = i;
    for (int i = origN - 1; i > 0; --i) {       // Fisher-Yates shuffle
        int j = std::rand() % (i + 1);
        int tmp = perm[i]; perm[i] = perm[j]; perm[j] = tmp;
    }
    Agent* a = new Agent[agents];
    for (int i = 0; i < agents; ++i) {
        int start = perm[i % origN];            // unique while agents <= origN
        a[i] = Agent(i, i, start);
        for (int k = 0; k < Agent::GOAL_CAP; ++k) a[i].pushGoal(std::rand() % origN);
    }
    delete[] perm;
    return a;
}

// Plan ONE round with BFS+TEG and check every committed path for a vertex
// collision: two agents on the same node at the same time step. Returns the
// number of collisions found (0 = collision-free) and fills planning time.
static long collisionCheckOneRound(const Graph& g, int agents, int H, int dwell,
                                   unsigned seed, double& planMs) {
    BfsTeg bfs;
    PrioritizedPlanning pp(&bfs, g, g.size());
    Agent* a = makeAgents(g, agents, seed);
    Path* out = new Path[agents];

    using clock = std::chrono::steady_clock;
    auto t0 = clock::now();
    pp.planRound(a, agents, H, dwell, out);
    auto t1 = clock::now();
    planMs = 1000.0 * std::chrono::duration<double>(t1 - t0).count();

    // Exact occupancy ledger: occ[node*(H+1)+t] = agent id, -1 if free.
    long N = (long)g.size() * (H + 1);
    int* occ = new int[N];
    for (long i = 0; i < N; ++i) occ[i] = -1;

    long collisions = 0;
    for (int i = 0; i < agents; ++i) {
        if (out[i].empty()) continue;
        for (int t = 0; t <= H; ++t) {
            int node = out[i].at(t);
            if (node < 0) continue;
            long key = (long)node * (H + 1) + t;
            if (occ[key] >= 0) ++collisions;   // someone already here at this step
            else occ[key] = i;
        }
    }

    delete[] occ;
    delete[] out;
    delete[] a;
    return collisions;
}

int main(int argc, char** argv) {
    const int   originalNodes = 2858;            // SMAT 2022 original OHT nodes
    const std::string layoutPath =
        (argc > 1) ? argv[1] : "data/edges_expanded.txt";
    // Args:  OHT_MAPF <layout> <agents> <H> <C> <rounds> <dwell>
    // REPORT capture: many agents (more contention), modest H. e.g.:
    //   OHT_MAPF data/edges_expanded.txt 600 1000 200 10 1
    const int   agents = (argc > 2) ? std::atoi(argv[2]) : 500;
    const int   H      = (argc > 3) ? std::atoi(argv[3]) : 1000;  // planning horizon
    const int   C      = (argc > 4) ? std::atoi(argv[4]) : 200;   // commit length
    const int   rounds = (argc > 5) ? std::atoi(argv[5]) : 10;    // simulation rounds
    const int   dwell  = (argc > 6) ? std::atoi(argv[6]) : 1;     // work steps per goal
    const unsigned SEED = 7;

    Graph* g = loadExpandedGraph(layoutPath, originalNodes);
    if (!g) return 1;

    std::cout << "==================================================\n";
    std::cout << "  OHT_MAPF  —  BFS+TEG Prioritized Planning, SMAT 2022\n";
    std::cout << "==================================================\n";
    std::cout << "  Layout   : " << g->originalSize() << " original + "
              << (g->size() - g->originalSize()) << " virtual = " << g->size()
              << " nodes\n";
    std::cout << "  Agents   : " << agents << "   Horizon H : " << H
              << "   Commit C : " << C << "   Rounds : " << rounds
              << "   dwell : " << dwell << "\n";
    std::cout << "  TEG size : " << (long)g->size() * (H + 1)
              << " vertices  (V x (H+1))\n";
    std::cout << "--------------------------------------------------\n";

    // (1) Correctness: one planning round, all-pairs collision check.
    double planMs = 0;
    long collisions = collisionCheckOneRound(*g, agents, H, dwell, SEED, planMs);
    std::cout << "  Collision check (1 round, all committed paths): "
              << collisions << " collisions  ->  "
              << (collisions == 0 ? "COLLISION-FREE" : "FAILED") << "\n";
    std::cout << "  Plan time (1 round)     : " << std::fixed
              << std::setprecision(1) << planMs << " ms\n";
    std::cout << "--------------------------------------------------\n";

    // (2) Throughput: full receding-horizon simulation.
    {
        using clock = std::chrono::steady_clock;
        std::srand(SEED);
        BfsTeg bfs;
        PrioritizedPlanning pp(&bfs, *g, g->size());
        Environment env(*g, agents, &pp, H, C, dwell);
        auto t0 = clock::now();
        env.run(rounds);
        auto t1 = clock::now();
        double secs = std::chrono::duration<double>(t1 - t0).count();
        std::cout << "  Goals reached  : " << env.arrivedCount() << "\n";
        std::cout << "  Throughput     : " << std::fixed << std::setprecision(4)
                  << env.throughput() << "  goals/step\n";
        std::cout << "  Time / round   : " << std::setprecision(1)
                  << (1000.0 * secs / rounds) << " ms\n";
    }
    std::cout << "==================================================\n";

    delete g;
    return 0;
}
