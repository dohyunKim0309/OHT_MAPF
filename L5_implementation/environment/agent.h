//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_AGENT_H
#define OHT_MAPF_AGENT_H

#include "../data_structure/data.h"

// Agent: one mobile unit in the simulation world — a bundle of data: id, priority,
// current node, and a GOAL QUEUE (the next goals to visit, in order). A concrete
// Data child so PP's MinHeap can order it by priority via operator<.
// The goal queue (not a single goal) exists so an agent that reaches a goal does
// NOT idle forever: it dwells a few work steps and heads to the next goal — which
// is what avoids the target conflict. The Environment owns the agent and keeps the
// queue filled from an infinite random stream; PP reads current + goals and passes
// them to findPath. Fields private; getters + mutators below.
// Contract: L3_interface/data_types/agent.md
class Agent : public Data {
public:
    static const int GOAL_CAP = 12;  // goal lookahead capacity (enough to cover H)

    Agent();
    Agent(int id, int priority, int current);

    int getId() const;
    int getPriority() const;
    int getCurrent() const;
    void setCurrent(int node);       // Environment advances the agent along its path

    // ── goal queue ──────────────────────────────────────────────────────────
    int  currentGoal() const;        // goals[0] = the goal being headed to (-1 if empty)
    int  goalCount() const;          // number of queued goals
    const int* goals() const;        // contiguous goals[0..goalCount) for findPath
    void pushGoal(int node);         // append a goal (Environment fills the stream)
    void popGoal();                  // consume goals[0] on arrival (shift the rest)

    // Order by priority: smaller priority is popped first by PP's MinHeap.
    bool operator<(const Data& other) const override;

private:
    int id;                  // stable index into the Environment's agent array
    int priority;            // PP processing order (smaller = planned first)
    int current;             // current node (original index)
    int goalBuf[GOAL_CAP];   // FIFO of upcoming goals, goalBuf[0] = current target
    int nGoals;              // number of valid entries in goalBuf
};

#endif //OHT_MAPF_AGENT_H
