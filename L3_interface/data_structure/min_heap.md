---
layer: L3
tags: [interface, ds]
---

# min_heap (최소 힙 / 우선순위 큐)

## 책임
`Data*`를 우선순위 순으로 보관하고, 항상 **최소 원소를 O(log n)에 꺼내는**
자료구조. 완전 이진 트리를 [[dynamic_array]] 위에 배열로 표현한다. 비교는
`Data`의 `operator<`에 위임하므로, "무엇이 더 작은가"는 담기는 구체 Data가
정한다.

**아는 것:**
- 부모 ≤ 자식이라는 힙 불변식과, 그것을 sift-up/sift-down으로 복구하는 법.
- 배열 인덱스로 부모·자식을 찾는 법(1-기반: 부모 i → 자식 2i, 2i+1, 부모 i/2).
- 루트(인덱스 1)가 항상 최소라는 것.

**하지 않는 것:**
- 두 `Data*` 중 무엇이 작은지 *스스로 판단하지 않는다* — `operator<`에 위임한다.
- 담은 `Data*`가 *무엇인지*(Agent인지 등) 모른다.
- 담은 `Data*`의 수명을 책임지지 않는다 (비소유).

## 구현 방식

**뼈대**: [[dynamic_array]]를 **private 상속**해 구현한다
(is-implemented-in-terms-of). 힙을 연속 배열로 표현하며 **1-기반 인덱싱**을
쓴다(인덱스 0은 dummy로 비워둔다) — 부모/자식 식이 `2i`, `2i+1`, `i/2`로
깔끔하고, 과제 코드(`2022142223_problem2.cpp`)·강의자료와 일치한다.
private 상속이므로 dynamic_array의 public 연산(get/set/pushBack/popBack)은
바깥에 노출되지 않는다 — 호출자가 배열을 직접 건드려 힙 불변식을 깨지
못한다. push/pop만 공개한다.

**public field / private field**:
 - 없음. 배열 상태(buffer/size/capacity)는 베이스 dynamic_array가 들고 있다.

**methods**:
 - 생성자: 베이스에 dummy(nullptr) 하나를 pushBack해 1-기반을 맞춘다.
 - `void push(Data* elem)` — 끝에 추가 후 부모와 비교하며 위로(sift-up).
   `operator<`로 자신이 부모보다 작으면 swap, 루트까지 반복. O(log n).
 - `Data* pop()` — 루트(최소)를 반환. 마지막 원소를 루트로 옮기고
   (베이스 popBack) 크기를 줄인 뒤, 더 작은 자식과 비교하며 아래로(sift-down).
   빈 힙 호출은 계약 위반(assert). O(log n).
 - `int getSize() const` — 원소 개수(dummy 제외 = 베이스 getSize − 1).
 - `bool isEmpty() const` — 원소가 없으면 true.
 - (private) `less(a, b)` — `*a < *b`를 한 곳에 격리.

**비교**: 두 원소 `a`, `b`의 우선순위는 `*a < *b`(`Data::operator<`)로
판정한다. min_heap은 비교 로직을 모른 채 이 결과만 쓴다.

**소유권**: 담은 `Data*`는 **비소유**. push/pop·소멸자 모두 `Data*`를
delete 하지 않는다. pop은 그 `Data*`를 반환해 호출자에게 넘긴다. 베이스
dynamic_array도 비소유라 buffer만 정리한다. 전체 정책은
[[data_structure_design#소유권]].


## Time Complexity
 - push: O(log n) (sift-up)
 - pop: O(log n) (sift-down)
 - getSize / isEmpty: O(1)

## 의존성
### 1. 의존
- [[dynamic_array]] — 힙 배열의 저장소(1-기반).
- [[data]] — 담는 `Data*`와 비교 연산 `operator<`.
### 2. 의존됨
- planner(PP) — 에이전트 우선순위 큐. 상세는 [[data_structure_design#사용처]].

## 사용처
- PP가 Agent를 우선순위 순으로 하나씩 꺼내는 priority queue.
  (BFS+TEG의 탐색은 unweighted라 우선순위가 불필요해 min_heap이 아니라
  queue를 쓴다 — min_heap은 PP 레벨에서만.)

선정 근거는 [[data_structure_design#선정 근거]].
