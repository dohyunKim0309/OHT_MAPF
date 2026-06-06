---
layer: L3
tags: [interface, ds, building_block]
---

# AvlTreeNode (avl_tree의 빌딩 블록)

## 책임
[[avl_tree]]의 뼈대 부품. `Data*` 한 칸과 좌/우 자식 링크, 그리고 높이를
보관한다. [[linked_list]]가 쓰는 [[linked_list_node]]와 구조가 달라
(자식이 좌/우 둘 + 높이) 별도 빌딩 블록으로 둔다.

**아는 것:**
- 자기가 담은 `Data*` 하나.
- 자기 좌/우 자식(`left`/`right`)이 누구인지.
- 자기 높이(`height`) 값 하나.

**하지 않는 것:**
- 트리 전체(균형, 정렬 순서, 어디에 삽입할지)를 모른다 — [[avl_tree]]가 안다.
- 높이를 *계산·갱신*하지 않는다. 값을 보관만 하며, 언제 어떤 값으로 바꿀지는
  avl_tree가 정한다(friend로 직접 쓴다). balance factor·회전도 avl_tree의 일.
- 담은 `Data*`의 수명을 책임지지 않는다 (비소유).
- 다른 노드를 만들거나 지우지 않는다 — 자식 링크를 들고·바꿀 뿐이다.

## 구현 방식

**public field**:
 - 없음 (모든 필드는 캡슐화한다).

**private field**:
- `Data* data` — 담는 한 칸. 생성자로 받는다.
- `AvlTreeNode* left`, `AvlTreeNode* right` — 자식 링크. 초기값 `nullptr`.
- `int height` — 노드 높이. 초기값 `1` (단일 노드 = leaf 의 높이는 1).

**methods**:
 - 생성자 `AvlTreeNode(Data* data)` — `Data*` 보관, left/right=nullptr, height=1.
 - `Data* getData() const` — 담은 `Data*` 읽기.
 - `AvlTreeNode* getLeft() const`, `void setLeft(AvlTreeNode*)` — 좌 자식 접근.
 - `AvlTreeNode* getRight() const`, `void setRight(AvlTreeNode*)` — 우 자식 접근.

**height·data 직접 접근**: public 접근자를 두지 않는다(`data`는 `getData()`로
읽기만, `height`는 아예 비공개). [[avl_tree]]를 `friend`로 선언해 avl_tree만
`height`와 `data`를 직접 읽고 쓴다. 높이는 균형 로직의 일부이고, `data` 교체는
삭제(2-children 케이스: successor의 `Data*`로 내용 대체)에 필요하다 — 둘 다
트리 로직의 일부라 그 주인을 avl_tree로 한정한다. 바깥에서는 `data`가 바뀌지
않는다(friend인 avl_tree만 교체).

**소유권**: 담은 `Data*`는 **비소유**. 노드 소멸자는 그것을 delete
하지 않는다. `Data*`의 생사는 도메인 소유자가 쥔다. 전체 정책은
[[data_structure_design#소유권]].


## Time Complexity
 - 자식 링크 접근(get/set Left/Right): O(1)
 - `getData`: O(1)

## 의존성
### 1. 의존
- [[data]] — `Data*`를 한 칸에 담는다.
### 2. 의존됨
- [[avl_tree]] — AvlTreeNode를 뼈대로 쓰고, friend로 height에 접근한다.

## 사용처
- [[avl_tree]]의 내부 노드. 바깥(planner/pp 등)에는 노출되지 않는다.

선정 근거(왜 노드 기반 트리인가, 대안 비교)는 [[data_structure_design#선정 근거]].
