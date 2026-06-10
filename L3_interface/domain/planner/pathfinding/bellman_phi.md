---
layer: L3
tags: [interface, domain, planner, algorithm]
---

# BellmanPhi (ϕ Bellman-Ford)

> **구성 축** (axis-role: bottom)
> inherits:: [[pathfinding|PathFinder]]
> uses:: [[graph]], [[queue]], [[reservation_table]], [[dynamic_array]]
> produces:: [[path]]

## 위치
[[planner]]의 단일 에이전트 경로 탐색 구현 중 하나(`PathFinder` 상속, 전략 패턴).
[[bfs_teg]]와 **평행 배치**되어 교체 가능하다 — PP는 어느 쪽인지 모른 채 호출한다.

## 책임
시간 제약 하의 단일 에이전트 최단(=최早 도달) 경로를, **그래프를 시간으로 복제하지
않고** 푼다. 대신 노드별 "가장 이른 도달 시각"을 Bellman-Ford식 완화로 구하되, 한
간선을 건널 때 [[reservation_table]]의 **interval query `phi`**로 점유를 회피한다.

[[bfs_teg]]와 **같은 입력**(unweighted 확장본 [[graph]], 같은 [[reservation_table]])을
쓴다. 차이는 *시간을 다루는 방식*뿐이다:
- BFS+TEG: 그래프를 시각마다 복제(TEG, 공간 V·(H+1))하고 그 위에서 BFS. 점유는
  `isFree` **point query**로 탐색 시 흡수.
- ϕ-BF: 그래프를 복제하지 않고(원본 V·E만), 점유는 `phi` **interval query**로
  런타임 회피. 이것이 두 알고리즘의 자료구조 활용 비교 핵심축이다
  ([[data_structure_design#사용처]]).

**아는 것:**
- 노드별 최早 도달 시각 `earliest[]`를 완화로 확정하는 법.
- 간선 완화 시 도착 시각 = `phi(v, earliest[u] + 1)` — 1스텝 이동 후 v의 점유를
  건너뛴 첫 진입 가능 시각.
- 갱신된 노드를 [[queue]]에 넣어 다시 완화하는 SPFA식 전파.
- **목표 시퀀스 이어붙이기**: start→goals[0] 완화·복원 → 도달 후 작업 정지 → 그 시각을
  새 출발로 goals[1] 완화 → … H를 채울 때까지. 각 구간은 독립 완화, reservation 시각 누적.
- **dwell 창 확보**: 목표 도착 시 `[t, t+1+dwell)` 점유(도착 1칸 + 작업 `dwell`칸,
  기본 dwell=1). `phi`는 도착 한 시점만 보장하므로, 그 창이 전부 비도록 `phiWindow`로
  도착을 늦춘다([[reservation_table]] window query). **안 하면 작업 정지 중 충돌**.
- **대기 점유**: 이동 중 v 진입이 점유로 밀리면 진입 직전 노드에서 기다림 —
  점유 기록 규약은 [[reservation_table]] 참조(시각별 위치를 채우면 PP가 자동 기록).
- predecessor + 도착시각으로 각 구간의 시각별 경로(대기·작업정지 포함)를 복원하는 법.

**하지 않는 것:**
- 그래프를 시간 복제하지 않는다(그게 BFS+TEG와의 대비점).
- reservation을 *기록*하지 않는다 — `phi`/`phiWindow`로 읽기만. 기록은 PP.
- 우선순위 큐를 쓰지 않는다 — 단위 간선이라 FIFO 완화로 충분(SPFA).

## 구현 방식

### 상태 (노드별, 시간 복제 없음)
- `earliest[v]` — 시작에서 v에 도달 가능한 가장 이른 시각(초기 ∞).
  크기 = `graph.size()`([[dynamic_array]]). **V·(H+1)이 아니라 V** — 복제 없음.
- `pred[v]` — 완화 경로의 직전 노드(복원용).
- `inQueue[v]` — 큐 중복 방지.

### 완화 (한 간선)
노드 `u`가 시각 `earliest[u]`에 도달돼 있을 때 간선 `u→v`:
```
arrive = phi(v, earliest[u] + 1)     // 1스텝 이동 + v 점유 회피
if arrive <= H and arrive < earliest[v]:
    earliest[v] = arrive; pred[v] = u; enqueue v
```
`phi(v, s)`는 s 이상이면서 v가 비는 첫 시각(점유 구간 `[c,d)`에 들면 d로 점프).

### 전파 (SPFA)
시작 `earliest[start] = phi(start, 0)`에서 시작해 큐에 넣고, 꺼낸 노드의 모든
이웃을 완화하며 갱신된 노드를 다시 큐에 넣는다. 큐가 비면 종료.

### 경로 복원 — 대기 채움
ϕ-BF는 노드별 도착 시각만 갖는다. `pred`로 시작↔목표 노드 시퀀스를 얻고, 각 노드의
`earliest`로 시각을 안다. **연속한 두 노드의 도착 시각 차가 1보다 크면**(phi 점프로
대기가 생긴 경우) 그 사이 시각은 **앞 노드에 머무름**으로 채운다. 도달 후 남은 시각은
목표로 채운다(정지). 결과는 [[path]](길이 H+1) — BFS+TEG와 같은 형식이라 PP·
environment가 동일하게 쓴다.

## Time/Space Complexity
- 공간: `earliest`/`pred`/`inQueue` 각 O(V) (V=확장본 노드). **시간 복제 없음** —
  이것이 TEG의 O(V·(H+1)) 대비 핵심 이점.
- 시간: SPFA 완화 — 최악 O(V·E)이나 실제론 갱신 횟수에 비례. 각 완화는 `phi`
  호출 O(log k)(노드의 점유 구간 수).

## 의존성
### 1. 의존
- [[graph]](확장본) / [[reservation_table]](`phi` 읽기) / [[queue]] / [[dynamic_array]].
- `PathFinder`(추상) — 상속.
### 2. 의존됨
- [[planner]](PP) — `PathFinder*`로 주입받아 호출.

## 사용처
- PP가 BellmanPhi를 주입받은 경우의 단일 에이전트 경로 탐색.
- [[bfs_teg]]와의 비교 실험(계획 시간·메모리). 선정 근거·비교 축은
  [[data_structure_design#선정 근거]].

## 미결
- 음수 가중치 없음(시간은 단조 증가)이라 SPFA 종료 보장. 사이클로 인한 무한 완화
  없음 — `earliest`가 단조 감소만 하고 정수·하한 있음.
- empty(도달 실패) 시 비용: 큐가 도달 가능 노드를 다 훑음 — BFS+TEG와 같은 성격.
