---
layer: L3
tags: [interface, domain, planner, algorithm]
---

# BfsTeg (BFS + Time-Expanded Graph)

## 위치
[[planner]]의 단일 에이전트 경로 탐색 구현 중 하나. `Planner` 추상 클래스를
상속한다(전략 패턴). 베이스라인 알고리즘.

## 책임
시간 제약 하의 단일 에이전트 최단경로를, **그래프를 시간으로 확장(TEG)한 뒤
그 위에서 BFS**로 푼다. reservation은 TEG에서 점유된 `(node, time)`을 제외하는
방식으로 흡수하므로, BFS 자체는 reservation을 모른 채 순수 최단경로를 찾는다.

**아는 것:**
- `(node, time)`을 정점으로 보는 시간 확장, 그 위의 BFS(레벨 = 시각).
- 전이 두 가지: 이동 `(u,t)→(v,t+1)`(graph 간선 u→v), 대기 `(u,t)→(u,t+1)`.
- 점유된 `(node,time)`을 [[reservation_table]]의 `isFree`로 걸러 제외하는 법.
- predecessor 기록 → 목표에서 역추적해 경로 복원.

**하지 않는 것:**
- reservation을 *기록*하지 않는다 — 읽기(isFree)만. 기록은 PP.
- 우선순위 비교를 하지 않는다 — unweighted라 BFS(FIFO)면 최단. min_heap 불필요.

## 구현 방식

### TEG 정점과 전이
정점 = `(node, time)`, `0 ≤ time ≤ H`. 시작 `(start, 0)`. 각 스텝:
- **이동**: graph의 각 이웃 v에 대해 `(v, t+1)`.
- **대기**: `(u, t+1)` (제자리).
모든 간선 길이 1이라 시간 1스텝 = 간선 1개(unweighted 확장본을 쓰는 이유).

### reservation 흡수
`(v, t+1)`로 가기 전 `reservation.isFree(v, t+1)`를 확인. 점유면 그 정점으로의
전이를 막는다(TEG에서 제거된 것과 동치). 대기 `(u,t+1)`도 같은 검사.

### 탐색 (BFS)
- **빌려 쓰는 자료구조**: [[queue]](방문할 `(node,time)`을 FIFO로), [[graph]]
  (이웃 순회), [[dynamic_array]](visited / predecessor 저장).
- visited: `(node,time)` 단위. 같은 정점 재방문 방지.
- BFS라 처음 도달한 `(goal, t)`가 최단(가장 이른 도달). 거기서 종료.
- horizon H까지 못 찾으면 실패(빈 경로).

### 경로 복원
predecessor를 `(node,time)`별로 기록(dynamic_array). 목표 정점에서 시작까지
predecessor 체인을 역추적, 뒤집어 시각순 노드 시퀀스를 만든다(stack 불필요 —
배열 역순).

## 구성
- **빌려온 자료구조**: [[queue]], [[graph]], [[dynamic_array]], [[reservation_table]](읽기).
- **자기 데이터**: TEG 정점 표현 `(node,time)`. 별도 Data 클래스로 둘지, 정수
  인코딩(`node*(H+1)+time`)으로 둘지는 [[#미결]].
- **자기 알고리즘**: 위 BFS + 복원.

## Time Complexity
- TEG 정점 수 = V'×(H+1) (V' = 확장본 노드). 단 BFS는 시작점 H-hop 반경만
  방문 → 희소 그래프라 실제 방문은 훨씬 작음.
- 한 에이전트 탐색: O(방문 정점 + 간선). horizon 제한이 핵심.

## 의존성
### 1. 의존
- [[graph]] / [[reservation_table]] / [[queue]] / [[dynamic_array]].
- `Planner`(추상) — 상속.
### 2. 의존됨
- [[planner]](PP) — `Planner*`로 주입받아 호출.

## 사용처
- PP가 BfsTeg를 주입받은 경우의 단일 에이전트 경로 탐색.

선정 근거(왜 TEG+BFS인가, ϕ-BF와의 비교)는 [[data_structure_design#선정 근거]].

## 미결
- TEG 정점 표현: 정수 인코딩 `node*(H+1)+time` vs (node,time) 구조체/Data.
  visited·predecessor 배열 크기 = V'×(H+1)이라 인코딩이 단순할 듯.
- TEG를 명시적으로 물질화할지(정점·간선 생성), 암묵적으로 둘지(graph+time
  계산만). 메모리: 명시적은 V'×H. 암묵적이 가벼움 — 구현 시 확정.
- 대기(wait)를 항상 허용할지, 목표 도달 후 정지 처리와 어떻게 맞물리는지
  (environment의 "horizon 끝까지 정지"와 연동).
