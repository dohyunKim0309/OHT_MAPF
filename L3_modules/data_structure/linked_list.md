---
layer: L3
tags: [modules, ds]
---

# linked_list (queue의 뼈대)

## 책임
[[linked_list_node]]를 엮어 만든 단일 연결 리스트. head/tail 포인터를 들고,
양끝 삽입과 앞쪽 제거를 O(1)에 제공한다. `Data*`를 순서대로 보관한다.
저장한 `Data*`는 **비소유** — 삭제는 담당하지 않는다.

이 모듈의 경계는 **리스트 구조 / 도메인 의미** 축으로 판정한다.

**들어온다 (이 모듈):** 노드를 엮고 끊어 순서 있는 리스트를 유지하는 일.
- head/tail 관리, 양끝 삽입·앞쪽 제거, 노드 생성·삭제, 크기 추적.

**나간다 (사용처 모듈):** 저장된 `Data*`가 *무엇을 뜻하는지*, 어떤 순서
규율로 쓰는지(FIFO 등)의 해석. [[queue]]가 담당.

## 구현 방식

**public field**:
 - 없음 (head/tail/size는 캡슐화한다).

**private field**:
- `LinkedListNode* head` — 맨 앞 노드. 초기값 `nullptr`.
- `LinkedListNode* tail` — 맨 끝 노드. 초기값 `nullptr`.
- `int size` — 현재 노드 개수.

**methods** (잠정 — 사용처가 요구하는 연산이 확정되면 보강):
 - 생성자: 빈 리스트(head=tail=nullptr, size=0).
 - 소멸자: 모든 `LinkedListNode`를 해제. 단, 담은 `Data*`는 delete 하지 않음.
 - `void pushBack(Data* elem)` — tail 뒤에 추가. O(1).
 - `void pushFront(Data* elem)` — head 앞에 추가. O(1).
 - `Data* popFront()` — head 제거 후 그 `Data*` 반환. O(1).
 - `int getSize() const` — 현재 크기.

단일 연결이므로 tail에서의 제거(popBack)는 직전 노드를 알 수 없어 O(n)이다.
queue는 popBack을 쓰지 않으므로 제공하지 않는다.

**소유권**: 저장한 `Data*`는 **비소유**. 소멸자는 노드(`LinkedListNode`)만
해제하고 각 `Data*`는 delete 하지 않는다. `Data*`의 생사는 도메인 소유자가
쥔다. 전체 정책은 [[data_structure_design#소유권]].


## Time Complexity
 - 양끝 삽입(pushBack/pushFront): O(1)
 - 앞쪽 제거(popFront): O(1)
 - 끝쪽 제거(popBack): O(n) — 단일 연결이라 미제공
 - 임의 위치 접근: O(n)

## 의존성
### 1. 의존
- [[linked_list_node]] — 뼈대 노드.
- [[data]] — 노드가 담는 `Data*`.
### 2. 의존됨
- [[queue]] — FIFO 순서로 linked_list를 사용.

## 사용처
- [[queue]]의 내부 뼈대. enqueue는 pushBack, dequeue는 popFront로 매핑.

선정 근거는 [[data_structure_design#선정 근거]].
