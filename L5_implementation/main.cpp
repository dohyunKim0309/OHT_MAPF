//
// OHT_MAPF — receding-horizon prioritized planning on a real OHT layout.
// Loads the SMAT 2022 semiconductor-fab OHT network (unweighted expanded form),
// runs the Environment with BfsTeg as the single-agent path finder, and reports
// throughput. See L3_interface/domain/{environment,planner} and L4_mechanism/*.
//

#include "environment/environment.h"
#include "planner/prioritized_planning.h"
#include "planner/bfs_teg.h"
#include "planner/bellman_phi.h"
#include "data_structure/graph.h"

#include <cstring>

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
// File format: first line "N M" (vertices, edges), then M lines "from to"
// (0-indexed). Original nodes are [0, originalCount); virtual nodes follow.
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

int main(int argc, char** argv) {
    using clock = std::chrono::steady_clock;

    std::srand(7);   // fixed seed: reproducible random start/goal placement

    // ── Configuration ───────────────────────────────────────────────────────
    const int   originalNodes = 2858;            // SMAT 2022 original OHT nodes
    const std::string layoutPath =
        (argc > 1) ? argv[1] : "data/edges_expanded.txt";
    // Light defaults so a no-argument run from the IDE finishes in a few seconds.
    // Args:  OHT_MAPF <layout> <agents> <H> <C> <rounds> <algo>
    //   <algo> = "bfs" (BFS+TEG, default) | "phi" (phi Bellman-Ford)
    // e.g.:  OHT_MAPF data/edges_expanded.txt 40 500 50 10 phi
    const int   agents = (argc > 2) ? std::atoi(argv[2]) : 50;
    const int   H      = (argc > 3) ? std::atoi(argv[3]) : 1000;  // planning horizon
    const int   C      = (argc > 4) ? std::atoi(argv[4]) : 200;   // commit length
    const int   rounds = (argc > 5) ? std::atoi(argv[5]) : 5;    // simulation rounds
    const bool  usePhi = (argc > 6) && std::strcmp(argv[6], "phi") == 0;

    Graph* g = loadExpandedGraph(layoutPath, originalNodes);
    if (!g) return 1;

    // Strategy pattern: pick the PathFinder; PP and Environment are unchanged.
    BfsTeg     bfs;
    BellmanPhi phi;
    PathFinder* finder = usePhi ? static_cast<PathFinder*>(&phi)
                                : static_cast<PathFinder*>(&bfs);
    const char* algorithm = usePhi ? "phi Bellman-Ford (no time-expansion)"
                                   : "BFS + Time-Expanded Graph";
    PrioritizedPlanning pp(finder, *g, g->size());
    Environment env(*g, agents, &pp, H, C);

    // ── Banner ──────────────────────────────────────────────────────────────
    std::cout << "==================================================\n";
    std::cout << "  OHT_MAPF  —  Prioritized Planning on SMAT 2022\n";
    std::cout << "==================================================\n";
    std::cout << "  Path finder      : " << algorithm << "\n";
    std::cout << "  Layout           : " << g->originalSize()
              << " original + " << (g->size() - g->originalSize())
              << " virtual = " << g->size() << " nodes\n";
    std::cout << "  Agents           : " << agents << "\n";
    std::cout << "  Horizon  (H)     : " << H << " steps planned per round\n";
    std::cout << "  Commit   (C)     : " << C << " steps executed per round\n";
    std::cout << "  Rounds           : " << rounds
              << "  (= " << rounds * C << " time steps)\n";
    std::cout << "--------------------------------------------------\n";

    auto t0 = clock::now();
    env.run(rounds);
    auto t1 = clock::now();
    double secs = std::chrono::duration<double>(t1 - t0).count();

    // ── Results ───────────────────────────────────────────────────────────
    std::cout << "  Goals reached    : " << env.arrivedCount() << "\n";
    std::cout << "  Time elapsed     : " << env.elapsed() << " steps\n";
    std::cout << "  Throughput       : " << std::fixed << std::setprecision(4)
              << env.throughput() << " goals / step\n";
    std::cout << "  Wall-clock       : " << std::setprecision(2) << secs << " s  ("
              << std::setprecision(1) << (1000.0 * secs / rounds)
              << " ms / round)\n";
    std::cout << "==================================================\n";

    delete g;
    return 0;
}
