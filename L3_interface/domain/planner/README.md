---
layer: L3
tags: [interface, domain, planner]
---

# planner 모듈 (PrioritizedPlanning + PathFinder)

## 위치
도메인 구성요소. `data_structure/`(공용 토대)를 빌려 쓴다. 의존은
planner → data_structure 한 방향. environment가 이 모듈의 PP를 호출한다.

이 모듈은 두 층을 담는다:
- **PrioritizedPlanning (PP)** — 전체 에이전트를 우선순위 순으로 조율하는
  프레임워크. environment가 매 라운드 호출.
- **PathFinder** (추상) ← BfsTeg / BellmanPhi — 단일 에이전트 경로 탐색.
  PP가 `PathFinder*`로 주입받아 호출(전략 패턴).

## 책임 (PrioritizedPlanning)
한 라운드에 전체 에이전트의 이동을 계획한다. 에이전트를 우선순위 순으로
처리하며, 각자의 경로를 PathFinder로 구하고 reservation_table에 기록해
다음(낮은 우선순위) 에이전트가 피하게 한다.

**아는 것:**
- 에이전트들을 우선순위 순으로 하나씩 처리하는 PP 절차.
- 각 에이전트의 경로를 PathFinder로 구하고, 그 결과를 reservation_table에
  기록해 다음 에이전트가 피하게 하는 흐름.

**하지 않는 것:**
- 세계를 돌리지 않는다(시간 진행·throughput은 environment). PP는 "현재 상태를
  받아 한 라운드 계획을 반환"할 뿐이다.
- 경로를 실제로 *시뮬레이션*하지 않는다.

## 구성 (자기 데이터 + 자기 알고리즘 + 빌려온 자료구조)

**빌려온 자료구조 (data_structure):**
- min_heap — 에이전트 우선순위 큐(PP).
- graph — layout (unweighted 확장본).
- queue·dynamic_array·avl_tree — PathFinder·reservation_table 내부 뼈대.

**자기 데이터 (구체 Data 자식):**
- Agent — min_heap에 담아 우선순위 추출. **소유는 environment**, PP는 빌림.
- Interval — reservation_table의 avl_tree에 담김(`[start,end)`).

**자기 알고리즘:**
- PP 조율(우선순위 순 처리·reservation 기록).
- 단일 에이전트 경로 탐색 2종 — [[bfs_teg]], [[bellman_phi]] (평행 배치,
  교체 가능). 자료구조 활용 차이는 [[data_structure_design]] 참조.
- reservation_table의 "닿으면 병합" 조율 ([[reservation_table]]).

## PrioritizedPlanning 인터페이스

environment가 매 라운드 호출하는 상위 프레임워크. 주입된 `PathFinder*`로
단일 에이전트 탐색을 위임한다.

```
class PrioritizedPlanning {
    PrioritizedPlanning(PathFinder* finder, const Graph& graph, int nodeCount);
    // 한 라운드 계획: 현재 에이전트들의 H스텝 경로를 우선순위 순으로 구해 반환.
    // reservation을 내부에서 clear→기록하며 충돌을 회피시킨다.
    void planRound(Agent* agents, int n, int H, Path* out);
};
```

- **입력**: 전체 에이전트 배열(현재 위치·목표·우선순위), 에이전트 수 n,
  계획 지평 H.
- **출력**: `out[i]` = 에이전트 i의 H스텝 [[planner#Path|Path]].
- **소유**: PathFinder·Graph는 빌림(비소유). reservation_table은 PP가 소유·관리.
  Agent는 environment 소유(PP는 우선순위로 읽고 min_heap에 빌려 담음).

## PathFinder 추상 계약 (전략 패턴)

PP는 단일 에이전트 경로를 찾을 때 `PathFinder*`(추상)에게 위임한다. 어느
알고리즘인지 모른 채 호출하고, 주입된 구현 객체가 알고리즘을 결정한다.
새 알고리즘은 클래스 추가만으로 끼운다(PP 코드 불변).

```
class PathFinder {                    // 추상
    virtual Path findPath(start, goal, graph, reservation, horizon) = 0;
};
class BfsTeg     : public PathFinder { ... };   // TEG 시간확장 + BFS
class BellmanPhi : public PathFinder { ... };   // ϕ Bellman-Ford
```

- **입력**: start/goal 노드, [[graph]](unweighted 확장본), [[reservation_table]]
  (충돌 회피용), horizon H.
- **출력**: [[#Path]] — 시각순 노드 배열. 못 찾으면 빈 Path.
- **부작용 없음**: findPath는 reservation을 *읽기만* 한다. 찾은 경로를
  reservation에 *기록*하는 것은 PP의 일(다음 에이전트가 피하도록).

### Path
findPath의 반환 타입. **모든 planner가 공유**하는 공통 구조체다(BfsTeg·ϕ-BF·
environment가 같은 타입을 쓴다).

- **무엇**: 시각순 노드 배열. `path.at(t)` = 시각 t에 에이전트가 있는 노드.
  길이는 **H+1**(시각 0..H). 같은 노드가 연속되면 그 시각엔 머무름(대기),
  목표 도달 후 남은 시각은 목표 노드로 채워진다(정지).
- **빈 Path**: 길이 0. "horizon 내 경로 없음"(도달 실패)을 뜻한다. `path.empty()`.
- **소유권/수명**: 내부 `int[]` 버퍼를 **소유하는 RAII** — 소멸자가 해제.
  복사 불가, **이동(move) 가능**이라 findPath가 값으로 싸게 반환한다. 받은
  쪽(PP/environment)이 소유.
- **연산**: `at(t)`(읽기), `set(t, node)`(빌더 측 쓰기), `length()`, `empty()`.
- environment는 receding이라 반환된 H+1 경로 중 **앞 C스텝만** 실행한다.

## PP 흐름 (한 라운드, 전체 에이전트 조율)

environment가 **매 라운드(C스텝)** 호출한다. 한 라운드는:

1. reservation을 비운다(이전 라운드 점유 clear).
2. 모든 에이전트를 우선순위 순으로 min_heap에서 꺼낸다.
3. 각 에이전트에 대해 `planner->findPath(...)`로 **H스텝 경로**를 구한다.
4. 구한 경로가 점유하는 (node, time)을 [[reservation_table]]에 기록한다
   (다음(낮은 우선순위) 에이전트가 회피).
5. 모든 에이전트의 H스텝 경로를 모아 environment에 반환. environment는 앞 C스텝만
   실행하고 다음 라운드에 다시 호출.

> H는 멀리 보는 **계획 지평**, C는 실제로 실행하는 **commit 길이**(receding
> horizon). reservation은 H스텝 전체를 기록하되, 실행은 C스텝만 하고 라운드마다
> 다시 계획한다.
> 같은 reservation을 BfsTeg는 탐색 시 isFree로, BellmanPhi는 런타임 phi로
> 읽는다 — 비교 핵심 축([[data_structure_design#사용처]]).

## 하위 구성요소
- [[reservation_table]] — 노드별 사용 불가 구간. 2층: dynamic_array<avl_tree>.
  Interval 병합 조율을 여기서 한다.
- [[bfs_teg]] — 베이스라인. TEG 1회 빌드 후 reservation을 탐색 시 isFree로 흡수.
- [[bellman_phi]] — ϕ Bellman-Ford. reservation을 interval query(ϕ)로 회피.

## 미결
- 강연결 아님: 도달 불가 목표 시 findPath empty + BFS가 horizon 전체 방문(느림).
  도달 가능 목표 보장(강연결 검증·재추첨)은 environment에서.
- H/C 기본값 튜닝(현재 H≈100, C는 파라미터).

(해소: Agent 소유=environment. Planner 추상 계약·PP 라운드 흐름 명문화. Path=길이
H+1 노드 배열 RAII. TEG 정점=정수 인코딩, 명시적 1회 빌드.)
