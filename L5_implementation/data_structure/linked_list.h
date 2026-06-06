//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_LINKED_LIST_H
#define OHT_MAPF_LINKED_LIST_H

#include "data.h"
#include "node.h"   // LinkedListNode

// LinkedList: LinkedListNode 를 엮은 단일 연결 리스트. head/tail 을 들고
// 양끝 삽입과 앞쪽 제거를 O(1)에 제공한다. queue 의 뼈대.
// 아는 것: head/tail/size, 노드를 엮고 끊는 법.
// 하지 않는 것: 담은 Data* 의 의미·순서 규율 해석(queue가 함),
//   Data* 의 수명 책임(비소유).
// 설계 근거: L3_modules/data_structure/linked_list.md
class LinkedList {
public:
    // Constructor - empty list: head=tail=nullptr, size=0
    LinkedList();

    // Destructor
    ~LinkedList();

    // add in tail
    void pushBack(Data* elem);

    // add in front of tail
    void pushFront(Data* elem);

    // delete head, return Data* of that Data. O(1)
    // unable to call in empty list
    Data* popFront();

    // # of Nodes
    int getSize() const;

private:
    LinkedListNode* head; // head node pointer
    LinkedListNode* tail; // tail node pointer
    int size;
};



#endif //OHT_MAPF_LINKED_LIST_H
