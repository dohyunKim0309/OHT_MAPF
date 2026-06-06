---
layer: L4
tags: [mechanism, planner]
---

# PrioritizedPlanning — 함수 명세 (L4)

상위 설계는 [[planner]] (L3, #PrioritizedPlanning 인터페이스). 한 라운드에 전체
에이전트의 H스텝 경로를 우선순위 순으로 구하고, 각 경로를 reservation에 기록해
다음(낮은 우선순위) 에이전트가 피하게 한다. 단일 에이전트 탐색은 `PathFinder*`에
위임(전략 패턴).

**소유권**: `PathFinder*`·`Graph`는 빌림(비소유). `ReservationTable`은 PP가
소유·관리(라운드마다 clear→기록). `Agent`는 environment 소유 — PP는 읽고
min_heap에 포인터만 빌려 담는다.

---

## 1. Fields

### public
- 없음.

### private
- `PathFinder* finder` — 주입된 단일 에이전트 탐색기(비소유).
- `const Graph* graph` — layout(비소유).
- `ReservationTable reservation` — PP 소유. 노드 수로 생성.

---

## 2. Methods

### public

#### `void planRound(Agent* agents, int n, int H, Path* out)` — 한 라운드 계획
- **사전**: `agents`는 n개 에이전트(현재 위치·목표·우선순위). `out`은 n칸 배열.
  `H ≥ 0`.
- **사후**: `out[i]` = 에이전트 i의 H스텝 [[planner#Path|Path]](못 찾으면 빈
  Path). 높은 우선순위가 먼저 계획되어 reservation에 기록되고, 낮은 우선순위는
  그것을 회피. reservation은 이 라운드의 점유로 채워진 상태로 남는다(다음
  라운드 시작에 다시 clear).
- **소유권**: `out[i]`(Path)는 호출자(environment) 소유. reservation의 Interval은
  PP 소유(clear가 정리).

```
planRound(agents, n, H, out):
    reservation.clear()                       // 이전 라운드 점유 비움

    // 우선순위 순으로 에이전트 처리 (min_heap: 작은 priority가 먼저)
    heap = MinHeap()
    for i in 0 .. n-1:
        heap.push(agents[i])                  // Agent* 빌려 담음 (operator< = priority)

    while not heap.isEmpty():
        a = heap.pop()                        // 다음 우선순위 에이전트
        path = finder.findPath(a.current, a.goal, *graph, reservation, H)
        out[a.id] = move(path)                // 그 에이전트 자리에 저장
        recordPath(out[a.id], H)              // reservation에 점유 기록 (아래)
```

### private

#### `void recordPath(Path& path, int H)` — 경로의 점유를 reservation에 기록
- **사전**: `path`는 길이 H+1 노드 배열(빈 Path면 기록 없음).
- **사후**: 경로가 시각 t에 머무는 노드 `path.at(t)`를 reservation에 점유로
  기록. 다음(낮은 우선순위) 에이전트의 findPath가 isFree/phi로 이를 회피한다.
- **방법**: 시각 t의 점유는 `[t, t+1)` 구간. 같은 노드에 연속으로 머물면
  reserve의 병합이 자동으로 `[t, t+k)`로 합친다 — 매 시각 reserve해도 트리는
  분리·정렬·병합 상태를 유지.

```
recordPath(path, H):
    if path.empty(): return
    for t in 0 .. H:
        node = path.at(t)
        reservation.reserve(node, t, t+1)     // 병합은 reserve가 알아서
```

> 빈 Path(도달 실패) 에이전트는 점유를 기록하지 않는다 — 이번 라운드엔 못 움직임.
> environment가 그 에이전트를 제자리에 두거나 다음 라운드 재시도.

---

## 결정된 정책
- **라운드마다 reservation clear**: receding horizon이라 매 라운드 새 계획.
  이전 라운드 점유는 무효. (2026-06-06)
- **점유 기록은 시각별 `[t,t+1)`**: reserve의 병합이 연속 점유를 구간으로 합침.
  대기(같은 노드 연속)도 자동 병합. (2026-06-06)
- **우선순위 = Agent.priority**, min_heap으로 추출(작은 값 먼저). (2026-06-06)

## 결정된 정책 (L5 작성 중 확정, 2026-06-06)
- **도달 실패(빈 Path)**: 제자리 유지(environment가 `continue`로 그 스텝 안 움직임).
- **우선순위**: `Agent.priority` 고정, 기본 `priority = id`(잠정, 동적은 추후).
- **out 버퍼**: PP는 호출자가 준 `Path[n]`을 채운다. Path는 `int[]` RAII이며
  기본 생성자(빈 Path)를 가져 호출자가 `new Path[n]`로 만든다(정보 모순 해소).

## 미결
- 우선순위 동적화(예: 대기 오래한 에이전트 우선) 여부.
