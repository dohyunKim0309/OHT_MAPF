#include "node.h"

// ── AvlTreeNode ─────────────────────────────────────────────────────────

// Data* 를 보관하고, 자식은 없고(nullptr), 높이는 1(leaf)로 시작한다.
AvlTreeNode::AvlTreeNode(Data* data)
    : data(data), left(nullptr), right(nullptr), height(1) {}

Data* AvlTreeNode::getData() const {
    return data;
}

AvlTreeNode* AvlTreeNode::getLeft() const {
    return left;
}

void AvlTreeNode::setLeft(AvlTreeNode* node) {
    left = node;
}

AvlTreeNode* AvlTreeNode::getRight() const {
    return right;
}

void AvlTreeNode::setRight(AvlTreeNode* node) {
    right = node;
}

// ── LinkedListNode ──────────────────────────────────────────────────────

// Data* 를 보관하고, 다음 노드는 없다(nullptr).
LinkedListNode::LinkedListNode(Data* data)
    : data(data), next(nullptr) {}

Data* LinkedListNode::getData() const {
    return data;
}

LinkedListNode* LinkedListNode::getNext() const {
    return next;
}

void LinkedListNode::setNext(LinkedListNode* node) {
    next = node;
}
