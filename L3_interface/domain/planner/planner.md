---
layer: L3
tags: [interface, domain, planner]
---

# PrioritizedPlanning (한 라운드 전체 조율)

> **구성 축** (axis-role: middle)
> called-by:: [[environment]]
> delegates-to:: [[pathfinding]]
> uses:: [[reservation_table]], [[min_heap]], [[graph]]
> reads:: [[agent]]
> produces:: [[path]]

## 역할
한 라운드에 **전체 에이전트의 경로를 한꺼번에** 계획한다. 에이전트를 우선순위
순으로 하나씩 처리하면서, 각자의 경로를 [[pathfinding|PathFinder]]로 구하고 그
점유를 [[reservation_table]]에 기록한다. 그러면 *나중에(낮은 우선순위)* 처리되는
에이전트가 먼저 기록된 점유를 피해 경로를 찾는다 — 이것이 충돌을 막는 유일한 장치다.

PP는 단일 에이전트 탐색을 직접 하지 않는다. 그건 주입된 [[pathfinding|PathFinder]]
(전략 패턴)에게 위임하고, PP는 *조율*만 한다 — 우선순위 순서, reservation 기록.

**하지 않는 것**
- 세계를 돌리지 않는다(시간 진행·throughput·도달 처리는 [[environment]]). PP는
  "현재 상태 → 한 라운드 계획"을 반환할 뿐이다.
- 경로를 *실행/시뮬레이션*하지 않는다.
- 단일 에이전트 탐색 알고리즘을 모른다(PathFinder 위임).

## 인터페이스

```
class PrioritizedPlanning {
    PrioritizedPlanning(PathFinder* finder, const Graph& graph, int nodeCount);
    void planRound(Agent* agents, int n, int H, int dwell, Path* out);
};
```

`planRound` — 한 라운드 계획:
- **입력**: 에이전트 배열(현재 위치·목표 시퀀스·우선순위), 에이전트 수 `n`, 계획
  지평 `H`, 목표당 작업 정지 `dwell`.
- **출력**: `out[i]` = 에이전트 `i`의 H스텝 [[path]](목표 시퀀스를 dwell 정지와 함께
  따라감). 내부에서 reservation을 clear→기록하며 충돌을 회피시킨다.
- **소유**: `PathFinder`·[[graph]]는 빌림(비소유). [[reservation_table]]은 PP가
  소유·관리. [[agent]]는 [[environment]] 소유 — PP는 우선순위로 읽고 [[min_heap]]에
  `Agent*`만 빌려 담으며, 각 에이전트의 목표 시퀀스를 findPath에 넘긴다.

## 한 라운드 흐름

[[environment]]가 **매 라운드** 호출한다:

1. **reservation 비움** — 이전 라운드 점유 clear(receding이라 매 라운드 새 계획).
2. **우선순위 정렬** — 모든 에이전트를 [[min_heap]]에 넣어 우선순위 순으로 추출.
3. **하나씩 계획** — 꺼낸 에이전트마다 `finder->findPath(start, goals, …, H)`로
   H스텝 경로를 구한다.
4. **점유 기록** — 그 경로가 점유하는 (node, time)을 [[reservation_table]]에 기록.
   다음(낮은 우선순위) 에이전트가 이를 피한다.
5. 모든 에이전트 경로를 `out[]`에 모아 반환. environment가 앞 C스텝만 실행하고 다음
   라운드에 다시 호출.

> **H vs C**: `H`는 멀리 보는 계획 지평, `C`(environment 소유)는 실제로 실행하는
> commit 길이. reservation엔 H스텝 전체를 기록하되 실행은 C스텝만 하고 재계획한다.

> **충돌 회피의 전제 — 무한 정지 없음**: 에이전트가 목표 도달 후 영구 정지하면, 그
> 점유를 *먼저 계획된* 고순위가 못 보고 통과하는 target conflict가 생긴다. 그래서
> 에이전트는 도달 시 유한 `dwell`만 정지하고 다음 목표로 떠난다([[pathfinding]]·
> [[agent]] 목표 시퀀스). 점유가 유한 구간이라 우선순위 순 기록만으로 충돌이 막힌다.

## 구성요소
- [[pathfinding]] — 단일 에이전트 탐색(추상 계약). 구현: [[bfs_teg]]·[[bellman_phi]].
- [[reservation_table]] — 노드별 점유 구간. 2층 `dynamic_array<avl_tree>`, "닿으면
  병합" 조율.
- 빌려온 토대: [[min_heap]](우선순위 큐), [[graph]](layout). 자료구조 선정 근거는
  [[data_structure_design]].

## 미결
- 우선순위 동적화(예: 대기 오래한 에이전트 우선) 여부.
- H/C·dwell 기본값 튜닝.
