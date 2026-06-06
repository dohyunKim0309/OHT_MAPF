#include "avl_tree.h"

#include <cassert>

// Contract:  L3_interface/data_structure/avl_tree.md
// Mechanism: L4_mechanism/avl_tree.md
// Height convention: leaf = 1, NULL = 0.

AvlTree::AvlTree() : root(nullptr) {}

// Free every node post-order. The stored Data* is non-owning.
AvlTree::~AvlTree() {
    destroy(root);
}

// ── public ──────────────────────────────────────────────────────────────

void AvlTree::insert(Data* elem) {
    root = insert(root, elem);
}

Data* AvlTree::find(Data* key) const {
    AvlTreeNode* cur = root;
    while (cur != nullptr) {
        Data* d = cur->getData();
        if (*key < *d) {
            cur = cur->getLeft();
        } else if (*d < *key) {
            cur = cur->getRight();
        } else {
            return d;   // equivalent
        }
    }
    return nullptr;
}

// Explicit opt-in: delete every stored Data* too, then reset to empty.
// Normally the tree is non-owning; the caller calls this only when it owned
// the stored Data* and wants the tree to do the bulk cleanup.
void AvlTree::clearAndDelete() {
    destroyAndDelete(root);
    root = nullptr;   // reusable after clearing
}

// ── private: height helpers (access node->height via friendship) ──────────

int AvlTree::height(AvlTreeNode* node) {
    return node == nullptr ? 0 : node->height;
}

void AvlTree::updateHeight(AvlTreeNode* node) {
    int hl = height(node->getLeft());
    int hr = height(node->getRight());
    node->height = 1 + (hl > hr ? hl : hr);
}

int AvlTree::balanceFactor(AvlTreeNode* node) {
    return height(node->getLeft()) - height(node->getRight());
}

// ── private: single rotations (return new subtree root, preserve in-order) ─

AvlTreeNode* AvlTree::rotateRight(AvlTreeNode* node) {
    assert(node != nullptr && node->getLeft() != nullptr && "rotateRight precondition");
    AvlTreeNode* pivot = node->getLeft();
    node->setLeft(pivot->getRight());
    pivot->setRight(node);
    updateHeight(node);   // the demoted old root first
    updateHeight(pivot);
    return pivot;
}

AvlTreeNode* AvlTree::rotateLeft(AvlTreeNode* node) {
    assert(node != nullptr && node->getRight() != nullptr && "rotateLeft precondition");
    AvlTreeNode* pivot = node->getRight();
    node->setRight(pivot->getLeft());
    pivot->setLeft(node);
    updateHeight(node);
    updateHeight(pivot);
    return pivot;
}

// ── private: restore balance ───────────────────────────────────────────────

AvlTreeNode* AvlTree::rebalance(AvlTreeNode* node) {
    updateHeight(node);
    int bf = balanceFactor(node);

    if (bf > 1) {                                   // left heavy
        if (balanceFactor(node->getLeft()) < 0) {   // LR: make it LL first
            node->setLeft(rotateLeft(node->getLeft()));
        }
        return rotateRight(node);                   // resolve LL
    }
    if (bf < -1) {                                  // right heavy (symmetric)
        if (balanceFactor(node->getRight()) > 0) {  // RL
            node->setRight(rotateRight(node->getRight()));
        }
        return rotateLeft(node);
    }
    return node;                                    // already balanced
}

// ── private: recursive insert (returns the (rotated) subtree root) ─────────

AvlTreeNode* AvlTree::insert(AvlTreeNode* node, Data* elem) {
    if (node == nullptr) {
        return new AvlTreeNode(elem);   // leaf, height = 1
    }
    Data* d = node->getData();
    if (*elem < *d) {
        node->setLeft(insert(node->getLeft(), elem));
    } else if (*d < *elem) {
        node->setRight(insert(node->getRight(), elem));
    } else {
        return node;   // duplicate key: not inserted, subtree unchanged
    }
    return rebalance(node);
}

// ── private: destroy (post-order, frees nodes only) ────────────────────────

void AvlTree::destroy(AvlTreeNode* node) {
    if (node == nullptr) return;
    destroy(node->getLeft());
    destroy(node->getRight());
    delete node;   // node only; the stored Data* is non-owning.
}

// Like destroy, but also deletes each stored Data* (caller-owned, opt-in).
void AvlTree::destroyAndDelete(AvlTreeNode* node) {
    if (node == nullptr) return;
    destroyAndDelete(node->getLeft());
    destroyAndDelete(node->getRight());
    delete node->getData();   // the owned payload
    delete node;              // then the node
}
