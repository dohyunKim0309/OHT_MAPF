# 발표 (10분 · 영어) — 단일 문서

이 문서가 발표의 **유일한 출처**다. 슬라이드 구성·말할 내용·시각물 제작계획을 한곳에.

## 규칙
- 슬라이드당 **이미지 ≤ 2–3개**, **글 ≤ 50단어**(영어 키워드/명사구). 나머진 *말로*.
- 구조: **큰 번호 = 섹션(내용 단위)**, **소번호 = 실제 슬라이드 1장**.
- 각 슬라이드에 3가지를 적는다 — **[화면]**(보이는 것), **[말]**(스크립트 요지),
  **[자료]**(시각물 + 누가: AI 제작 / YOU 캡처 / DONE 기존).
- 평가 4요건(영상): subject · code · why · results + 자료구조 축(WHERE/HOW/WHY).

## 시각물 한눈에 (누가 만드나)
- **AI 제작**: 그림 A(왜 시간), 그림 B-1(TEG 격자), 가상노드 펼침,
  양자화 표, 받침 자료구조 표, **reservation 2층 그림 + 합치기 애니메이션**(6.3),
  **min-heap 그림**(6.4), 논문 1p 캡처, .pptx 조립.
- **YOU(캡처)**: GitHub repo 스크린샷, 실행 스크린샷(대형 설정),
  **코드 캡처 3개** — `Interval::operator<` · `Interval::touches`/`merge`(6.3) ·
  `Agent::operator<`(6.4).
- **DONE(기존)**: AVL 회전 그림 5개(`results/report/assets/`·원래 L3 assets), 영어 보고서(텍스트 출처).
- 모든 자료는 `results/presentation/assets/`에 모은다.

---

# 1. Intro

### 1.1 Title
- **[화면]** 제목 "Multi-Agent Path Finding on a Real Fab OHT Network — Data Structure Project"
  · 김도현 · 202214223 · 전기전자공학과
- **[말]** 한 문장 소개.
- **[자료]** 텍스트만.

### 1.2 The problem  [subject]
- **[화면]** fab OHT 이미지 1장. 글(≤10): "Hundreds of OHTs share one rail — collision stalls production."
- **[말]** fab에서 수백 대 OHT가 한 레일 공유; 충돌·교착이면 생산 정지. 충돌 없이 경로를
  짜는 게 MAPF.
- **[자료]** OHT fab 이미지 — YOU(저작권 OK인 것) 또는 AI 단순 모식도.

---

# 2. Problem setup  (데이터보다 먼저)

### 2.1 Discrete-time motion model  [subject·why]
- **[화면]** 모식도: 한 스텝에 V·Δt 이동 또는 대기, directed graph 한 조각.
  글(≤15): "Directed graph · step = move V·Δt or wait · min separation V·Δt."
- **[말]** 시간은 Δt 이산. 한 스텝에 속도 V로 V·Δt 이동 또는 대기. 뒤차는 V·Δt 간격 유지.
- **[자료]** AI 모식도.

### 2.2 Edge → unit edges (virtual nodes)  [subject·why]
- **[화면]** **가상노드 펼침 그림**(간선 w → 노드 w−1개 체인, 모두 길이 1).
  글(≤12): "Split each edge into unit steps → collision = same (node, time)."
- **[말]** 간선을 가상노드로 펼쳐 모든 간선 길이 1 → "엣지 위 최소거리 충돌"이
  "(node,time) 점유 충돌" 하나로 통일. 단방향이라 swap 충돌 없음 = 노드 충돌만.
- **[자료]** AI 그림(가상노드 펼침).

### 2.3 The conflict: why time is part of the problem  ★  [subject·why]
- **[화면]** **그림 A** — 두 에이전트 각자 최단경로 → 같은 노드 같은 시각 충돌.
  글(≤10): "Independently optimal paths collide. The missing dimension is *time*."
- **[말]** 단일이면 최단경로로 끝. 여럿이면 각자 최적이 같은 점을 같은 순간에 지나 충돌 —
  그래서 충돌 모델이 곧 문제의 일부다. 빠진 차원은 시간: *언제* 어디 있는지를 알아야 회피.
  이게 우리가 MAPF로 푸는 이유.
- **[자료]** 그림 A — DONE.

---

# 3. Data acquisition  [subject·why]

### 3.1 What data
- **[화면]** **논문 1p 캡처** + **GitHub repo 캡처**(2개).
  글(≤15): "SMAT 2022 OHT network — Choi et al., PLOS ONE 2024 · 2,858 nodes / 3,424 edges."
- **[말]** 실제 fab OHT 망(삼성 협업 PLOS ONE 공개 데이터). 토이 아닌 실데이터. 토폴로지만
  사용(좌표 기밀).
- **[자료]** 논문 1p PNG — AI; repo 스크린샷 — YOU.

### 3.2 Weighted → needs quantization
- **[화면]** **짧고 큰 글씨만**(≤8): "Directed **weighted** graph → quantize to integer steps."
- **[말]** 거리가 실수 weight라 이산 시간엔 정수 스텝으로 양자화해야(=거리÷V·Δt 반올림).
- **[자료]** 없음(또는 화살표 1개).

### 3.3 Preprocessing — two stages (code)
- **[화면]** **코드 캡처 2장**(넘기며): `convert_weights`(거리→정수 스텝, round+clamp) /
  `expand_unweighted`(weight-w → w−1 가상노드 체인). 글(≤14): "stage 1 quantize · stage 2
  expand to unit edges."
- **[말]** 직접 짠 전처리. **stage 1** `convert_weights` = 거리÷V 반올림(+clamp≥1)으로
  정수 스텝. 이 코드가 있어야 **오차를 측정·튜닝**해 원하는 만큼 처리할 수 있다.
  **stage 2** `expand_unweighted` = weight-w 간선을 w−1 가상노드 체인으로 → 모두 길이 1.
- **[자료]** **YOU 코드 캡처**: `convert_weights` / `expand_unweighted` (두 슬라이드).

### 3.4 Quantization options — the tuning result
- **[화면]** **양자화 옵션 표**(V / maxW / 간선오차 / 경로오차 / 노드수).
- **[말]** 그 코드로 V를 바꿔가며 측정한 결과. V를 키우면 그래프 작아지나 오차↑. 경로
  오차가 진짜 척도(간선 오차는 합산되며 상쇄). 채택 V=500 → 경로오차 3.6%, 21,178노드.
- **[자료]** 양자화 표 — DONE(`data/PREPROCESSING.md §5`).

---

# 4. The big picture — how the simulator is organized  ★  [subject·why]

> 자료구조를 나열하기 전에, *시스템*을 먼저 보여준다. 그러면 자료구조는
> "나열"이 아니라 "이 시스템이 요구하는 것"으로 자연히 끌려 나온다. (top-down)

### 4.1 Prioritized Planning (PP)
- **[화면]** 모식도: 에이전트 3대를 우선순위 순(1→2→3)으로 하나씩 계획, 각 경로를
  점유로 칠해 다음 에이전트가 피함. 글(≤16): "Plan agents one at a time, in priority
  order. Each committed path becomes occupancy the next agent must avoid."
- **[말]** MAPF를 한 번에 푸는 건 비싸다. **Prioritized Planning** = 우선순위 순으로
  에이전트를 *하나씩* 계획하고, 그 경로를 점유로 기록 → 다음(저순위) 에이전트가 그걸
  피해 계획. 빠르고 단순(완전·최적은 포기). 이게 우리 top-level 틀.
- **[자료]** PP 모식도 — AI.

### 4.2 Receding horizon + three roles
- **[화면]** 블록 그림: **Environment**(시간 진행, H계획·C실행·재계획) → **PP**(우선순위
  조율) → **PathFinder**(단일 에이전트 탐색). 글(≤14): "Environment runs time · PP
  orders agents · PathFinder routes one agent."
- **[말]** 세 역할. **Environment**는 한 번에 H스텝 계획하되 앞 C스텝만 실행하고 재계획
  (receding horizon). **PP**는 우선순위로 에이전트를 꺼내 조율. **PathFinder**는 한
  에이전트의 시간제약 경로를 찾는다(교체 가능). 다음 절이 PathFinder의 실체.
- **[자료]** 3역할 블록 그림 — AI.

### 4.3 The PathFinder: BFS over a Time-Expanded Graph  ★
- **[화면]** **그림 B-1**(TEG 격자 — (node,t), 점유 X, BFS 경로). 글(≤15): "PathFinder =
  BFS on a Time-Expanded Graph (TEG): replicate the graph per step, avoid via `isFree`."
- **[말]** PathFinder의 실체 = **BFS + TEG**. 시간을 다루려고 그래프를 시각마다 복제(TEG),
  점유 (node,t)를 걸러 BFS. 충돌 0. — 여기서 *왜 필요한 게 무엇인지*가 드러난다(다음).
- **[자료]** 그림 B-1 — DONE.

### 4.4 What this picture needs — and why  ★
- **[화면]** 구성요소 → 필요 이유 표/화살표:
  - **BFS** (TEG 탐색) → 레벨 순 프런티어가 필요
  - **TEG** (시간 확장 그래프) → layout을 시각마다 복제해 표현
  - **Reservation** (점유 저장·질의) → 모든 에이전트의 경로를 충돌 회피 근거로
  - **priority 추출** (PP가 다음 에이전트) → 우선순위 순으로 꺼냄
  글(≤16): "BFS needs a frontier · TEG & layout are graphs · reservation stores occupancy ·
  PP pulls the next agent by priority."
- **[말]** 이 큰 그림이 요구하는 네 가지: BFS는 **프런티어**가, TEG·layout은 **그래프**가,
  reservation은 **점유 저장·질의**가, PP는 **우선순위 추출**이 필요하다. *왜 필요한가*는
  여기까지 — 다음 절이 *무슨 자료구조로, 왜 그것인지*.
- **[자료]** 구성요소→필요 매핑 그림 — AI.

---

# 5. Data structures (class topics)  [자료구조 축]  ★

> 관통 메시지: 범용 자료구조는 자기가 담는 타입을 모르고 정렬·우선순위를
> **`Data::operator<` 하나에 위임**한다. 도메인 규칙은 그 위 계층이 조립.
> 4.4의 "왜 필요"를 받아, 각 구성요소를 *무슨 자료구조로, 왜 그것*으로.

### 5.1 Component → data structure
- **[화면]** 4.4의 구성요소를 자료구조에 매핑:
  - **BFS frontier** → **queue** (FIFO)
  - **TEG & layout** → **graph** (adjacency list, on dynamic_array)
  - **reservation** → **dynamic_array (per-node) + AVL tree (intervals)**
  - **priority 추출** → **min-heap**
  글(≤18): "frontier → queue · graph → adjacency list · reservation → array + AVL ·
  priority → min-heap."
- **[말]** 4.4에서 *왜 필요*했던 것들을 자료구조로 받는다. BFS 프런티어=queue, TEG·layout=
  graph(인접 리스트, dynamic_array 위), reservation=노드별 dynamic_array + 구간 AVL tree,
  우선순위 추출=min-heap. 이 중 **reservation의 AVL과 min-heap**이 강조 둘 — 깊게 본다.
- **[자료]** 구성요소→자료구조 매핑 그림 — AI.

### 5.2 Dependency map — built from class structures
- **[화면]** **자료구조 의존성 그림**(Data → 빌딩블록[avl_tree_node·linked_list_node·
  dynamic_array] → 자료구조[avl_tree·min_heap·graph·linked_list·queue]).
  글(≤14): "All from class structures. Two tiers: building blocks → structures."
- **[말]** 다 직접 구현·조합. 아래(담는 Data)→위(쓰는 구조). queue는 linked_list 뼈대,
  graph·min-heap은 dynamic_array 뼈대, AVL은 avl_tree_node. **stack은 의도적 제외**
  (pred 역순이면 충분). 다음: 이 구조들이 담는 Data 계약, 그리고 강조 셋(graph·AVL·heap).
  (queue·linked_list·dynamic_array는 여기서 한 호흡 — 별도 강조는 셋만.)
- **[자료]** 의존성 그래프 — DONE.

### 5.3a The Data contract (base)
- **[화면]** **`data.h` 코드 캡처**(19줄). 글(≤16): "Every structure holds `Data*`.
  `virtual ~Data` → polymorphic delete.  `virtual operator<` → comparison delegated."
- **[말]** 모든 자료구조는 구체 타입 대신 **`Data*`**를 담는다 → 컨테이너를 타입마다
  중복 안 함. 단 두 가지가 이걸 안전·범용으로 만든다: **virtual 소멸자**(위 컨테이너가
  `delete`해도 자식 소멸자가 돌아 누수 없음 — 소유는 위, 정리는 자식), **virtual
  operator<**(비교를 자식이 *override*해 위임 — 자료구조는 규칙을 모름). ASan 무누수의 뿌리.
- **[자료]** **YOU 코드 캡처**: `data.h` 전체.

### 5.3b The concrete children — operator< overloading
- **[화면]** **코드 캡처**: `Interval : public Data`(operator< by start, touches/merge) +
  `Agent : public Data`(operator< by priority) 선언부 나란히. 글(≤16): "Interval & Agent
  override `operator<` — that's what makes a generic structure a domain structure."
- **[말]** Data를 *public 상속*한 구체 자식들이 **operator< overloading**의 실체.
  **Interval**(start 기준)이 범용 AVL을 '구간 트리'로, **Agent**(priority 기준)가 범용
  min-heap을 '우선순위 큐'로 만든다 — 자료구조는 그대로, operator<만 바뀌어 도메인 구조가
  됨. (Interval은 reservation table 내부에서, Agent는 PP의 heap에서 쓰임 — 뒤 강조에서.)
- **[자료]** **YOU 코드 캡처**: `Interval`·`Agent` 클래스 선언부(operator< 보이게).

### 5.4a ★ Emphasis (graph) — adjacency list (figure)
- **[화면]** **graph 그림**: 노드 + 인접 리스트(노드별 이웃 배열, dynamic_array 위).
  글(≤16): "layout & TEG as adjacency lists — sparse, O(V+E), fast neighbor scan."
- **[말]** 4.4의 "TEG·layout=graph" 요구. **인접 리스트**(노드마다 이웃을 dynamic_array에)
  로 구현 — fab 그래프는 희소(각 노드 in/out ≤ 2)라 인접 행렬(O(V²)) 대신 리스트가 맞다.
  O(V+E) 공간, 이웃 순회 효율. layout 원본과 TEG 둘 다 같은 Graph로 표현.
- **[자료]** graph(인접 리스트) 그림 — AI.

### 5.4b ★ Emphasis (graph) — code
- **[화면]** **코드 캡처**: `Graph`의 `addEdge` / `neighbor` / 인접 저장부.
  글(≤12): "adjacency stored on dynamic_array · neighbor(u, i) in O(1)."
- **[말]** Graph는 노드별 이웃을 dynamic_array로 들고, `addEdge`로 추가·`neighbor`로 조회.
  TEG도 같은 Graph 타입 — (node,t) 정점을 일반 노드처럼 다룬다.
- **[자료]** **YOU 코드 캡처**: `Graph::addEdge` / `neighbor` (인접 저장부).

### 5.5a ★ Emphasis A — Reservation Table (figure)
- **[화면]** 좌: **2층 구조 그림**(바깥 DynamicArray[node]→안쪽 AvlTree⟨Interval⟩).
  우: **합치기 애니메이션**(`[2,4)` 트리에 `[4,7)` 삽입 → predecessor `[2,4)` → touches →
  merge `[2,7)` → remove+insert). 글(≤18): "Per-node occupancy as sorted, coalesced
  intervals. `operator<` turns generic AVL into an interval tree."
- **[말]** reservation table은 2층 — 바깥 dynamic array(노드 인덱스), 안쪽 노드마다
  AVL tree of Interval. **(1) operator< overloading**(Interval, start 기준)이 범용 AVL을
  '구간 트리'로 만드는 다리 — insert/find/predecessor/successor가 좌우를 가르는 근거 전부.
  **(2) 합치기(coalescing)**: AVL은 touches/merge를 *모른다*. `reserve()`가 Interval의
  touches(닿나)·merge([min,max))를 AVL predecessor/successor(이웃)+remove/insert와 엮어,
  닿는 좌·우 이웃을 흡수해 하나로(한 번에 셋+ 가능). 트리엔 늘 분리·정렬 구간만 → isFree
  정확·빠름. AVL이 정렬배열(삽입 O(n))·해시(순서질의 불가)보다 맞는 이유.
  **누가 무엇을**: *기록*은 PP(`recordPath`→`reserve`가 Interval 생성), *읽기*는
  BFS+TEG(`isFree` point query로 점유 (node,t) 건너뜀). Interval·AVL은 reservation table의
  **내부 구현** — BFS+TEG도 PP도 그 내부를 모른 채 `reserve`/`isFree` 인터페이스로만 쓴다
  (캡슐화). reservation table이 모든 에이전트의 충돌 회피를 잇는 공유 핵심.
- **[자료]** 2층 그림 + 합치기 애니메이션 — DONE. (코드 캡처는 5.5c 별도 슬라이드)

### 5.5b ★ Emphasis A — why a balanced tree (not array / hash)
- **[화면]** **대안 비교 표**(정렬배열·해시·비균형BST·AVL × 삽입·순서질의·판정).
  글(≤16): "Workload: insert occupancy + find first interval ≥ t — constantly. AVL wins both."
- **[말]** 작업 부하 = 점유 삽입 + "t 이상 첫 구간 찾기"를 *끊임없이*. 정렬배열은 삽입
  O(n), 해시는 순서질의 불가, 비균형 BST는 최악 O(n). **AVL만 삽입·이웃질의 둘 다
  O(log n) 보장** — 합치기·isFree가 요구하는 게 정확히 그것.
- **[자료]** Why-AVL 비교 표 — DONE.

### 5.5c ★ Emphasis A — coalescing code (touches · merge)
- **[화면]** **코드 캡처**: `Interval::touches` / `merge` + `reserve`의 흡수 루프(일부).
  글(≤12): "`touches`/`merge` = the coalescing geometry · `reserve` orchestrates."
- **[말]** operator<는 5.3b에서 봤다(정렬 다리). 여기선 **합치기 기하** — **touches**(닿나)·
  **merge**([min,max))와, 이를 AVL predecessor/successor로 엮어 흡수하는 **reserve**.
  AVL은 touches/merge를 모르고, reserve가 조립. (그림 5.5a의 코드 확인.)
- **[자료]** **YOU 코드 캡처**: `Interval::touches`/`merge` · `reserve` 흡수 루프.

> **주의(슬라이드 분할)**: 그림과 코드 캡처를 한 슬라이드에 욱여넣지 않는다. 강조는
> a·b·c 하위 슬라이드로 — (그림) → (왜/코드)를 *넘기며* 설명. 한 장에 동시 금지.
> 5.4(graph)=a 그림·b 코드 / 5.5(AVL)=a 그림·b why·c 코드 / 5.6(min-heap)=a 그림·b 코드.

### 5.6a ★ Emphasis B — min-heap (figure)
- **[화면]** **min-heap 그림**(1-based 완전이진트리 + 배열, extract-min). 글(≤16): "PP
  pulls the next agent by priority — repeated extract-min, O(log n)."
- **[말]** 4.4의 "우선순위 순 추출" 요구가 여기로. PP는 우선순위 순으로 에이전트를
  하나씩 → **반복 extract-min**. **왜 우선순위 큐 중 min-heap인가**: 정렬배열은 삽입마다
  O(n), 비정렬 배열은 추출마다 O(n) — min-heap은 삽입·추출 둘 다 **O(log n)** 보장.
- **[자료]** min-heap 그림 — DONE.

### 5.6b ★ Emphasis B — encapsulation code (private inheritance)
- **[화면]** **코드 캡처**: `MinHeap : private DynamicArray` 선언부 + push/pop 시그니처.
  글(≤14): "private inheritance hides get/set/pushBack — invariant can't be broken."
- **[말]** Agent::operator<는 5.3b에서 봤다(우선순위 다리). 여기선 **캡슐화** — min-heap이
  **DynamicArray를 private 상속**해 배열의 get/set/pushBack을 숨긴다 → 호출자가 힙
  불변식을 못 깸. (Data는 public 상속=다형성, 자료구조 간은 private 상속=은닉 — 대비.)
- **[자료]** **YOU 코드 캡처**: `MinHeap : private DynamicArray` 선언부.

---

# 6. Results  ★  [results]

### 6.1 Collision-free at fab scale
- **[화면]** **실행 스크린샷** + 작은 표(node·agents·H·goals·throughput·ms/round).
  글(≤20): "Real fab, 21,178 nodes · collision-free (all-pairs check) · leak-free
  (ASan/UBSan) · cost bound = TEG's V×(H+1)."
- **[말]** 실 fab에서 prioritized planner 완주. 모든 committed path 검증 — 인접/대기,
  충돌 0(직접 전수검사). ASan/UBSan 무누수. 확장성을 가르는 건 탐색 로직이 아니라
  **자료배치** — TEG의 V×(H+1) 정점이 H의 상한.
- **[자료]** 스크린샷 — YOU; 표 — AI.

---

# 7. Conclusion

### 7.1 Takeaway
- **[화면]** 핵심 그림(B-1 또는 reservation 2층) 1개 재활용.
  글(≤15): "Correctness rests on the reservation invariant; scale is bounded by data
  layout — not the search."
- **[말]** 실 fab MAPF를 BFS+TEG로 구현·검증. 정확성의 뿌리는 reservation table의 coalescing
  불변식, 확장성의 한계는 TEG의 V×(H+1) 자료배치. 자료구조 선택·배치가 동작 여부와
  확장성을 가른다 — 수업의 핵심 교훈.
- **[자료]** B-1 또는 reservation 2층 재활용.

---

## 흐름 점검
- **top-down 흐름**: 1 intro → 2 문제설정(2.3 충돌·왜 시간) → 3 데이터(획득→양자화 필요
  →전처리 코드→옵션 표) → **4 큰 그림(PP·env·pathfinder + BFS·TEG, 그리고 *왜 필요*)** →
  5 자료구조(*무슨 구조·왜 그것*) → 6 결과 → 7 결론.
- **4절 = 왜 필요 / 5절 = 무슨 구조·왜 그것** (역할 분담 확정).
  4.1 PP → 4.2 세 역할 → 4.3 BFS+TEG(pathfinder 실체) → 4.4 구성요소가 왜 필요(BFS·TEG·
  reservation·priority추출).
  5.1 구성요소→자료구조 매핑 → 5.2 의존성맵 → **5.3 Data 계약(a 부모 data.h · b 구체 자식
  Interval·Agent operator< overloading)** → **강조 셋**: **5.4 graph ★(a 그림·b 코드) →
  5.5 AVL ★(a 그림·b why·c 합치기 코드) → 5.6 min-heap ★(a 그림·b 캡슐화 코드)**.
- **operator< overloading은 5.3b(Data 자리)에서 한 번에** — Interval(→AVL 구간트리)·
  Agent(→우선순위 힙) 나란히. 이후 강조 절의 코드는 *도메인 특화*(touches/merge · 캡슐화)에 집중.
- **슬라이드 분할 원칙**: 그림+코드를 한 장에 금지. 강조는 a·b·c로 넘기며.
- 공통 메시지: **operator< overloading이 범용 자료구조를 도메인 구조로 바꾼다**(5.3b) +
  **virtual dtor(public 상속=다형성)·private 상속(=은닉)으로 안전·캡슐화**. graph는 희소→인접 리스트.
- 코드 캡처(YOU): preprocessing 2함수(3절) · `data.h`(5.3a) · `Interval`·`Agent` 선언부(5.3b) ·
  `Graph::addEdge`/`neighbor`(5.4b) · `touches`/`merge`+`reserve`(5.5c) · `MinHeap:private DynamicArray`(5.6b).
- 슬라이드 수 연연 안 함. 각 장 이미지≤3·글≤50단어 준수.
