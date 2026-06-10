//
// Created by DohyunKim on 26. 6. 6..
//

#include "agent.h"

#include <cassert>

Agent::Agent() : id(-1), priority(0), current(-1), nGoals(0) {}

Agent::Agent(int id, int priority, int current)
    : id(id), priority(priority), current(current), nGoals(0) {}

int Agent::getId() const { return id; }
int Agent::getPriority() const { return priority; }
int Agent::getCurrent() const { return current; }
void Agent::setCurrent(int node) { current = node; }

int Agent::currentGoal() const { return nGoals > 0 ? goalBuf[0] : -1; }
int Agent::goalCount() const { return nGoals; }
const int* Agent::goals() const { return goalBuf; }

void Agent::pushGoal(int node) {
    assert(nGoals < GOAL_CAP && "Agent::pushGoal: goal queue full");
    goalBuf[nGoals++] = node;
}

void Agent::popGoal() {
    assert(nGoals > 0 && "Agent::popGoal: empty goal queue");
    for (int i = 1; i < nGoals; ++i) goalBuf[i - 1] = goalBuf[i];  // shift left
    --nGoals;
}

// Order by priority. Safe within this project: MinHeap only ever compares
// Agent* with Agent* (the same rule Interval uses for its avl_tree).
bool Agent::operator<(const Data& other) const {
    return priority < static_cast<const Agent&>(other).priority;
}
