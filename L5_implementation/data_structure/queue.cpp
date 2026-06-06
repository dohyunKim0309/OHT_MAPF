//
// Created by DohyunKim on 26. 6. 6..
//

#include "queue.h"

// FIFO - Queue. use LinkedList - pushBack
void Queue::enqueue(Data *elem) {
    LinkedList::pushBack(elem);
}

// FIFO - Dequeue
Data* Queue::dequeue() {
    return LinkedList::popFront();
}

int Queue::getSize() const {
    return LinkedList::getSize();
}

bool Queue::isEmpty() const {
    return LinkedList::getSize() == 0;
}
