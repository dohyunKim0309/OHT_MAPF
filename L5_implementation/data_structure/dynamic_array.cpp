//
// Created by DohyunKim on 26. 6. 6..
//

#include "dynamic_array.h"

#include <cassert>

DynamicArray::DynamicArray(int initialCapacity)
    : buffer(new Data*[initialCapacity]),
      size(0),
      capacity(initialCapacity) {}


// delete only internal buffer.
DynamicArray::~DynamicArray() {
    delete[] buffer;
}

// get data on given index.
Data* DynamicArray::get(int index) const {
    assert(index >= 0 && index < size && "DynamicArray::get index out of range");
    return buffer[index];
}

// set data by given Data* pointer on given index.
void DynamicArray::set(int index, Data* elem) {
    assert(index >= 0 && index < size && "DynamicArray::set index out of range");
    buffer[index] = elem;
}

// add data on back of the array
void DynamicArray::pushBack(Data *elem) {
    // if exceedes the capacity, then size up the array by 2x(sizeUp();)
    if (size == capacity) {
        sizeUp();
    }
    buffer[size] = elem;
    ++size;
}

// size up by 2x
void DynamicArray::sizeUp() {
    int newCapacity = capacity * 2;
    Data** newBuffer = new Data*[newCapacity];
    for (int i = 0; i < size; ++i) {
        newBuffer[i] = buffer[i];
    }
    delete[] buffer;           // delete old buffer
    buffer = newBuffer;
    capacity = newCapacity;
}

// get and return size of Dynamic Array
int DynamicArray::getSize() const {
    return size;
}