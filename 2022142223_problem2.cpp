#include <iostream>
using namespace std;

class MinHeap {
private:
    int heap[100];
    int size;

public:
    MinHeap() {
        size = 0;
    }

    void push(int value) {
        // ################ problem 2-1 #####################
        // Contract: insert value into the heap, then reorder until heap property (parent < child) is satisfied

        // 1. insert at the end
        heap[size+1] = value;
        size++;
        // 2. compare the inserted value with its parent, swap if parent > child, iterate
        int index = size;
        while (index > 1) {
            if (heap[index/2] > heap[index]) {
                // swap two values
                swap(heap[index], heap[index/2]);
                index /= 2;
            }
            else return;
        }
        // ################ problem 2-1 #####################
    }

    int pop() {
        // ################ problem 2-2 #####################
        // Contract: remove and return the root value, then reorder until heap property (parent < child) is satisfied

        // 1. save root value(for return)
        int value = heap[1];
        // 2. move last to root, shrink size
        heap[1] = heap[size];
        size--;
        // 3. reorder until heap property is satisfied
        int index = 1;
        int left = index*2;
        int right = index*2+1;
        // loop while at least left child exists (complete binary tree: no left = no right)
        while (left <= size) {
            // pick smaller child (handle case where right child doesn't exist)
            int smaller;
            if (right > size) smaller = left;              // only left child exists
            else if (heap[left] < heap[right]) smaller = left;
            else smaller = right;

            // if heap property satisfied, stop
            if (heap[index] <= heap[smaller]) break;

            // swap with smaller child and move down
            swap(heap[index], heap[smaller]);
            index = smaller;
            left = index*2;
            right = index*2+1;
        }
        // 4. return root value
        return value;
        // ################ problem 2-2 #####################
    }

    void printHeap() {
        // ################ problem 2-3 #####################
        // Simple printing in index order
        for (int i = 1; i < size+1; i++) {
            cout << heap[i] << " ";
        }
        cout << endl;
        // ################ problem 2-3 #####################
    }
};

// ################ Do not touch here #####################
int main() {
    MinHeap pq;
    int values[5] = {20, 15, 30, 5, 10};

    cout << "[Problem 2-1] Push values into Min Heap" << endl;
    cout << "Push: ";
    for (int i = 0; i < 5; i++) {
        cout << values[i] << " ";
        pq.push(values[i]);
    }
    cout << endl << endl;

    cout << "[Problem 2-3] Heap after push" << endl;
    pq.printHeap();
    cout << endl;

    cout << "[Problem 2-2] Pop values from Min Heap" << endl;
    cout << "Pop: " << pq.pop() << endl;
    cout << "Pop: " << pq.pop() << endl;
    cout << endl;

    cout << "[Problem 2-3] Heap after pop" << endl;
    pq.printHeap();

    return 0;
}
// ################ Do not touch here #####################