---
layer: L4
tags: [mechanism, planner, algorithm]
---

# BellmanPhi — 함수 명세 (L4)

상위 설계는 [[bellman_phi]] (L3). `PathFinder`를 상속해 `findPath`를 구현한다.
[[bfs_teg]]와 **같은 입력**(unweighted 확장본 [[graph]], 같은 [[reservation_table]])을
받지만, 그래프를 시간 복제하지 **않는다**. 노드별 최早 도달 시각을 SPFA식 완화로
구하고, 한 간선을 건널 때 `reservation.phi(v, t)`로 점유를 회피한다.

`phi(x, t)` = t 이상이면서 노드 x가 비는 가장 이른 시각(점유 `[c,d)`에 들면 d로
점프, 분리 구간이라 유한 번에 끝남). 이미 [[reservation_table]]이 제공한다.

**소유권**: `earliest`/`pred`/`inQueue` 배열과 큐는 탐색 상태 — `findPath` 안의
지역(또는 멤버로 1회 할당 후 세대 무효화, BfsTeg와 같은 최적화 여지). [[graph]]·
[[reservation_table]]은 비소유 읽기. 반환 [[path]]는 호출자(PP) 소유.

---

## 1. Fields

### public
- 없음 (`PathFinder` 인터페이스만).

### private
- (선택) 재사용 스크래치: `earliest[]`/`pred[]`/세대 스탬프 — 크기 `graph.size()`.
  BfsTeg의 세대 카운터와 같은 이유로 멤버화 가능(초기화 O(1)). 최초 버전은 지역.

---

## 2. Methods

### public

#### `Path findPath(start, goals[], goalCount, dwell, graph, reservation, H)` — 목표 시퀀스 ϕ 완화 + dwell + 절단
- **사전**: `start`·`goals[]` 원본 노드. `graph` 확장본. `reservation` 읽기 전용.
  `H ≥ 0`, `dwell ≥ 0`, 호출자가 H를 채울 만큼 목표를 충분히 줌.
- **사후**: start에서 `goals`를 순서대로 향하며 각 목표에서 `dwell`스텝 작업 정지, 그
  궤적의 **앞 H+1스텝**을 [[path|Path]]로 반환. 목표가 H보다 멀면 그 방향 부분 경로.
  목표 시퀀스가 H 전에 소진되면 마지막 목표에서 정지(그래서 호출자가 목표를 넉넉히
  줘야 함). 빈 Path는 *시작조차 못 떠날 때*뿐.
- **소유권**: 스크래치는 멤버 재사용. 반환 Path는 호출자 소유.

전체 구조는 **구간별 단일목표 ϕ 완화를 dwell로 이어붙인 것**이다:

```
findPath(start, goals, goalCount, dwell, graph, reservation, H):
    path = node[H+1]
    cur = start
    t   = reservation.phi(cur, 0)          // 시작 노드 첫 진입 가능 시각(보통 0)
    fill path[0 .. min(t,H)] = start       // 출발 전 대기(있다면)
    if t > H: return path                  // 시작부터 horizon 밖

    for k in 0 .. goalCount-1:
        // (1) cur@t 에서 goals[k] 까지 ϕ 완화(H 무관) — 단일목표 절과 동일
        relaxToGoal(cur, t, goals[k], graph, reservation)  // earliest[]/pred[] 채움
        arriveT = earliest[goals[k]]
        // (2) 목표서 작업 정지 — 점유 [arriveT, arriveT+1+dwell) 가 전부 비어야 함.
        //     아니면 그 창이 들어갈 만큼 도착을 늦춘다(아래 dwellStart, phiWindow).
        arriveT = dwellStart(goals[k], arriveT, dwell, reservation)
        // (3) 시각별로 path 채우기: cur@t → goals[k]@arriveT 경로 + dwell 정지
        fillSegment(path, cur, t, goals[k], arriveT, dwell, earliest, pred, H)
        // (4) 다음 구간의 출발 = 점유 창이 끝난 시점(도착 1 + 작업 dwell)
        cur = goals[k];  t = arriveT + 1 + dwell
        if t > H: return path              // horizon 다 참
    // 목표 모두 소진 & 아직 H 안 — 마지막 목표서 정지(호출자가 목표를 더 줬어야 함)
    fill path[t .. H] = cur
    return path
```

> **H 독립성**: 각 구간 완화는 노드별 `earliest`(크기 V)만 쓰므로 목표가 멀어도 자료구조
> 크기 불변(BFS+TEG의 V·(H+1)와 대비되는 핵심). H는 *반환 절단* 길이일 뿐.
>
> **종료 보장**: 각 구간 완화는 단조(엄격 감소 시만 갱신)라 유한 수렴. 구간 수는
> goalCount로 유한.

### private

#### `relaxToGoal(cur, t0, goal, graph, reservation)` — 한 구간의 ϕ 완화
- **사전**: `cur`에 시각 `t0`에 있다. `earliest`/`pred` 스크래치 보유.
- **사후**: `earliest[cur]=t0`에서 시작해 단일목표 SPFA 완화(H 무관)로 `earliest[goal]`을
  확정. 간선 완화는 `arrive = phi(v, earliest[u]+1)` (1스텝 이동 + v 점유 회피),
  `arrive < earliest[v]`면 갱신·`pred[v]=u`·큐 적재.
- **주의**: 매 구간 시작에 스크래치를 **세대 카운터로 무효화**(전체 초기화 X). `cur`만
  `t0`로 시드.

```
relaxToGoal(cur, t0, goal, graph, reservation):
    gen += 1
    earliest[cur] = t0; pred[cur] = -1; enqueue cur
    while queue not empty:
        u = dequeue; tu = earliest[u]
        for v in neighbors(u):
            a = reservation.phi(v, tu + 1)
            if visitedGen[v] != gen or a < earliest[v]:
                visitedGen[v]=gen; earliest[v]=a; pred[v]=u; enqueue v
    // earliest[goal] = 도착 시각 (강연결이라 항상 확정)
```

#### `dwellStart(goal, arriveT, dwell, reservation)` — dwell 창이 들어갈 도착 시각
- **점유 규약**: 목표 도착 시각 `t`이면 goal을 **`[t, t+1+dwell)`** 점유한다(도착 1칸 +
  작업 dwell칸). dwell=0이면 `[t, t+1)`(기존 도착 점유와 동일).
- **왜 필요**: 그 점유 창 `[t, t+1+dwell)`가 **전부 비어야** 머물 수 있다. `phi`는
  *도착 한 시점*만 보장하므로, 창이 빌 때까지 도착 `t`를 늦춘다. 안 하면 작업 정지 중
  다른 에이전트가 그 노드를 지나 충돌(자체 검토로 잡은 함정).
- **사후**: `t ≥ arriveT`이면서 `[t, t+1+dwell)`가 goal에서 전부 비는 가장 이른 `t` 반환.

```
dwellStart(goal, arriveT, dwell, reservation):
    need = 1 + dwell                           // 점유 창 길이
    t = arriveT
    loop:
        t = reservation.phi(goal, t)           // t 이상 첫 진입 가능 시각
        nextBlock = reservation.phi(goal, t+1) > t+1 ? ... // 다음 점유 시작
        if [t, t+need) 가 전부 빔: return t     // (다음 점유 시작 ≥ t+need)
        t = (그 막힌 구간의 끝)                  // 막힌 곳 뒤로 점프 후 재시도
```
(구현 노트: "창 전체가 비는가"는 reservation의 successor/phi로 합성하거나
`nextBlockedAfter`를 추가해 판정. dwell=0이면 `need=1`이라 곧 `phi(goal, arriveT)`.)

#### `fillSegment(path, cur, t0, goal, arriveT, dwell, earliest, pred, H)` — 한 구간 시각별 채움
- **사후**: `pred` 체인(goal→cur)을 따라 `cur@t0 … goal@arriveT` 시각별 위치를 채우고,
  이어서 **`[arriveT, arriveT+1+dwell)`**를 goal로 채운다(도착+작업 정지). 모두 `[0,H]`
  절단. 연속 노드의 시각 간격이 1보다 크면(phi 점프) 그 사이는 **앞 노드에 머무름**.

> **대기 점유의 정확한 규약**(인간 확정): 노드 v 진입이 점유 `[a,b)` 때문에 b로 밀리면,
> 에이전트는 *진입 직전 노드*에서 기다린다 — 그 직전 노드 도착이 `k`이면 직전 노드를
> `[k, b)` 점유(`k`에 도착해 `b−1`까지 머물다 `b`에 v로 이동). 즉 **에이전트가 각 노드에
> 머무는 전 구간**(이동 대기·작업 정지 포함)이 점유다. fillSegment가 시각별 위치를 빈틈
> 없이 채우면, PP의 `recordPath`가 매 시각 `[t,t+1)`로 reserve하고 인접 구간을 병합해
> 이 규약이 자동 성립한다 — 별도 계산 불필요. (목표 점유 `[t,t+1+dwell)`도 같은 원리.)

> **부분 경로**: 목표가 H보다 멀면 어느 구간의 채움이 H에서 잘린다 — 시각 H 위치는
> 목표로 가는 길 위의 한 점. environment는 앞 C(≤H)만 실행하므로 다음 라운드에 더
> 가까워진다(강연결이라 결국 도달).

---

## 결정된 정책
- **확장본 공유 + ϕ 시간회피**: [[bfs_teg]]와 같은 unweighted 확장본·같은
  reservation을 쓰되, 시간 복제 없이 `phi` interval query로 점유를 런타임 회피.
  공간 O(V)(복제 없음) — TEG의 O(V·(H+1)) 대비 비교의 핵심. (2026-06-07)
- **SPFA 완화 + FIFO queue**: 단위 간선이라 우선순위 큐 불필요. 갱신된 노드만
  재완화. (2026-06-07)
- **도착시각 → 시각별 경로(대기 채움)**: phi 점프로 생긴 대기를 앞 노드로 채워
  [[path]] 형식을 BFS+TEG와 일치시킴. (2026-06-07)
- **다목표 + dwell**(2026-06-08): 단일목표 완화를 `goals` 시퀀스로 이어붙이고, 각 목표서
  작업 정지. 무한 정지 제거 → target conflict 해소. 기본 `dwell=1`.
- **목표 점유 규약**(2026-06-08, 인간): 목표 도착 `t`면 goal을 `[t, t+1+dwell)` 점유
  (도착 1칸 + 작업 dwell칸). dwell=0이면 `[t, t+1)`.
- **dwell 창 점유 보장**(2026-06-08, ★중요): 위 `[t, t+1+dwell)`가 *전부 비어야* 머문다.
  `phi`는 도착 한 시점만 보장하므로 `dwellStart`로 창이 들어갈 만큼 도착을 늦춘다. 빠뜨리면
  작업 정지 중 다른 에이전트가 그 노드를 지나 충돌(자체 검토로 사전 포착).
- **대기 점유 규약**(2026-06-08, 인간): 이동 중 v 진입이 점유로 b까지 밀리면, 진입 직전
  노드를 `[그 노드 도착시각, b)` 점유(거기서 기다림). fillSegment가 시각별 위치를 채우면
  recordPath의 매-시각 reserve+병합으로 자동 기록.

## 미결
- 재사용 스크래치(세대 카운터) 멤버화 — 다목표라 구간마다 gen++로 재사용(필수에 가까움).
- 목표 시퀀스가 H 전에 소진되면 마지막 목표서 정지(유한이나, 한 라운드 내 정지) →
  environment가 목표를 *넉넉히* 줘야 라운드 내 정지가 안 생김. 공급량 기준은 environment.
- `dwellStart`의 `nextBlockedAfter`를 reservation에 추가할지, 기존 successor/phi로 합성할지
  (구현 시 결정).
