#ifndef OHT_MAPF_AVL_TREE_H
#define OHT_MAPF_AVL_TREE_H

#include "data.h"
#include "node.h"   // AvlTreeNode

// AvlTree: a balanced BST built from AvlTreeNode. Keeps every node's left/right
// subtree height difference within 1, so insert/find stay O(log N).
// Height convention: leaf = 1, NULL = 0.
// Knows: root, node linking and balancing (rotations), height updates.
// Does not: interpret the stored Data* (reservation_table etc. does that), nor
//   own the Data* (non-owning). Key comparison is delegated to Data::operator<.
// Contract:  L3_interface/data_structure/avl_tree.md
// Mechanism: L4_mechanism/avl_tree.md
class AvlTree {
public:
    // empty tree: root = nullptr
    AvlTree();

    // Frees every node. The stored Data* is non-owning and is not deleted.
    ~AvlTree();

    // Insert at the operator<-ordered position, then walk back up updating
    // heights and rotating where needed to restore balance. O(log N).
    // A key already present is not inserted (duplicates are rejected).
    void insert(Data* elem);

    // Find the Data* equivalent to key by operator<. Returns nullptr if absent.
    // O(log N).
    Data* find(Data* key) const;

    // Empty the tree AND delete every stored Data*, then reset to empty.
    // The tree is normally non-owning (~AvlTree frees only nodes); this is an
    // explicit opt-in for a caller that DID own the stored Data* and wants the
    // tree to do the bulk cleanup work. The caller decides when to call it.
    void clearAndDelete();

private:
    AvlTreeNode* root;

    // ── balancing machinery (accesses node->height via friendship) ──
    static int height(AvlTreeNode* node);          // 0 if NULL
    static void updateHeight(AvlTreeNode* node);   // 1 + max(child heights)
    static int balanceFactor(AvlTreeNode* node);   // left height - right height

    static AvlTreeNode* rotateRight(AvlTreeNode* node);  // fixes LL, returns new root
    static AvlTreeNode* rotateLeft(AvlTreeNode* node);   // fixes RR, returns new root
    static AvlTreeNode* rebalance(AvlTreeNode* node);    // picks and applies rotation

    static AvlTreeNode* insert(AvlTreeNode* node, Data* elem);  // recursive insert
    static void destroy(AvlTreeNode* node);                     // post-order free (nodes only)
    static void destroyAndDelete(AvlTreeNode* node);            // post-order free + delete Data*
};

#endif //OHT_MAPF_AVL_TREE_H
