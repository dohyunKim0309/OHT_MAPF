---
layer: L3
tags: [modules, ds]
---

# queue (FIFO)

## 책임
FIFO(먼저 넣은 것 먼저 꺼냄) 자료구조. [[linked_list]]를 **private 상속**해
구현한다(is-implemented-in-terms-of). enqueue/dequeue를 양끝 O(1)에 제공하고,
`Data*`를 보관하며 **비소유**다.

**아는 것:**
- 먼저 들어온 것을 먼저 내보낸다는 FIFO 규율.
- enqueue는 linked_list의 뒤(pushBack), dequeue는 앞(popFront)에 매핑된다.

**하지 않는 것:**
- 노드를 직접 엮거나 끊지 않는다 — 그건 [[linked_list]]가 한다. queue는
  linked_list 연산을 FIFO 의미로 호출만 한다.
- 담은 `Data*`가 *무엇인지*(Graph Node인지 등) 모른다 — 사용처가 해석한다.
- 담은 `Data*`의 수명을 책임지지 않는다 (비소유).

## 구현 방식

**상속**: `class Queue : private LinkedList`. private 상속이므로
linked_list의 public 연산(`pushFront` 등 FIFO를 깨는 연산 포함)은 바깥에
노출되지 않는다 — queue는 enqueue/dequeue만 FIFO 의미로 다시 공개한다.
합성(멤버로 보유)과 의도는 같고("쓴다"이지 "이다"가 아님), 베이스 연산
위임을 간결히 하기 위해 private 상속을 택한다.

**public field / private field**:
 - 없음. 상태(head/tail/size)는 베이스 LinkedList가 들고 있다.

**methods**:
 - `void enqueue(Data* elem)` — 뒤에 추가. `LinkedList::pushBack`. O(1).
 - `Data* dequeue()` — 앞에서 제거 후 반환. `LinkedList::popFront`. O(1).
   빈 큐에서 호출은 계약 위반(linked_list의 assert로 잡힘).
 - `int getSize() const` — `LinkedList::getSize`.
 - `bool isEmpty() const` — getSize() == 0.

생성자·소멸자는 두지 않는다 — 베이스 LinkedList의 것이 자동으로 빈 큐를
만들고, 소멸 시 노드를 정리한다(비소유 보장도 베이스가 유지).

dequeue 외에 "제거 없이 맨 앞 보기"(peek/front)는 현재 두지 않는다.
BFS+TEG·ϕ-BF가 그것을 요구하면 planner 가지에서 보강한다(미결).

**소유권**: 담은 `Data*`는 **비소유**. queue·linked_list 모두 노드만
관리하고 `Data*`는 delete 하지 않는다. dequeue는 그 `Data*`를 반환해
호출자에게 넘긴다. 전체 정책은 [[data_structure_design#소유권]].


## Time Complexity
 - enqueue: O(1)
 - dequeue: O(1)
 - getSize / isEmpty: O(1)

## 의존성
### 1. 의존
- [[linked_list]] — 뼈대. enqueue→pushBack, dequeue→popFront.
### 2. 의존됨
- planner — BFS+TEG의 탐색 큐, ϕ-BF의 라운드 전파 큐. 상세는
  [[data_structure_design#사용처]].

## 사용처
- **BFS+TEG**: TEG 위 BFS의 탐색 큐. 방문할 (node,time)을 레벨 순서로 꺼낸다.
- **ϕ-BF**: 라운드 전파 큐. 값이 갱신된 노드를 넣고 먼저 들어온 것부터
  이웃을 재검사한다(SPFA식).

선정 근거는 [[data_structure_design#선정 근거]].
