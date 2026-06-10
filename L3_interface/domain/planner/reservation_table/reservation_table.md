---
layer: L3
tags: [interface, domain, planner, reservation_table]
---

# ReservationTable (시간 점유 표)

> **구성 축** (axis-role: bottom)
> used-by:: [[planner]], [[bfs_teg]]
> uses:: [[avl_tree]], [[dynamic_array]]
> owns:: [[interval]]

## 위치
[[planner]] 하위 도메인 구성요소. 공용 토대([[avl_tree]], [[dynamic_array]])를
빌려 쓰고, 자기 데이터([[#Interval]])와 자기 알고리즘(구간 병합 조율)을 가진다.

## 책임
각 그래프 정점이 **언제 사용 불가인지**를 시간 구간 `[start, end)`의 모임으로
보관한다. planner가 한 에이전트의 경로를 확정하면 그 점유를 여기 기록하고,
다음 에이전트는 이걸 피해 경로를 찾는다.

**아는 것:**
- 노드별로 사용 불가 구간 목록을 정렬·병합 상태로 유지하는 법.
- "닿으면 병합" 규칙 — 새 구간이 좌·우 이웃과 닿거나 겹치면 하나로 합친다
  (`[2,4)`+`[4,5)` → `[2,5)`, 셋 이상도 한 번에).
- 질의에 답하는 법 — point query("이 (node,t) 비었나"), interval query
  ("t 이상 첫 진입 가능 시각" = ϕ).

**하지 않는 것:**
- 트리를 직접 회전·균형 잡지 않는다 — [[avl_tree]]가 한다. ReservationTable은
  avl_tree의 연산(삽입·삭제·이웃 접근)을 조합해 병합을 *조율*만 한다.
- 누가 점유했는지(agent id)는 모른다 — 간단 버전은 "막혔다"는 사실만 둔다.
- 경로를 찾지 않는다 — 질의에 답할 뿐, 탐색은 planner의 일.

## 구현 방식

### 2층 구조
- **바깥층**: `dynamic_array`(또는 정점 수 길이의 배열). 인덱스 = 그래프 노드
  번호. `table[x]` = 노드 x의 reservation 트리.
- **안쪽층**: 각 칸의 [[avl_tree]]. 노드 x의 사용 불가 구간들을 Interval로
  담아 시작점 순으로 정렬 보관. 서로 겹치지 않는 분리 구간(병합으로 유지).

### Interval (이 모듈이 정의·소유하는 흐르는 데이터 타입)
`[start, end)` 정수 반열림 구간. 상세 정의는 [[interval]]. 요약: `Data` 상속
(`operator<`=시작점), `touches`/`merge`로 "닿으면 병합". 이 모듈이 `new`/`delete`로
소유하고 [[avl_tree]]에 담는다.

### 병합 조율 (reserve 연산)
새 구간 `[a,b)`를 노드 x에 기록할 때:
1. avl_tree에서 `[a,b)`의 좌측 이웃(predecessor)·우측 이웃(successor)을 찾는다.
2. 이웃이 `touches`면 흡수: 트리에서 삭제하고 `merge`로 합친다. 닿는 동안 반복
   (양방향, 여러 개 흡수 가능).
3. 합쳐진 하나를 트리에 삽입한다.

→ "거리 제약 충돌"이 가상노드로 (node,time) 점유 충돌이 됐듯, 점유 구간도
병합으로 항상 분리·정렬 상태를 유지한다.

### 질의
- **point query** `isFree(x, t)`: 노드 x의 트리에서 t를 포함하는 구간이 있는지.
  BFS+TEG가 TEG 빌드 시 점유 정점 제거 필터로 쓴다.
- **interval query** `phi(x, t)`: t 이상이면서 진입 가능한 가장 이른 시각.
  t가 어떤 구간에도 없으면 t, 구간 `[c,d)`에 들면 d. ϕ-BF가 쓴다.
- **window query** `phiWindow(x, t, len)`: `t` 이상이면서 노드 x가 **`len`스텝 연속**
  비는 가장 이른 시각 `s`(즉 `[s, s+len)` 전체가 빔). ϕ-BF의 **dwell 창**(`len=1+dwell`)
  확보에 쓴다 — 도착 한 시점만 보장하는 `phi`로는 머무는 구간을 못 지키기 때문.
  `phi`/successor 합성 또는 전용 구현. (`len=1`이면 `phi`와 동일.)

### 점유 기록 규약 (occupancy convention)
에이전트가 경로상 **각 노드에 머무는 전 구간**을 점유로 기록한다. PP의 recordPath가
매 시각 `path.at(t)`를 `[t,t+1)`로 reserve하고 reserve의 병합이 인접 구간을 합치므로,
*경로가 시각별 위치를 빈틈없이 채우기만 하면* 아래가 자동 성립한다:
- **목표 도착·작업 정지**: 도착 `t`면 `[t, t+1+dwell)` 점유(도착 1칸 + 작업 `dwell`칸).
  dwell=0이면 `[t, t+1)`.
- **이동 중 대기**: 다음 노드 진입이 점유 `[a,b)` 때문에 `b`로 밀리면, 진입 직전 노드를
  `[직전 도착시각, b)` 점유(거기서 기다림).

이 규약이 충돌 회피의 근거다 — 모든 머무름(이동 대기·작업 정지)이 reservation에 유한
구간으로 남아 다음(낮은 우선순위) 에이전트가 회피한다.

**소유권**: Interval(`Data*`)은 ReservationTable이 **소유**한다(트리에 넣을 때
new). 다만 정리는 avl_tree의 `clearAndDelete()`에 *위임*한다 — 트리가 내부를
순회하며 각 Interval을 delete해 준다. ReservationTable이 트리에서 포인터를
하나씩 빼 delete하는 번거로움을 피하기 위함이다.

이는 비소유 정책의 미묘한 확장이다: avl_tree의 **기본 소멸자는 여전히
비소유**(노드만 해제, Interval은 안 건드림)이고, `clearAndDelete()`는 호출자가
"이 트리의 Data*는 내가 소유했고 지금 지워도 된다"를 확인하고 부르는 **명시적
옵트인**이다. delete를 *결정*하는 주체(ReservationTable)와 *실행*하는 주체
(트리가 대행)를 분리한 것 — 소유권은 여전히 ReservationTable에 있다.
전역 정책은 [[data_structure_design#소유권]] 참조.

## Time Complexity
- reserve(병합 삽입): O(log k + (흡수 수)·log k), k=노드 x의 구간 수
- isFree / phi: O(log k)
- 노드 접근(바깥층): O(1)

## 의존성
### 1. 의존
- [[avl_tree]] — 노드별 구간 트리. 삽입·삭제·이웃 접근 제공.
- [[dynamic_array]] — 바깥층(노드별 트리 포인터 배열).
- [[data]] — Interval이 상속.
### 2. 의존됨
- [[planner]] — BFS+TEG(point query), ϕ-BF(interval query).

## 사용처
- **BFS+TEG**: 탐색(BFS) 중 isFree로 점유 (node,time) 정점 스킵.
- **ϕ-BF**: 탐색 중 phi로 t 이상 첫 가능 시각 회피.
- 같은 표를 두 알고리즘이 *다른 질의*로 쓴다 — 비교 핵심 축
  ([[data_structure_design#사용처]]).
- **라운드 단위 clear**: receding horizon이라 PP가 **매 라운드 시작에 clear()**
  로 전체를 비우고, 그 라운드의 H스텝 점유를 다시 쌓는다. clear는 각 노드 트리에
  `clearAndDelete()`를 돌려 Interval까지 정리(코드의 `ReservationTable::clear`).

선정 근거(왜 노드별 avl_tree인가)는 [[data_structure_design#선정 근거]].

## 미결
- 바깥층 구체 타입 — 코드는 `AvlTree[]` 배열로 구현(트리 객체 직접 보유).
- **avl_tree에 `clearAndDelete()` 추가 필요** — 현재 avl_tree엔 없음. 이
  연산을 avl_tree 계약·코드에 보강해야 함(reservation_table 사용처 요구).
