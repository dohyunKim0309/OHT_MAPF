---
layer: L4
tags: [mechanism, environment]
---

# Environment — 함수 명세 (L4)

상위 설계는 [[environment]] (L3, #Environment 인터페이스). 세계를 보유하고
receding horizon으로 시뮬레이션을 돌린다 — 매 라운드 [[planner|PP]]에게 H스텝
계획을 받아 앞 C스텝만 실행, 도달 처리·throughput 측정.

**소유권**: `Agent` 배열은 environment 소유. `Graph`·`PrioritizedPlanning*`는
빌림. PP가 반환한 `Path`는 environment가 받아 소유(C스텝 실행에 사용 후 해제).

---

## 1. Fields

### public
- 없음.

### private
- `Agent* agents` — n개. environment 소유.
- `int n` — 에이전트 수.
- `const Graph* graph` — layout(비소유).
- `PrioritizedPlanning* pp` — 빌림.
- `int H` — 계획 지평. `int C` — commit 실행 스텝(C ≤ H).
- `int now` — 현재 시각(누적 스텝). `int arrived` — 누적 도달 카운트.

---

## 2. Methods

### public

#### `void step()` — 한 라운드 (계획 → C스텝 실행 → 도달 처리)
- **사전**: agents가 유효한 현재 위치·**목표 시퀀스**(큐가 차 있음)를 가짐.
- **사후**: 각 에이전트가 자기 H스텝 경로의 앞 C스텝을 따라 전진. C스텝 안에 현재 목표
  (`goals[0]`)에 도달한 에이전트는 카운트++, 그 목표를 큐에서 소비하고 **새 랜덤 목표를
  큐에 보충**(무한 스트림). `now += C`.
- **소유권**: PP가 준 Path 배열을 받아 쓰고 라운드 끝에 해제.

```
step():
    paths = Path[n]
    pp.planRound(agents, n, H, dwell, paths)   // dwell 포함

    for c in 1 .. C:                           // 앞 C스텝 실행
        for i in 0 .. n-1:
            if paths[i].empty(): continue      // 시작 막힘 — 제자리
            agents[i].current = paths[i].at(c) // 시각 c 위치로 전진
            if agents[i].current == agents[i].currentGoal():   // == goals[0]
                arrived += 1
                agents[i].advanceGoal(randomOriginalNode(graph))
                // 큐에서 도달 목표 소비 + 새 랜덤 목표 보충(큐 항상 채움)
    now += C
```

> **무한 정지 없음**: 에이전트가 목표 도달 후 영구 정지하지 않는다 — 경로는 `dwell`스텝
> 작업 정지 후 *큐의 다음 목표*로 계속 이어간다(findPath 다목표). 이것이 target conflict
> (목표서 영구 정지하는 저순위를 먼저 계획된 고순위가 통과)를 없앤 핵심. (2026-06-08)
>
> **큐를 채워 둔다**: findPath가 H스텝을 정지 없이 채우려면 충분한 목표가 필요하다.
> environment는 도달마다 보충해 큐를 K개로 유지(K는 H를 덮을 lookahead). 큐가 비면
> 그 라운드 내 정지가 생길 수 있으므로 보충이 필수.

#### `void run(int rounds)` — step 반복
```
run(rounds):
    for r in 1 .. rounds: step()
```

#### `double throughput() const` — 처리율
- **사후**: `arrived / now` (now=0이면 0.0). 단위 시각당 도달 수. **실수 반환** —
  소규모 함대는 보통 스텝당 1 미만이라 정수 절단을 피한다.

```
throughput():
    return now == 0 ? 0.0 : (double)arrived / now
```

#### `int arrivedCount() const` / `int elapsed() const` — 관측용 게터
- 누적 도달 수·경과 시각을 노출(렌더·로깅이 원시값으로 비율을 구성).

#### `void render() const` — visualizer
- 세계 상태(각 에이전트의 current 노드)를 출력. 형식(텍스트/그래픽)은 구현 시.

### private

#### `int randomOriginalNode(graph)` — 새 목표 추첨
- **사후**: `[0, graph.originalSize())`에서 랜덤 원본 노드 반환(가상노드 제외).
  강연결 아니면 도달 불가일 수 있음 — 그 경우 다음 라운드 findPath가 빈 Path,
  재시도(또는 재추첨)는 [[#미결]].

```
randomOriginalNode(graph):
    return rand() % graph.originalSize()
```

---

## 결정된 정책
- **receding**: step 한 번 = 한 라운드. H스텝 계획 중 앞 C스텝만 실행. (2026-06-06)
- **도달 처리**: C스텝 중 현재 목표 도달 시 카운트++, 큐에서 소비 + 새 랜덤 목표 보충.
  (2026-06-06; 다목표 큐로 갱신 2026-06-08)
- **무한 목표 스트림 + dwell**(2026-06-08): 에이전트는 목표 큐를 들고, 도달 시 다음
  목표를 즉시 보충(무한). 도달 목표마다 `dwell`스텝 작업 정지(fab 적재/하역 모델링).
  영구 정지가 없어 target conflict가 사라진다.
- **throughput = arrived / now**, **double 반환**(정수 절단 방지). 관측용
  `arrivedCount()`/`elapsed()` 게터 추가. (2026-06-06)

## 결정된 정책 (L5 작성 중 확정, 2026-06-06)
- **초기 배치**: 생성자에서 `[0, originalSize())` 랜덤 시작·랜덤 목표(seed로 재현).
  시나리오 파일은 추후 여지.
- **우선순위**: `priority = id`(잠정). Agent 생성자가 priority를 인자로 받아 외부에서
  다르게 줄 수 있는 상태는 유지 — 동적 우선순위는 추후.

## 미결
- 도달 불가 목표(강연결 아님) 재추첨/재시도 정책.
- C스텝 중 목표 도달 후 즉시 새 목표로 이어 갈지(이번 라운드 내), 다음 라운드에
  갈지 — 현재는 다음 라운드(경로가 옛 목표 기준이라).
