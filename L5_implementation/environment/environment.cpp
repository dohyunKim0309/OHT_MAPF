//
// Created by DohyunKim on 26. 6. 6..
//

#include "environment.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

Environment::Environment(const Graph& graph, int agentCount,
                         PrioritizedPlanning* pp, int H, int C)
    : agents(nullptr), n(agentCount), graph(&graph), pp(pp),
      H(H), C(C), now(0), arrived(0) {
    assert(agentCount >= 0 && "Environment: negative agent count");
    assert(pp != nullptr && "Environment: needs a planner");
    assert(C > 0 && C <= H && "Environment: need 0 < C <= H");
    assert(graph.originalSize() > 0 && "Environment: graph has no original nodes");

    if (n > 0) agents = new Agent[n];
    for (int i = 0; i < n; ++i) {
        // priority = id (fixed per-round order); random start and goal.
        agents[i] = Agent(i, i, randomOriginalNode(), randomOriginalNode());
    }
}

Environment::~Environment() {
    delete[] agents;
}

void Environment::step() {
    if (n == 0) { now += C; return; }

    // PP fills one Path per agent (length H+1). Owned locally; freed on round end.
    Path* paths = new Path[n];   // each default-constructs to an empty Path

    pp->planRound(agents, n, H, paths);

    // Execute the first C steps of each plan.
    for (int c = 1; c <= C; ++c) {
        for (int i = 0; i < n; ++i) {
            if (paths[i].empty()) continue;        // unreachable: stay put
            agents[i].setCurrent(paths[i].at(c));  // advance to time-c node
            if (agents[i].getCurrent() == agents[i].getGoal()) {
                arrived += 1;
                agents[i].setGoal(randomOriginalNode());  // applied next round
            }
        }
    }
    now += C;

    delete[] paths;
}

void Environment::run(int rounds) {
    for (int r = 0; r < rounds; ++r) step();
}

double Environment::throughput() const {
    // Cumulative goals reached per elapsed time step. Real-valued: a small
    // fleet typically reaches well under one goal per step.
    return now == 0 ? 0.0 : static_cast<double>(arrived) / now;
}

int Environment::arrivedCount() const { return arrived; }

int Environment::elapsed() const { return now; }

void Environment::render() const {
    std::cout << "  t=" << now << ", arrived=" << arrived << "\n";
    for (int i = 0; i < n; ++i) {
        bool atGoal = agents[i].getCurrent() == agents[i].getGoal();
        std::cout << "    agent " << agents[i].getId()
                  << " : node " << agents[i].getCurrent()
                  << " -> goal " << agents[i].getGoal()
                  << (atGoal ? "   [at goal]" : "")
                  << "\n";
    }
}

int Environment::randomOriginalNode() const {
    return std::rand() % graph->originalSize();
}
