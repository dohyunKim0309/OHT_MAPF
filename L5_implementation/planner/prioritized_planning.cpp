//
// Created by DohyunKim on 26. 6. 6..
//

#include "prioritized_planning.h"

#include "../data_structure/min_heap.h"

#include <cassert>
#include <utility>

PrioritizedPlanning::PrioritizedPlanning(PathFinder* finder, const Graph& graph,
                                         int nodeCount)
    : finder(finder), graph(&graph), reservation(nodeCount) {
    assert(finder != nullptr && "PP: needs a PathFinder");
}

void PrioritizedPlanning::planRound(Agent* agents, int n, int H, Path* out) {
    assert((n == 0 || agents != nullptr) && "PP: null agents");
    assert((n == 0 || out != nullptr) && "PP: null out buffer");
    assert(H >= 0 && "PP: negative horizon");

    reservation.clear();   // drop last round's occupancy

    // Order agents by priority (MinHeap pops the smallest priority first).
    MinHeap heap;
    for (int i = 0; i < n; ++i) heap.push(&agents[i]);

    while (!heap.isEmpty()) {
        Agent* a = static_cast<Agent*>(heap.pop());
        Path path = finder->findPath(a->getCurrent(), a->getGoal(),
                                     *graph, reservation, H);
        recordPath(path, H);             // reserve before moving the Path out
        out[a->getId()] = std::move(path);
    }
}

void PrioritizedPlanning::recordPath(const Path& path, int H) {
    if (path.empty()) return;            // unreachable agent occupies nothing
    for (int t = 0; t <= H; ++t) {
        int node = path.at(t);
        reservation.reserve(node, t, t + 1);   // reserve() coalesces stays
    }
}
