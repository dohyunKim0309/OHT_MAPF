//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_ENVIRONMENT_H
#define OHT_MAPF_ENVIRONMENT_H

#include "agent.h"
#include "../data_structure/graph.h"
#include "../planner/prioritized_planning.h"

// Environment: holds the simulation world and advances time (receding horizon).
// Each round it asks PP for every agent's H-step plan, executes only the first
// C steps, counts arrivals (assigning a fresh random goal on arrival), and
// measures throughput. It does NOT plan or avoid collisions — those live in PP;
// the Environment just executes the collision-free paths PP returns.
// Ownership: owns the Agent array. Borrows Graph and PrioritizedPlanning*. The
// Path buffer PP fills is owned locally per round (move semantics free it).
// Contract:  L3_interface/domain/environment/README.md
// Mechanism: L4_mechanism/environment.md
class Environment {
public:
    // Build a world of agentCount agents on `graph`, planning with `pp`.
    // H = planning horizon, C = commit length (0 < C <= H). Agents start at
    // random original nodes with random original goals; priority = id.
    Environment(const Graph& graph, int agentCount, PrioritizedPlanning* pp,
                int H, int C);
    ~Environment();

    void step();               // one round: plan -> advance C steps -> handle arrivals
    void run(int rounds);      // step() repeated `rounds` times
    double throughput() const; // cumulative arrivals / elapsed time (0 if now == 0)
    int    arrivedCount() const; // total goals reached so far
    int    elapsed() const;      // elapsed time steps (now)
    void render() const;       // visualizer: print each agent's current node

private:
    Agent* agents;          // n agents, owned by this
    int n;                  // agent count
    const Graph* graph;     // layout (non-owning)
    PrioritizedPlanning* pp;// planner (non-owning)
    int H;                  // planning horizon
    int C;                  // commit length (steps actually executed per round)
    int now;                // elapsed time (accumulated steps)
    int arrived;            // cumulative arrival count

    // Draw a random original node in [0, graph.originalSize()) (no virtual nodes).
    int randomOriginalNode() const;
};

#endif //OHT_MAPF_ENVIRONMENT_H
