---
layer: L3
tags: [modules, ds, building_block]
---

# LinkedListNode (linked_list의 빌딩 블록)

## 책임
[[linked_list]]의 뼈대 부품. `Data*` 한 칸과 다음 노드 링크 하나를 보관한다.
[[avl_tree_node]]와 구조가 달라(자식이 `next` 하나뿐, 높이 없음) 별도
빌딩 블록으로 둔다.

**아는 것:**
- 자기가 담은 `Data*` 하나.
- 자기 다음 노드(`next`)가 누구인지.

**하지 않는 것:**
- 리스트 전체(head/tail, 순서, 길이)를 모른다 — [[linked_list]]가 안다.
- 담은 `Data*`의 수명을 책임지지 않는다 (비소유).
- 다른 노드를 만들거나 지우지 않는다 — `next` 링크를 들고·바꿀 뿐이다.

## 구현 방식

**public field**:
 - 없음 (모든 필드는 캡슐화한다).

**private field**:
- `Data* data` — 담는 한 칸. 생성자로 받는다.
- `LinkedListNode* next` — 다음 노드 링크. 초기값 `nullptr`.

**methods**:
 - 생성자 `LinkedListNode(Data* data)` — `Data*` 보관, next=nullptr.
 - `Data* getData() const` — 담은 `Data*` 읽기.
 - `LinkedListNode* getNext() const`, `void setNext(LinkedListNode*)` — 링크 접근.

**소유권**: 담은 `Data*`는 **비소유**. 노드 소멸자는 그것을 delete 하지
않는다. `Data*`의 생사는 도메인 소유자가 쥔다. 전체 정책은
[[data_structure_design#소유권]].


## Time Complexity
 - 링크 접근(get/set Next): O(1)
 - `getData`: O(1)

## 의존성
### 1. 의존
- [[data]] — `Data*`를 한 칸에 담는다.
### 2. 의존됨
- [[linked_list]] — LinkedListNode를 뼈대로 쓴다.

## 사용처
- [[linked_list]]의 내부 노드. 바깥에는 노출되지 않는다.

선정 근거는 [[data_structure_design#선정 근거]].
