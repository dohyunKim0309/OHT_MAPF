#include "linked_list.h"

#include <cassert>

LinkedList::LinkedList()
    : head(nullptr), tail(nullptr), size(0) {}

// 노드만 해제한다. 각 노드가 담은 Data* 는 도메인 소유자가 정리한다(비소유).
LinkedList::~LinkedList() {
    LinkedListNode* cur = head;
    while (cur != nullptr) {
        LinkedListNode* next = cur->getNext();
        delete cur;
        cur = next;
    }
}

void LinkedList::pushBack(Data* elem) {
    LinkedListNode* node = new LinkedListNode(elem);
    if (tail == nullptr) {
        head = node;
        tail = node;
    } else {
        tail->setNext(node);
        tail = node;
    }
    ++size;
}

void LinkedList::pushFront(Data* elem) {
    LinkedListNode* node = new LinkedListNode(elem);
    if (head == nullptr) {
        head = node;
        tail = node;
    } else {
        node->setNext(head);
        head = node;
    }
    ++size;
}

Data* LinkedList::popFront() {
    assert(head != nullptr && "LinkedList::popFront on empty list");
    LinkedListNode* node = head;
    Data* elem = head->getData();

    head = node->getNext();
    // if there was only one node on the linked list, then set tail to nullptr, like head
    if (head == nullptr) {
        tail = nullptr;
    }
    delete node;      // only delete node. elem(Data*) should be returned(memory is allocated, freed there)
    --size;
    return elem;
}

// return size of LinkedList
int LinkedList::getSize() const {
    return size;
}