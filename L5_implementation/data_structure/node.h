#ifndef OHT_MAPF_NODE_H
#define OHT_MAPF_NODE_H

#include "data.h"

// 이 파일은 두 빌딩 블록 노드를 함께 둔다 (개념은 분리, 파일은 하나):
//  - AvlTreeNode  : avl_tree의 뼈대 (좌/우 자식 + 높이)
//  - LinkedListNode : linked_list의 뼈대 (next 하나)
// 설계 근거: L3_interface/data_structure/avl_tree_node.md, linked_list_node.md

// ── AvlTreeNode ─────────────────────────────────────────────────────────
// 아는 것: 담은 Data* 하나, 좌/우 자식, 자기 높이.
// 하지 않는 것: 트리 전체(균형·정렬)를 모른다. 높이를 계산·갱신하지 않고
//   보관만 한다(갱신은 AvlTree가 friend로). Data* 수명을 책임지지 않는다(비소유).
class AvlTree;  // 전방 선언: height 접근을 friend 로 허용하기 위함

class AvlTreeNode {
public:
    // 담을 Data* 를 받아 보관한다. left/right = nullptr, height = 1.
    // 이 Data* 는 비소유 — 소멸자는 그것을 delete 하지 않는다.
    explicit AvlTreeNode(Data* data);

    // 담은 Data* 읽기. 한 번 받으면 바뀌지 않는다.
    Data* getData() const;

    // 자식 링크 접근. 트리 구조 연결은 AvlTree 가 이 함수들로 수행한다.
    AvlTreeNode* getLeft() const;
    void setLeft(AvlTreeNode* node);
    AvlTreeNode* getRight() const;
    void setRight(AvlTreeNode* node);

private:
    Data* data;          // 담는 한 칸 (비소유)
    AvlTreeNode* left;   // 좌 자식
    AvlTreeNode* right;  // 우 자식
    int height;          // 노드 높이. 균형 로직의 일부이므로 AvlTree 만 접근한다.

    // 높이는 public 접근자를 두지 않는다. 균형 로직의 주인인 AvlTree 만
    // height 를 직접 읽고 쓴다.
    friend class AvlTree;
};

// ── LinkedListNode ──────────────────────────────────────────────────────
// 아는 것: 담은 Data* 하나, 자기 다음 노드(next).
// 하지 않는 것: 리스트 전체(head/tail·순서·길이)를 모른다. 다른 노드를
//   만들거나 지우지 않는다(링크만 들고·바꿈). Data* 수명을 책임지지 않는다(비소유).
class LinkedListNode {
public:
    // 담을 Data* 를 받아 보관한다. next = nullptr.
    // 이 Data* 는 비소유 — 소멸자는 그것을 delete 하지 않는다.
    explicit LinkedListNode(Data* data);

    // 담은 Data* 읽기. 한 번 받으면 바뀌지 않는다.
    Data* getData() const;

    // 다음 노드 링크 접근. 리스트 연결은 LinkedList 가 이 함수들로 수행한다.
    LinkedListNode* getNext() const;
    void setNext(LinkedListNode* node);

private:
    Data* data;             // 담는 한 칸 (비소유)
    LinkedListNode* next;   // 다음 노드 링크
};

#endif // OHT_MAPF_NODE_H
