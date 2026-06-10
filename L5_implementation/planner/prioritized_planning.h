//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_PRIORITIZED_PLANNING_H
#define OHT_MAPF_PRIORITIZED_PLANNING_H

#include "pathfinding.h"
#include "reservation_table/reservation_table.h"
#include "../data_structure/graph.h"
#include "../environment/agent.h"

// PrioritizedPlanning (PP): plans one round for ALL agents.
// Agents are processed in priority order; each agent's H-step path is found by
// the injected PathFinder and then recorded into the reservation table, so the
// next (lower-priority) agent avoids it. Single-agent search is delegated to a
// PathFinder* (strategy pattern), so PP never knows which algorithm runs.
// Ownership: PathFinder* and Graph are borrowed. ReservationTable is owned by PP
// (cleared and refilled each round). Agents are owned by the Environment — PP
// only borrows Agent* into its MinHeap.
// Contract:  L3_interface/domain/planner/README.md (#PrioritizedPlanning)
// Mechanism: L4_mechanism/prioritized_planning.md
class PrioritizedPlanning {
public:
    PrioritizedPlanning(PathFinder* finder, const Graph& graph, int nodeCount);

    // Plan one round. out must point to n Path slots (caller-owned).
    // out[i] = agent i's H-step Path (visiting its goal queue with `dwell` work
    // steps per reached goal). Higher priority is planned first and reserved;
    // lower priority avoids it.
    void planRound(Agent* agents, int n, int H, int dwell, Path* out);

private:
    PathFinder* finder;             // injected single-agent search (non-owning)
    const Graph* graph;             // layout (non-owning)
    ReservationTable reservation;   // owned by PP; cleared -> filled each round

    // Record path's occupancy into the reservation so later agents avoid it.
    // Time t occupies node path.at(t) over [t, t+1); reserve() coalesces a
    // sustained stay into one interval. Empty path records nothing.
    void recordPath(const Path& path, int H);
};

#endif //OHT_MAPF_PRIORITIZED_PLANNING_H
