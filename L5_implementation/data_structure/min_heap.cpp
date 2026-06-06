#include "min_heap.h"

#include <cassert>

// Put a dummy at index 0 so real elements live at indices 1..size.
MinHeap::MinHeap() {
    DynamicArray::pushBack(nullptr);  // index 0 dummy
}

bool MinHeap::less(Data* a, Data* b) {
    return *a < *b;   // delegate to Data::operator
}

int MinHeap::getSize() const {
    // base array holds the dummy + the real elements.
    return DynamicArray::getSize() - 1;
}

bool MinHeap::isEmpty() const {
    return getSize() == 0;
}

// Insert at the end, then sift up.
void MinHeap::push(Data* elem) {
    DynamicArray::pushBack(elem);
    int index = DynamicArray::getSize() - 1;   // 1-based position of new elem

    while (index > 1) {
        int parent = index / 2;
        if (less(DynamicArray::get(index), DynamicArray::get(parent))) {
            Data* tmp = DynamicArray::get(index);
            DynamicArray::set(index, DynamicArray::get(parent));
            DynamicArray::set(parent, tmp);
            index = parent;
        } else {
            break;
        }
    }
}

// Remove and return the root (minimum), then sift down.
Data* MinHeap::pop() {
    assert(getSize() > 0 && "MinHeap::pop on empty heap");

    Data* minElem = DynamicArray::get(1);   // root = minimum
    Data* last = DynamicArray::popBack();    // detach last element

    if (getSize() == 0) {
        return minElem;   // only one real element existed
    }

    DynamicArray::set(1, last);              // move last to root
    int index = 1;
    int size = getSize();

    while (index * 2 <= size) {
        int left = index * 2;
        int right = index * 2 + 1;

        int smaller = left;
        if (right <= size && less(DynamicArray::get(right), DynamicArray::get(left))) {
            smaller = right;
        }

        if (!less(DynamicArray::get(smaller), DynamicArray::get(index))) {
            break;
        }

        Data* tmp = DynamicArray::get(index);
        DynamicArray::set(index, DynamicArray::get(smaller));
        DynamicArray::set(smaller, tmp);
        index = smaller;
    }

    return minElem;
}