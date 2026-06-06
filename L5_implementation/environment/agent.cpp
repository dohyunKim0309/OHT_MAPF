//
// Created by DohyunKim on 26. 6. 6..
//

#include "agent.h"

Agent::Agent() : id(-1), priority(0), current(-1), goal(-1) {}

Agent::Agent(int id, int priority, int current, int goal)
    : id(id), priority(priority), current(current), goal(goal) {}

int Agent::getId() const { return id; }
int Agent::getPriority() const { return priority; }
int Agent::getCurrent() const { return current; }
int Agent::getGoal() const { return goal; }

void Agent::setCurrent(int node) { current = node; }
void Agent::setGoal(int node) { goal = node; }

// Order by priority. Safe within this project: MinHeap only ever compares
// Agent* with Agent* (the same rule Interval uses for its avl_tree).
bool Agent::operator<(const Data& other) const {
    return priority < static_cast<const Agent&>(other).priority;
}
