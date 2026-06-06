#ifndef OHT_MAPF_MIN_HEAP_H
#define OHT_MAPF_MIN_HEAP_H

#include "data.h"
#include "dynamic_array.h"

// MinHeap: keeps Data* in priority order, pops the minimum in O(log n).
// Implemented in terms of DynamicArray (private inheritance): a complete
// binary tree laid out with 1-based indexing (index 0 is a dummy).
// Private inheritance hides the array's public ops (get/set/pushBack/...),
// so callers cannot break the heap invariant by poking the array directly.
// Comparison is delegated to Data::operator<. Non-owning.
// Design: L3_interface/data_structure/min_heap.md
class MinHeap : private DynamicArray {
public:
    // Empty heap. Puts a dummy at index 0 to keep 1-based indexing.
    MinHeap();

    // Insert, then sift up until parent <= child. O(log n).
    void push(Data* elem);

    // Return the minimum (root). Move the last element to the root,
    // shrink, then sift down. Empty heap is a contract violation (assert).
    Data* pop();

    // Number of elements (excludes the index-0 dummy).
    int getSize() const;

    // True if no elements.
    bool isEmpty() const;

private:
    // a < b by Data::operator< (the heap itself does not know the rule).
    static bool less(Data* a, Data* b);
};

#endif // OHT_MAPF_MIN_HEAP_H