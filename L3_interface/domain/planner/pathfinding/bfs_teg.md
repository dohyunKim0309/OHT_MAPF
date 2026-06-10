---
layer: L3
tags: [interface, domain, planner, algorithm]
---

# BfsTeg (BFS + Time-Expanded Graph)

> **구성 축** (axis-role: bottom)
> inherits:: [[pathfinding|PathFinder]]
> uses:: [[graph]], [[queue]], [[reservation_table]], [[dynamic_array]]
> produces:: [[path]]

## 위치
[[planner]]의 단일 에이전트 경로 탐색 구현 중 하나. `PathFinder` 추상 클래스를
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
- **목표 시퀀스**: 한 목표에 도달하면 그 (node,t)에서 `dwell`스텝 정지(작업) 후 그
  시각·노드를 새 출발로 다음 목표 BFS — H까지 이어붙임. 단 TEG가 H스텝까지만 물질화돼
  있어, 먼 목표까지 닿으려면 거리에 맞는 H 필요(ϕ-BF와의 비대칭).

**하지 않는 것:**
- reservation을 *기록*하지 않는다 — 읽기(isFree)만. 기록은 PP.
- 우선순위 비교를 하지 않는다 — unweighted라 BFS(FIFO)면 최단. min_heap 불필요.

## 구현 방식

### TEG 정점과 전이
정점 = `(node, time)`, `0 ≤ time ≤ H`. 시작 `(start, 0)`. 각 스텝:
- **이동**: graph의 각 이웃 v에 대해 `(v, t+1)`.
- **대기**: `(u, t+1)` (제자리).
모든 간선 길이 1이라 시간 1스텝 = 간선 1개(unweighted 확장본을 쓰는 이유).

### TEG 1회 빌드 + reservation 탐색 시 흡수
TEG 뼈대(정점·간선)는 layout·horizon에만 의존하고 reservation과 무관하다.
PP의 모든 에이전트가 같은 뼈대를 쓰므로 **1회만 빌드해 재사용**(BfsTeg 멤버).
reservation은 **탐색 시** 흡수한다 — BFS가 정점 `(v,t+1)`로 전이하기 전
`reservation.isFree(v, t+1)`를 확인해 점유면 스킵(TEG에서 제거된 것과 동치).
빌드 시 흡수하면 에이전트마다 TEG가 달라져 재빌드해야 하므로, 탐색 시로 둔다.

### 탐색 (BFS)
- **빌려 쓰는 자료구조**: FIFO 큐, TEG 이웃 순회, visited/predecessor 배열.
- visited: `(node,time)` 단위. 같은 정점 재방문 방지. (물리 노드는 여러 time =
  여러 정점이라, 한 탐색에서 각 TEG 정점은 한 번씩만 방문.)
- **세대 카운터**: visited/pred/큐를 멤버로 1회 할당, 매 호출 세대(gen) 증가로
  초기화를 O(1)에. `visitedGen[v]==gen`이면 이번 탐색에서 방문. (955만 칸을
  매번 0으로 미는 비용 제거.)
- BFS라 처음 도달한 `(goal, t)`가 최단(가장 이른 도달). 거기서 종료.
- horizon H까지 못 찾으면 실패(빈 경로). 도달 실패 시 horizon 전체를 훑어 느림 —
  강연결 아님이 원인(environment에서 도달 가능 목표 보장으로 완화).

### 경로 복원
predecessor를 `(node,time)`별(=vertexId별)로 기록(dynamic_array). 목표 정점에서
시작까지 predecessor 체인을 역추적, 뒤집어 시각순 노드 시퀀스를 만든다(stack
불필요 — 배열 역순).

### 반환 형식
공통 [[path|Path]] 구조체(길이 H+1 노드 배열)로 반환한다. 목표 시퀀스를 구간별로
이어 채우고, 각 목표 도착 후 `[arriveT, arriveT+1+dwell)`를 정지(도착+작업)로 채운다.
목표가 H 너머면 그 방향 **부분 경로**(H에서 절단), 목표 큐가 H 전 소진되면 마지막
목표서 정지. 빈 Path는 *시작조차 못 떠날 때*뿐. 상세는 [[path]].

## 구성
- **빌려온 자료구조**: [[queue]], [[graph]], [[dynamic_array]], [[reservation_table]](읽기).
- **자기 데이터**: TEG 정점은 **정수 인코딩** `vertexId = node*(H+1) + time`.
  - 디코딩: `node = id/(H+1)`, `time = id%(H+1)`.
  - visited/predecessor를 `dynamic_array[vertexId]`로 O(1) 인덱싱(크기 V'×(H+1)).
  - graph가 노드를 정수 인덱스로 다루므로 이와 일관(별도 Node* 객체 불필요).
- **소유권**: `TimeExpandedGraph`는 **BfsTeg의 멤버**(RAII). reservation과
  무관하게 layout만으로 **1회 빌드**해 들고, PP의 모든 에이전트가 재사용
  (빌드 N회→1회). 소멸자가 해제. reservation 흡수는 빌드가 아니라 **탐색 시**
  BFS가 `isFree`로 점유 정점을 스킵하는 방식 — 그래야 TEG를 재사용할 수 있다.
- **자기 알고리즘**: TEG 1회 빌드 + 그 위 BFS(탐색 시 isFree 흡수) + 복원.

## Time Complexity
- TEG 정점 수 = V'×(H+1) (V' = 확장본 노드). 단 BFS는 시작점 H-hop 반경만
  방문 → 희소 그래프라 실제 방문은 훨씬 작음.
- 한 에이전트 탐색: O(방문 정점 + 간선). horizon 제한이 핵심.

## 의존성
### 1. 의존
- [[graph]] / [[reservation_table]] / [[queue]] / [[dynamic_array]].
- `PathFinder`(추상) — 상속.
### 2. 의존됨
- [[planner]](PP) — `PathFinder*`로 주입받아 호출.

## 사용처
- PP가 BfsTeg를 주입받은 경우의 단일 에이전트 경로 탐색.

선정 근거(왜 TEG+BFS인가, ϕ-BF와의 비교)는 [[data_structure_design#선정 근거]].

## 미결
- (해소 2026-06-08) TEG 명시적 물질화 vs 암묵적 → **명시적 물질화로 확정**(L4·L5
  구현 완료). "TEG=V·T 공간 복제" 비교 가설을 코드로 실현하려면 명시적이어야 함.
  정점 표현은 정수 인코딩.

(해소: 정점 표현 = 정수 인코딩. 반환 = 길이 H 노드 배열. 대기/정지는
반환 형식이 흡수.)
