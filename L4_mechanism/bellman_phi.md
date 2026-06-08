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

#### `Path findPath(start, goal, graph, reservation, H)` — ϕ 완화 + 복원
- **사전**: `start`/`goal` 원본 노드(`< graph.originalSize()`). `graph` 확장본.
  `reservation` 읽기 전용. `H ≥ 0`.
- **사후**: horizon 내 최早 도달 경로를 [[planner#Path|Path]](길이 H+1)로 반환.
  못 찾으면(`earliest[goal] > H`) 빈 Path. 반환 경로는 점유를 피한다(phi 보장).
- **소유권**: 스크래치는 함수 끝(또는 멤버 재사용). 반환 Path는 호출자 소유.

```
findPath(start, goal, graph, reservation, H):
    n = graph.size()
    earliest = int[n] filled with INF
    pred     = int[n] filled with -1
    inQueue  = bool[n] filled with false

    s0 = reservation.phi(start, 0)        // 시작 노드 첫 진입 가능 시각
    if s0 > H: return empty               // 시작부터 horizon 내 불가
    earliest[start] = s0

    q = Queue()
    q.enqueue(start);  inQueue[start] = true

    while not q.isEmpty():
        u = q.dequeue();  inQueue[u] = false
        tu = earliest[u]
        if tu + 1 > H: continue           // 한 스텝도 더 못 감
        for i in 0 .. graph.degree(u)-1:
            v = graph.neighbor(u, i)
            arrive = reservation.phi(v, tu + 1)   // 1스텝 이동 + v 점유 회피
            if arrive <= H and arrive < earliest[v]:
                earliest[v] = arrive
                pred[v] = u
                if not inQueue[v]:
                    q.enqueue(v);  inQueue[v] = true

    if earliest[goal] > H: return empty
    return reconstruct(start, goal, earliest, pred, H)
```

> **종료 보장**: `earliest[v]`는 정수이고 0 이상, 완화는 *엄격히 더 작을 때만*
> 갱신하므로 각 노드는 유한 번만 큐에 든다(음수 사이클 없음). 시간이 단조라
> Bellman-Ford가 수렴한다.

### private

#### `reconstruct(start, goal, earliest, pred, H)` — 시각별 경로(대기 채움)
- **사전**: `earliest[goal] ≤ H`, `pred` 완성.
- **사후**: 길이 `H+1` 노드 배열. `pred` 체인을 goal→start로 역추적해 노드
  시퀀스를 얻고, 각 노드의 `earliest` 시각에 그 노드를 놓는다. **연속 노드의
  시각 간격이 1보다 크면**(phi 점프로 대기 발생) 그 사이 시각은 **앞 노드로 채움**
  (그 자리에 머무름). 시작 이전 시각(0..s0-1)도 시작 노드로 채운다(출발 대기).
  도달 후(arriveT+1..H)는 목표로 채움(정지).
- **소유권**: 새 배열 — 호출자 소유.

```
reconstruct(start, goal, earliest, pred, H):
    path = node[H+1]

    // 1) goal→start 역추적로 (노드, 도착시각) 쌍을 모은다
    seq = []                         // 시작→목표 순서가 되도록 뒤에서 push
    cur = goal
    while cur != -1:
        seq.prepend( (cur, earliest[cur]) )
        cur = pred[cur]
    // seq[0] = (start, earliest[start]=s0)

    // 2) 출발 전 대기: 시각 0..s0-1 은 start 에 머무름
    for t in 0 .. earliest[start] - 1:
        path[t] = start

    // 3) 각 구간 [노드 a @ ta] → [노드 b @ tb] 사이를 채운다
    for each consecutive (a, ta), (b, tb) in seq:
        for t in ta .. tb - 1:
            path[t] = a              // a 에 머무르다(이동 1스텝 + 대기 tb-ta-1)
    // 마지막 노드(goal)의 도착시각
    arriveT = earliest[goal]
    path[arriveT] = goal

    // 4) 도달 후 정지
    for t in arriveT + 1 .. H:
        path[t] = goal
    return path
```

> **대기의 의미**: `phi`가 `tu+1`에서 점유 구간을 만나 `tb`로 점프했다면, 에이전트는
> 노드 a에서 `tb-1`까지 기다렸다가 마지막 1스텝에 b로 이동한 것이다. 그래서 구간
> `[ta, tb)`를 a로 채우고 `tb`에 b가 놓인다. 이렇게 시각별 위치가 빈틈없이 채워져
> [[bfs_teg]]가 만드는 Path와 같은 형식이 된다(PP가 점유 기록을 동일하게 한다).

---

## 결정된 정책
- **확장본 공유 + ϕ 시간회피**: [[bfs_teg]]와 같은 unweighted 확장본·같은
  reservation을 쓰되, 시간 복제 없이 `phi` interval query로 점유를 런타임 회피.
  공간 O(V)(복제 없음) — TEG의 O(V·(H+1)) 대비 비교의 핵심. (2026-06-07)
- **SPFA 완화 + FIFO queue**: 단위 간선이라 우선순위 큐 불필요. 갱신된 노드만
  재완화. (2026-06-07)
- **도착시각 → 시각별 경로(대기 채움)**: phi 점프로 생긴 대기를 앞 노드로 채워
  [[path]] 형식을 BFS+TEG와 일치시킴. (2026-06-07)

## 미결
- 재사용 스크래치(세대 카운터) 멤버화 여부 — 최초 지역 구현 후 BfsTeg처럼 멤버로
  올릴지 측정 보고 결정.
- empty 쿼리 비용은 BFS+TEG와 같은 성격(도달 가능 노드 전수 완화). 도달 가능 목표
  보장은 environment에서.
