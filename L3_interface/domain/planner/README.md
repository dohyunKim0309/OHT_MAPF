---
layer: L3
tags: [interface, domain, planner]
---

# Planner (PP 플래닝)

## 위치
도메인 구성요소. `data_structure/`(공용 토대)를 빌려 쓴다. 의존은
planner → data_structure 한 방향.

## 책임
Planner는 곧 **PP(Prioritized Planning)로 전체 에이전트의 이동을 계획하는
것**이다. 별도의 상위 층이 아니라, Planner가 PP 그 자체다. 내부에서 단일
에이전트 경로 탐색을 부품으로 쓴다.

**아는 것:**
- 에이전트들을 우선순위 순으로 하나씩 처리하는 PP 절차.
- 각 에이전트의 경로를 단일 에이전트 planner로 구하고, 그 결과를
  reservation_table에 기록해 다음 에이전트가 피하게 하는 흐름.

**하지 않는 것:**
- 세계를 돌리지 않는다(시간 진행·throughput은 environment). Planner는
  "현재 상태를 받아 경로 계획을 반환"할 뿐이다.
- 경로를 실제로 *시뮬레이션*하지 않는다.

## 구성 (자기 데이터 + 자기 알고리즘 + 빌려온 자료구조)

**빌려온 자료구조 (data_structure):**
- min_heap — 에이전트 우선순위 큐.
- queue — 단일 에이전트 탐색/전파 큐.
- avl_tree, dynamic_array — reservation_table의 2층 구조.
- graph — layout (weighted 원본 + unweighted 확장본).

**자기 데이터 (구체 Data 자식):**
- Agent (min_heap에 담김) — *미결: 이 모듈인지 environment 소유인지.
  현재 결정: 소유는 environment, planner는 빌려 우선순위만.*
- Interval (reservation_table의 avl_tree에 담김) — `[start,end)`.
- (TEG 정점 — queue에 담김. graph의 Graph Node 직접 사용 여부 미결.)

**자기 알고리즘:**
- 단일 에이전트 경로 탐색 2종 — [[bfs_teg]], [[bellman_phi]] (평행 배치,
  교체 가능). 자료구조 활용 차이는 [[data_structure_design]] 참조.
- reservation_table의 "닿으면 병합" 조율 ([[reservation_table]]).

## Planner 추상 계약 (전략 패턴)

PP는 단일 에이전트 경로를 찾을 때 `Planner*`(추상)에게 위임한다. 어느
알고리즘인지 모른 채 호출하고, 주입된 구현 객체가 알고리즘을 결정한다.
새 알고리즘은 클래스 추가만으로 끼운다(PP 코드 불변).

```
class Planner {                       // 추상
    virtual Path findPath(start, goal, graph, reservation, horizon) = 0;
};
class BfsTeg     : public Planner { ... };   // TEG 시간확장 + BFS
class BellmanPhi : public Planner { ... };   // ϕ Bellman-Ford
```

- **입력**: start/goal 노드, [[graph]](unweighted 확장본), [[reservation_table]]
  (충돌 회피용), horizon H.
- **출력**: 경로 — 시각순 노드 시퀀스(없으면 빈 경로). horizon 내에서 못 찾으면
  실패.
- **부작용 없음**: findPath는 reservation을 *읽기만* 한다. 찾은 경로를
  reservation에 *기록*하는 것은 PP의 일(다음 에이전트가 피하도록).

## PP 흐름 (전체 에이전트 조율)

1. 모든 에이전트를 우선순위 순으로 min_heap에서 꺼낸다.
2. 각 에이전트에 대해 `planner->findPath(...)`로 경로를 구한다.
3. 구한 경로가 점유하는 (node, time)을 [[reservation_table]]에 기록한다
   (다음(낮은 우선순위) 에이전트가 회피).
4. 모든 에이전트의 경로를 모아 environment에 반환.

> 같은 reservation을 BfsTeg는 빌드 필터(isFree)로, BellmanPhi는 런타임
> 질의(phi)로 읽는다 — 비교 핵심 축([[data_structure_design#사용처]]).

## 하위 구성요소
- [[reservation_table]] — 노드별 사용 불가 구간. 2층: dynamic_array<avl_tree>.
  Interval 병합 조율을 여기서 한다.
- [[bfs_teg]] — 베이스라인. reservation을 TEG 빌드 시 정점 제거로 흡수.
- [[bellman_phi]] — ϕ Bellman-Ford. reservation을 interval query(ϕ)로 회피.

## 미결
- TEG 정점 표현((node,time)을 어떤 자료구조로 — 명시적 물질화 vs 암묵적) —
  [[bfs_teg]]에서 확정.
- 경로(Path) 반환 형식의 구체 타입 — 노드 배열 / (node,time) 배열.
- 새 목표 생성·도달 가능성 — environment와 함께.

(해소: Agent 소유는 environment 확정. Planner 추상 계약 위 명문화.)
