//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_QUEUE_H
#define OHT_MAPF_QUEUE_H

#include "data.h"
#include "linked_list.h"

// Queue: FIFO 자료구조. LinkedList 를 뼈대로 써서 enqueue/dequeue 를
// 양끝 O(1)에 제공한다. Data* 는 비소유.
// 아는 것: FIFO 규율, enqueue=뒤(pushBack)·dequeue=앞(popFront) 매핑.
// 하지 않는 것: 노드를 직접 엮고 끊기(LinkedList가 함), 담은 Data* 의
//   의미 해석, Data* 의 수명 책임(비소유).
// 설계 근거: L3_interface/data_structure/queue.md
class Queue : private LinkedList{
public:
    // add (use linked list - pushBack)
    void enqueue(Data* elem);

    // delete and return (use linked list 0 popFront)
    Data* dequeue();

    // number of current
    int getSize() const;

    // is it empty?
    bool isEmpty() const;
};

#endif // OHT_MAPF_QUEUE_H
