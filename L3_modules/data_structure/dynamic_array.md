---
layer: L3
tags: [modules, ds, building_block]
---
# dynamic_array (자체 자료구조 및 min_heap, graph, queue의 빌딩 블록)

## 책임
`Data*`를 연속 메모리에 동적 배열로 저장한다. 인덱스 기반 임의 접근과
끝에서의 추가를 효율적으로 제공하고, 용량이 차면 스스로 확장한다.

**아는 것:**
- 자기가 담은 `Data*`들과 그 개수(`size`)·용량(`capacity`).
- 인덱스로 칸에 접근하는 법, 끝에 추가하는 법, 가득 차면 확장하는 법.

**하지 않는 것:**
- 각 칸의 `Data*`가 *무엇을 뜻하는지* 모른다 — 우선순위 원소인지, 인접
  정점인지, dist 값인지는 사용처(min_heap·graph 등)가 해석한다.
- 정렬·우선순위·탐색 같은 상위 로직을 하지 않는다.
- 저장한 `Data*`의 수명을 책임지지 않는다 (비소유) — 삭제하지 않는다.

## 구현 방식

**public field**:
 - 없음 (내부 버퍼·크기·용량은 캡슐화한다).

**private field**:
- `Data** buffer` — `Data*`들을 담는 연속 버퍼.
- `int size` — 현재 저장된 개수.
- `int capacity` — 버퍼가 담을 수 있는 최대 개수.

**methods** (잠정 — 사용처가 요구하는 연산이 확정되면 보강):
 - 생성자: 초기 용량으로 버퍼 할당.
 - 소멸자: `buffer`만 해제. 저장된 `Data*`는 delete 하지 않는다(비소유).
 - `Data* get(int index) const` — 인덱스 접근.
 - `void set(int index, Data* elem)` — 인덱스 위치 덮어쓰기.
 - `void pushBack(Data* elem)` — 끝에 추가. 가득 차면 용량 확장.
 - `int getSize() const` — 현재 크기.

**용량 확장**: `size == capacity`에서 추가 시 버퍼를 더 큰 것(보통 2배)으로
재할당하고 기존 `Data*`들을 옮긴다. 분할상환 O(1)을 위함.

**소유권**: 저장한 `Data*`는 **비소유**. 소멸자는 내부 `buffer`만 해제하고
각 `Data*`는 delete 하지 않는다. `Data*`의 생사는 도메인 소유자가 쥔다.
전체 정책은 [[data_structure_design#소유권]].


## Time Complexity
 - 인덱스 기반 접근(get/set): O(1)
 - 끝에 추가/삭제(pushBack/popBack): 분할상환 O(1) (확장 시 O(n))
 - 중간 삽입/삭제: O(n) (뒤 원소들을 밀어야 함)

## 의존성
### 1. 의존
- [[data]] — 각 칸에 `Data*`를 담는다.
### 2. 의존됨
- [[min_heap]] — 힙 배열의 저장소.
- [[graph]] — 인접 리스트의 저장소.
- [[queue]] — FIFO 버퍼의 저장소.

## 사용처
- [[min_heap]]·[[graph]]·[[queue]]의 내부 뼈대.
- planner의 노드별 dist/ϕ값·predecessor 배열로 직접 사용.

선정 근거는 [[data_structure_design#선정 근거]].
