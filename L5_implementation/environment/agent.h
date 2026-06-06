//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_AGENT_H
#define OHT_MAPF_AGENT_H

#include "../data_structure/data.h"

// Agent: one mobile unit in the simulation world — just a bundle of data
// (id, priority, current node, goal node), like Interval bundles [start,end).
// A concrete Data child so PP's MinHeap can order it by priority via operator<.
// Fields are private; access through getters, mutate the two changing fields
// (current/goal) through setters. Ownership lives with the Environment; PP only
// borrows Agent* into its priority queue.
// Contract: L3_interface/domain/environment/README.md (#Agent)
class Agent : public Data {
public:
    Agent();
    Agent(int id, int priority, int current, int goal);

    int getId() const;
    int getPriority() const;
    int getCurrent() const;
    int getGoal() const;

    void setCurrent(int node);   // Environment advances the agent along its path
    void setGoal(int node);      // Environment assigns a new goal on arrival

    // Order by priority: smaller priority is popped first by PP's MinHeap.
    bool operator<(const Data& other) const override;

private:
    int id;        // stable index into the Environment's agent array
    int priority;  // PP processing order (smaller = planned first)
    int current;   // current node (original index)
    int goal;      // target node (original index)
};

#endif //OHT_MAPF_AGENT_H
