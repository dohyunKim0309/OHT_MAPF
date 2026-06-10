---
layer: L4
tags: [mechanism, planner, algorithm]
---

# BfsTeg — 함수 명세 (L4)

상위 설계는 [[bfs_teg]] (L3). `PathFinder` 추상 클래스를 상속하며 `findPath`를
구현한다. TEG는 **명시적으로 물질화**한다 — [[graph]]의 `TimeExpandedGraph`를
실제로 빌드한 뒤 그 위에서 보통 BFS를 돈다. 이로써 "TEG = 그래프의 시간
복제(V·T 공간)"가 코드로 실현되고, ϕ-BF(복제 없음)와의 공간 비교가 정직해진다.

**핵심: TEG는 reservation과 무관(layout만)하게 1회 빌드해 재사용한다.**
PP의 모든 에이전트가 같은 layout·horizon을 공유하므로, TEG 뼈대(정점·간선)는
에이전트마다 같다. 달라지는 건 reservation뿐. 그래서 TEG를 **BfsTeg의 멤버**로
1회 빌드(lazy)하고, **reservation은 탐색 시 흡수**한다 — BFS가 정점 방문 시
`isFree`를 확인해 점유 `(node,t)`를 스킵. 빌드를 N회→1회로 줄인다.

**정점 인코딩**: TEG 정점 `(node, t)` → 정수 `id = node*(H+1) + t`. 역:
`node=id/(H+1)`, `t=id%(H+1)`. TimeExpandedGraph는 이 id를 정점 인덱스로 갖는다.
**규모(V=500,H≈450)**: 정점 ~955만, 빌드 ~300ms(1회), 재사용 쿼리 ~85ms.
H는 "간선 하나"가 아니라 **경로 전체 홉 수**만큼 커야 도달 가능하다(확장본에서
원본 노드 간 홉 거리 중앙값 ~405). horizon이 짧으면 도달 실패(빈 경로).

**소유권**: `TimeExpandedGraph`는 **BfsTeg의 멤버**(RAII) — 1회 빌드해 들고
있다 소멸자에서 해제. [[reservation_table]]·[[graph]]은 비소유로 읽기만.
반환 경로(Path)는 호출자(PP) 소유.

`reservation.isFree(node, t)`는 노드가 시각 t에 점유 안 됨을 뜻한다.

---

## 1. Fields

### public
- 없음 (`PathFinder` 인터페이스만).

### private (모두 멤버 — 재사용)
- `TimeExpandedGraph* teg` — layout·H로 1회 빌드해 들고 PP 전체가 재사용(RAII).
- `int builtH` — 현재 teg가 빌드된 horizon(다르면 재빌드).
- `int* visitedGen` / `int* pred` / `NodeRef* qpool` — BFS 스크래치(세대 카운터로
  매 탐색 O(1) 무효화, 큐는 Data* 기반 Queue + NodeRef 풀). 크기 V·(H+1).
- `int gen` — 현재 탐색 세대.

---

## 2. Methods

### public

#### `Path findPath(start, goals[], goalCount, dwell, graph, reservation, H)` — TEG(재사용) + 구간별 BFS
- **사전**: `start`·`goals[]`는 원본 노드. `graph`는 unweighted 확장본. `reservation`
  읽기 전용. `H ≥ 0`, `dwell ≥ 0`.
- **사후**: start에서 `goals`를 순서대로 향하며 각 목표서 `dwell`스텝 작업 정지, 그
  궤적의 앞 H+1스텝을 [[path|Path]]로 반환. 목표가 H 너머면 그 방향 부분 경로. 빈 Path는
  *시작조차 못 떠날 때*뿐. 반환 경로는 점유 (node,t)를 피한다.
- **소유권**: 멤버 `teg`는 1회 빌드 후 재사용. visited/pred 멤버 스크래치 재사용. 반환
  Path는 호출자 소유.

**구조**: TEG 정점은 `(node,t)`라 시간이 이미 박혀 있다. 그래서 한 구간 BFS가 `(cur,t0)`
에서 `(goals[k], ·)`까지 가고, **dwell은 그 목표에서 wait 간선 `dwell`개**(=`(g,t)→(g,t+1)`)
로 이어진다. wait 간선도 BFS가 `isFree`로 점검하므로 **dwell 중 점유 회피가 자동**이다
(ϕ-BF가 `dwellStart`로 따로 처리해야 하는 것과 대비 — 여기선 시간확장 덕에 공짜).

```
findPath(start, goals, goalCount, dwell, graph, reservation, H):
    if not reservation.isFree(start, 0): return empty   // 시작부터 막힘
    if teg is null or builtH != H: buildTEG(graph, H)    // 멤버 TEG 1회 빌드
    ensureScratch(graph.size()*(H+1))

    src = id(start, 0)                       // 현재 시공간 정점
    pred[src] = -1; (src 시드)
    for k in 0 .. goalCount-1:
        // (1) src 에서 goals[k] 에 처음 닿는 (goals[k], tArr) 까지 BFS (gen++ 재사용)
        gv = bfsToNode(src, goals[k], H)     // 못 닿으면(=horizon 밖) 중단
        if gv == -1: break
        // (2) dwell: (goals[k], tArr)서 wait 간선 dwell개 — 점유 [tArr, tArr+1+dwell)
        //     (도착 1칸 + 작업 dwell칸). 각 wait 칸 (g,t)를 isFree로 점검하므로, 창이
        //     점유돼 못 머물면 그 wait 가 막혀 BFS가 *더 늦은 도착*을 자연히 택한다
        //     — ϕ-BF의 dwellStart 에 해당하는 처리가 시간확장만으로 공짜.
        gv = dwellViaWaits(gv, dwell, H)     // (goals[k], tArr+dwell) 또는 H 절단
        src = gv
        if time(src) >= H: break             // horizon 다 참
    return reconstructChain(src, pred, H, goals, goalCount)
```
(BFS는 한 TEG에서 구간마다 `gen++`로 재사용. dwell·이동 모두 같은 `isFree` 점검을
거치므로 dwell 점유 함정[ϕ-BF의 dwellStart]이 구조적으로 없다.)

### private

#### `buildTEG(graph, H)` — 멤버 TEG 빌드 (layout만, reservation 무관)
- **사전**: `graph`는 확장본. `H ≥ 0`.
- **사후**: 멤버 `teg`를 `graph.size()*(H+1)` 정점으로 새로 만들고 간선을 채운다 —
  이동 `(u,t)→(v,t+1)`(graph 간선 u→v, t<H), 대기 `(u,t)→(u,t+1)`(t<H).
  **reservation은 보지 않는다**(점유 흡수는 탐색 시 findPath가 함). `builtH=H`.
- **소유권**: 멤버 `teg`를 (재)생성. 이전 teg가 있으면 먼저 해제. 1회 빌드 후
  PP의 모든 에이전트가 공유 — 빌드 N회→1회.
- **병렬 빌드**(2026-06-08): 이 빌드가 BFS+TEG의 주 비용(큰 H에서). `addEdge(from,·)`는
  `from = u*(H+1)+t` 행에만 쓰므로, 외곽 `u` 루프를 하드웨어 스레드 수만큼 분할하면 각
  스레드가 *서로 disjoint한 행*만 채워 **공유 쓰기·락 없이** 병렬화된다. 알고리즘·자료
  구조는 불변 — *빌드 방식*만 멀티코어. ThreadSanitizer로 무경쟁 검증, 결과 동일(충돌 0).
  (실측 4코어: 1회 빌드 370→209ms.)

```
buildTEG(graph, H):
    delete teg
    n = graph.size()
    teg = TimeExpandedGraph(n*(H+1))
    for u in 0 .. n-1:
        for t in 0 .. H-1:                     // t==H 에선 나가는 간선 없음
            from = id(u, t)
            teg.addEdge(from, id(u, t+1))       // 대기
            for i in 0 .. graph.degree(u)-1:
                v = graph.neighbor(u, i)
                teg.addEdge(from, id(v, t+1))   // 이동
    builtH = H
```

#### `reconstruct(goalVertex, pred, H, goal)` — 복원 + 도달 후 정지 채움
- **사전**: `goalVertex` 도달됨, `pred` 완성.
- **사후**: 길이 `H+1` 노드 배열. 도달 시각까지 역추적, 이후 시각은 목표로
  채움(정지). 이 정지 구간도 경로에 있어 PP가 목표 점유를 reservation에 기록.
- **소유권**: 새 배열 — 호출자 소유.

```
reconstruct(goalVertex, pred, H, goal):
    path = node[H+1]
    cur = goalVertex
    arriveT = goalVertex % (H+1)
    while cur != -1:
        path[ cur%(H+1) ] = cur/(H+1)
        cur = pred[cur]
    for t in arriveT+1 .. H:
        path[t] = goal                         // 도달 후 정지
    return path
```

---

## 결정된 정책
- **TEG 명시적 물질화 + 1회 빌드 재사용**: TimeExpandedGraph를 실제 빌드
  (정점 V'×(H+1), 간선 이동+대기). **reservation과 무관(layout만)하게 빌드**해
  BfsTeg 멤버로 들고 PP 전체가 재사용 — 빌드 N회→1회. reservation은 **탐색 시
  흡수**(BFS가 점유 (node,t) 스킵). "TEG=시간 복제(V·T 공간)" 비교 가설은
  유지(공간은 여전히 V·T). (2026-06-06, 인간 통찰로 빌드시→탐색시 흡수로 전환)
- **정수 인코딩** `node*(H+1)+t`로 TEG 정점 = 일반 그래프 정점. (2026-06-06)
- **반환 = 길이 H+1 노드 배열**, 도달 후 목표로 정지 채움. (2026-06-06)
- **H는 경로 홉 수만큼**: "간선 하나"가 아니라 경로 전체 길이여야 도달.
  확장본 원본 노드 간 홉 거리 중앙값 ~405 → H를 그만큼 줘야 함. 실측: H≈450에서
  빌드 ~300ms(1회) + 재사용 쿼리 ~85ms. (2026-06-06)

## 결정된 정책 (추가)
- **세대 카운터**: visitedGen/pred/q를 멤버로 1회 할당, 매 findPath에 `gen += 1`로
  방문 표시를 O(1) 무효화(전체 초기화 X). `visitedGen[v]==gen`이면 이번 탐색 방문.
  found 쿼리가 12ms까지 내려가 초기화 제거 효과 확인. (2026-06-06)
- **다목표 + dwell**(2026-06-08): 한 TEG 안에서 구간마다 BFS를 이어, `(start,0)`→
  goals[0]→(wait dwell)→goals[1]→… 한다. **dwell은 wait 간선 `dwell`개**라 각 `(g,t)`가
  `isFree`로 점검돼 dwell 중 점유 회피가 자동(ϕ-BF는 `dwellStart`로 별도 처리해야 하는
  것과 대비 — 시간확장의 이점). 무한 정지 없음 → target conflict 해소. 구간마다 `gen++`로
  스크래치 재사용.

## 미결
- **empty(도달 실패) 쿼리 병목**: 목표 도달 못 하면 BFS가 horizon 내 도달 가능
  정점을 전부 방문 후 실패(100~200ms). 세대 카운터로 *초기화*는 없앴으나 *방문량*
  은 알고리즘 본질. 근본 원인은 강연결 아님(도달 불가 목표). → environment에서
  도달 가능 목표 보장(강연결 검증·재추첨)으로 완화.
- 시작 노드가 원본인지(에이전트는 원본 노드에 있다고 가정) — environment 보장 확인.
