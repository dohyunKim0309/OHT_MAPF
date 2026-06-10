//
// Created by DohyunKim on 26. 6. 6..
//

#include "environment.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

Environment::Environment(const Graph& graph, int agentCount,
                         PrioritizedPlanning* pp, int H, int C, int dwell)
    : agents(nullptr), n(agentCount), graph(&graph), pp(pp),
      H(H), C(C), dwell(dwell), now(0), arrived(0) {
    assert(agentCount >= 0 && "Environment: negative agent count");
    assert(pp != nullptr && "Environment: needs a planner");
    assert(C > 0 && C <= H && "Environment: need 0 < C <= H");
    assert(dwell >= 0 && "Environment: negative dwell");
    assert(graph.originalSize() > 0 && "Environment: graph has no original nodes");

    if (n > 0) agents = new Agent[n];
    for (int i = 0; i < n; ++i) {
        // priority = id (fixed per-round order); random start; fill the goal queue
        // from the infinite random stream.
        agents[i] = Agent(i, i, randomOriginalNode());
        for (int g = 0; g < Agent::GOAL_CAP; ++g)
            agents[i].pushGoal(randomOriginalNode());
    }
}

Environment::~Environment() {
    delete[] agents;
}

void Environment::step() {
    if (n == 0) { now += C; return; }

    // PP fills one Path per agent (length H+1). Owned locally; freed on round end.
    Path* paths = new Path[n];   // each default-constructs to an empty Path

    pp->planRound(agents, n, H, dwell, paths);

    // Execute the first C steps of each plan.
    for (int c = 1; c <= C; ++c) {
        for (int i = 0; i < n; ++i) {
            if (paths[i].empty()) continue;        // can't leave start: stay put
            int node = paths[i].at(c);
            // A -1 cell means the path was truncated here: at this step the agent
            // had no collision-free cell to move to (its own node became occupied
            // by a higher-priority agent in a later round, or the goal was
            // unreachable within H). The agent simply HOLDS its last real position
            // — which PP already reserved as occupancy up to that step, so this
            // stay is consistent with the reservation table and constrains
            // lower-priority agents exactly as any other occupied cell does. We
            // never write -1 as a position (that would corrupt the next round's
            // start and the isFree queries).
            if (node < 0) continue;                // hold current node this step
            agents[i].setCurrent(node);            // advance to time-c node
            // Reached the current goal? consume it and refill the stream.
            if (agents[i].getCurrent() == agents[i].currentGoal()) {
                arrived += 1;
                agents[i].popGoal();
                agents[i].pushGoal(randomOriginalNode());  // keep the queue full
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
        std::cout << "    agent " << agents[i].getId()
                  << " : node " << agents[i].getCurrent()
                  << " -> goal " << agents[i].currentGoal()
                  << "\n";
    }
}

int Environment::randomOriginalNode() const {
    return std::rand() % graph->originalSize();
}
