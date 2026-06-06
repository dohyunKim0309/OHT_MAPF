---
tags: [work_log]
---

# Work Log

막힘·발견·결정의 시간순 기록. [[L2]]의 "막힘"과 "통찰의 상향 기록"을 실제로 담는
매체다. 한 가지에서 얻은 통찰이 인간 머릿속에만 남아 사라지거나, 다른 가지 작업의
LLM 컨텍스트에 들어오지 못하는 것을 막는다.

## 기록 규칙

- **시간순**으로 append 한다. 최신 항목이 아래로.
- 각 항목은 신호를 분리 표기한다:
  - 〔도메인〕 — MAPF·자료구조·알고리즘 등 *문제 자체*에 관한 신호
  - 〔메타〕 — 방법론·폴더 구조·문서 체계 등 *작업 방식 자체*에 관한 신호
- 종류는 **막힘 / 발견 / 결정**. 막힘은 원인(정보 부족 / 정보 모순)과 갱신 위치를
  함께 적는다.
- 미해결 막힘·발견은 아래 [[#미해결]]에 동기화한다. 해소되면 옮기지 말고 줄을 지우되,
  해소 사실은 그 날짜의 기록에 남긴다.

## 미해결

- 〔메타〕 `L0_methodology/`의 `L1.md`/`L2.md` 명명이 ① L2 명명 규칙
  (`_<숫자>_<이름>.md`)과 ② 프로젝트 층 이름(`L1_purpose` 등)과 토큰 충돌. `[[L1]]`의
  지시 대상이 모호. (2026-06-06 발견)
- 〔메타〕 빌딩 블록 명명 모순 — [[data_structure_dependency_map]]은 `tree_node`,
  [[folder_structure_and_responsibilities]]는 `avl_tree_node`. 같은 부품을 두 이름으로
  부른다. 정보 모순. (2026-06-06 발견)
- 〔메타〕 계약↔기전 축 적용 후속 — L3 계약 문서에 남은 *상세 기전*을 L4로 재단해야.
  [[avl_tree]]의 회전 단계 설명(ASCII·이미지)이 대표 사례. (2026-06-06 발견)
- 〔도메인〕 graph unweighted 통일 후속 정합 — graph가 weight 없이 통일되며 ϕ-BF도
  weighted 원본이 아닌 **unweighted 확장본**을 쓰게 됨. 그러나 [[data_structure_design]]
  (graph "두 형태 보유", ϕ-BF "weighted 원본", 사용처 표·비교 축), [[L1_purpose/README]],
  [[work_log]] 이전 기록은 아직 "ϕ-BF=weighted / BFS+TEG=확장본"의 옛 서술. 새 모델은
  *둘 다 같은 확장본 공유*, 차이는 "TEG 시간확장(BFS) vs 확장본 위 ϕ 시간회피(ϕ-BF)".
  정보 모순 — 위 문서들 일괄 갱신 필요. (2026-06-06 발견, 갱신 보류)

## 기록

### 2026-06-06

- **발견 〔메타〕** 인덱스가 가리키는 노드 부재. `work_log.md`와
  `L5_implementation/CLAUDE.md`가 상위 문서들에서 `[[work_log]]`·임베드로 참조되나
  파일이 없음. 하향 링크 끊김 = "정보 부족" 막힘. 갱신 위치: 부재 노드 생성.
- **결정 〔메타〕** `work_log.md` 신설(이 문서). 이제부터 막힘·발견·결정을 여기 기록한다.
  이로써 위 발견의 `work_log` 부분은 해소. `L5_implementation/CLAUDE.md` 부분은
  [[#미해결]]에 잔류.
- **발견 〔메타〕** `L0_methodology/`의 `L1.md`/`L2.md` 명명이 L2 명명 규칙 및 프로젝트
  층 이름과 충돌(위 [[#미해결]] 참조). → 보류, 인간 판단 대기.
- **발견 〔도메인〕** `quene`는 `queue`의 오타. [[data]] 내 링크는 작업 중 이미 정정됨.
  빈 스텁 파일 `quene.md`(0바이트)만 잔존.
- **결정 〔도메인〕** `quene.md` → `queue.md` 리네임. 도메인 명명 일관성 회복.
- **발견 〔메타〕** 구조가 *문서에 임베드되는 도식 이미지*의 자리를 정의하지 않음
  (AVL 회전 png 5개가 vault 루트에 방치 — Obsidian 기본 첨부 폴더가 루트라서).
  정보 부족 막힘. 갱신 위치: L2.
- **결정 〔메타〕** 도식 이미지 = 그 문서가 속한 층의 콘텐츠로 규정(외부 자원 아님).
  → 문서 옆 `assets/`에 co-locate. AVL png 5개를 `L3_interface/data_structure/assets/`로
  이동. Obsidian `attachmentFolderPath`를 `./assets`로 설정해 환경으로 강제.
  [[folder_structure_and_responsibilities]]에 "문서 자산" 규칙 추가.
- **결정 〔도메인〕** [[avl_tree]] 문서 작성 시작 — 책임·선택 근거(정렬 삽입 시 BST가
  O(N)으로 퇴화 → balance condition)·구현 방식(삽입 시 단일/이중 회전, 4→2 case 축약)
  채움. 외부 제공 기능은 미정(L4). 빌딩 블록은 최신 명명 `avl_tree_node` 사용(위
  [[#미해결]] 모순 참조).
- **막힘 〔도메인〕** Double Rotation이 왜 필요한지(왜 Single로는 안 되는지) 이해 공백.
  → 대화로 해소: 무거운 덩어리가 "안쪽"(left-right)이면 Single은 불균형을 반대쪽으로
  옮길 뿐. [[avl_tree]] Case 2 절을 ASCII 단계도 + 직관(`k2`가 루트로 올라감, in-order
  불변)으로 강화.
- **결정 〔도메인〕** [[avl_tree]]를 형제 L3 문서(dynamic_array·linked_list) 양식에 맞춰
  보강 — 구현 방식에 public/private field·methods(이름+역할, 잠정)·소유권·Time Complexity
  추가. 별도 "외부 제공 기능" 절은 methods로 흡수. 구체 알고리즘은 L4로 미룸.
- **발견 〔도메인〕** L4 직전, [[avl_tree]]와 [[avl_tree_node]]의 높이 규약 불일치 포착
  (leaf=0/−1 vs leaf=1/0). [[#미해결]]에 등록, 인간 판단 대기.
- **결정 〔도메인〕** 높이 규약을 **leaf=1 / NULL=0**으로 통일(인간 결정). [[avl_tree]]의
  정의·`height()` 노트 수정, [[#미해결]]에서 해당 항목 제거. [[avl_tree_node]]는 이미
  일치(init height=1).
- **결정 〔메타〕** L4 문서 양식 확정(folder_structure 미결 해소) — **모듈별 1문서**,
  함수당 1절(시그니처·사전/사후·소유권·의사코드). L4 모듈 문서는 L3와 basename이 겹쳐
  L4 링크는 경로 명시(`[[L4_mechanism/avl_tree]]`)하는 규칙도 추가.
- **결정 〔도메인〕** [[L4_mechanism/avl_tree]] 작성 — insert(공개+재귀)·rotateLeft/Right·
  rebalance·height/updateHeight/balanceFactor·find 의사코드. 높이 규약 leaf=1/NULL=0 반영.
- **결정 〔메타〕** L4 문서 내부 조직을 1) Fields 2) Methods, 각각 public/private 하위로
  통일(가독성). [[L4_mechanism/avl_tree]] 재구성, folder_structure L4 책임에 반영.
- **결정 〔메타〕** L3/L4 축을 **계약 ↔ 기전**으로 재정의(인간 결정). `module`/`function`은
  코드 단위 단어라 추상화 축과 어긋났던 것 → L3=각 구성요소의 계약(표면), L4=각 연산의
  기전(내부). 폴더 `L3_modules→L3_interface`·`L4_functions→L4_mechanism` rename, 전 참조
  경로·링크·태그 일괄 치환, folder_structure·CLAUDE.md·README의 레이어 정의를 새 어휘로 갱신.
- **결정 〔도메인〕** `L5_implementation/data_structure/avl_tree.{h,cpp}` 작성 —
  [[L4_mechanism/avl_tree]] 명세대로 `AvlTree` 클래스(스텁의 소문자 `avl_tree`는 node.h의
  `friend class AvlTree`와 불일치라 바로잡음). insert/find + 회전 기계, 높이 규약
  leaf=1/NULL=0. 검증: `g++ -Wall -Wextra` 무경고 + 기능 테스트 통과(N=1000 정렬 삽입 +
  LL/RR/LR/RL + 중복 거부 스모크).
- **기록 〔도메인〕** reservation_table 구조 확정(인간) — 그래프 정점 수만큼 길이의 dynamic
  array, 각 칸에 `AvlTree*`(정점별 시간 구간 Interval 저장). [[avl_tree]] 사용처 구체화.
  reservation_table L3 문서 생기면 이관.
- **결정 〔도메인〕** AVL 중복 키 정책 = **거부**(같은 키는 애초에 삽입 안 함, 인간 결정).
  [[#미해결]]에서 항목 제거. [[L4_mechanism/avl_tree]] insert 의사코드·사후조건 및 L5
  `insert` else 분기를 no-op(`return node`)로 갱신.
- **결정 〔메타〕** L5 코드 주석은 **영어**로(인간). [[avl_tree]] 코드 주석을 영어로 전환.
  컨벤션은 `L5_implementation/CLAUDE.md`(부재, [[#미해결]]) 생기면 명문화. 기존 한글 주석
  파일(data/node/linked_list 등)은 추후 일괄 전환 여지.
- **결정 〔메타〕** `L5_implementation/CLAUDE.md` 신설 — "코드 주석은 영어로" 명문화 +
  관측된 코딩 컨벤션(명명·헤더가드·비소유·assert·빌드) 정리. [[#미해결]]의 부재 항목 해소,
  [[claude_md_index]]의 임베드가 이제 해석된다.
- **기록 〔도메인〕** reservation_table 방향(인간) — 점유 에이전트 정보는 MAPF 디버깅에
  유용하나 과제 버전에선 무시하고 키를 유일 취급. 한 정점 내 **시간 구간 병합**(인접/중첩
  Interval 머지)을 진행 예정. reservation_table 설계 시 구체화.
- **발견 〔도메인〕** 실제 데이터 확보 — 삼성전자 협업 연구(PLOS One, jja8989/visualization
  -for-OHT-network repo)의 **SMAT 2022 OHT 네트워크**. 노드 2,858 / 엣지 3,424, 순수
  단방향(self-loop·역방향쌍·중복 0 검증). L1의 단방향 가정을 실데이터가 뒷받침. directed라
  edge swap 충돌 원천 부재 → 노드 점유 충돌만 처리. `data/` + 전처리 스크립트·문서.
- **결정 〔도메인〕** 모델 정립(인간) — **V = 에이전트 속도**(한 스텝당 거리, 양자화 상수가
  아님). `weight = round(거리/V)` = 간선 통과 타임스텝 수. 에이전트는 매 스텝 V로 이동 또는
  0(대기). **충돌 제약 = 최소거리 V·Δt**. 간선을 가상노드로 펼치면(unweighted 확장) 이
  거리 제약이 "(node,time) 점유 충돌"로 통일됨 — 확장본을 만든 근본 이유.
- **결정 〔도메인〕** V=500, H(horizon)=28 채택(인간). maxW=28=H → 전 간선이 horizon 안에
  통과. 확장본 21,178노드. **오차는 두 종류**: 간선별 7.1%(최대 20%)이나, 경로는 간선합이라
  올림/내림 상쇄돼 **경로 오차 3.6%**(랜덤 200쌍 측정). MAPF는 경로시간을 쓰므로 경로오차가
  진짜 척도. 1순위 "전체 그래프 계산 가능", 오차는 수용. data_preprocessor에 2단계(확장)
  추가, 기본 V=500, edges_expanded.txt 출력. PREPROCESSING.md 갱신.
- **발견/막힘 〔도메인〕** TEG 규모 — 명시적 TEG = 확장본 × T. V=50 확장본 22만, V=500은
  2.1만으로 완화되나 T를 곱하면 여전히 큼. **그러나 horizon H=28 제한 시** BFS는 시작점
  H-hop 반경(수천 정점)만 방문 → 한 에이전트 탐색은 가벼움(확장본 전체크기 무관). 반면
  ϕ-BF는 확장본 없이 원본 V·E(~980만)만 → 더 쌈. **"실 fab 규모에서 명시적 TEG는
  ϕ-BF보다 무겁다"가 L1 비교 가설을 정량 입증** — 보고서 핵심 결과 후보. 갱신 위치:
  planner 가지(TEG 물질화 전략), L1 비교 서술.
- **결정 〔도메인〕** Interval 소유권(인간) = **비소유 + clearAndDelete 위임**. avl_tree
  기본 소멸자는 비소유(노드만), `clearAndDelete()`는 호출자(reservation_table)가 Data*를
  소유했음을 확인하고 부르는 명시적 옵트인 — 소유 결정은 호출자, delete 실행은 트리 대행.
  avl_tree.{h,cpp}에 clearAndDelete/destroyAndDelete 추가(ASan: 정리·재사용 무누수).
  avl_tree.md·design#소유권에 "비소유 + 옵트인 일괄정리" 명문화. reservation_table.md 갱신.
- **막힘→결정 〔도메인〕** BFS+TEG 의사코드를 처음에 *암묵적 TEG*(visited 배열로
  (node,time) 계산만)로 썼다가, 인간이 "그러면 TEG를 명시적으로 구현하는 게 아니다"라고
  지적 → 정정. 암묵적은 ① 원래 "명시적 TEG" 결정 위반 ② TimeExpandedGraph 클래스 미사용
  ③ "TEG=시간 복제(V·T 공간)" 비교 가설을 무너뜨림. 메모리 점검(V=500,H=28: TEG 정점
  ~61만, ~15MB, M4 1초 미만)으로 명시적이 충분히 가능 확인 → **명시적 TEG로 확정**.
  L4 bfs_teg.md를 buildTEG(정점·간선 생성, 점유 정점 간선 미생성으로 흡수) + 일반 BFS로
  재작성. 정보 모순(설계↔의사코드)을 의사코드 수정으로 해소.
- **발견 〔도메인〕** H=28 도달률 0.3% — V/H를 "maxW≤H(간선 하나)"로 맞췄으나 *경로는
  간선 수백 개*. 확장본 원본 노드 간 홉 거리 중앙값 ~405, 최대 1406. H는 경로 전체 홉
  수만큼 커야 도달. 잘못된 기준(간선 단위)을 바로잡음.
- **결정 〔도메인〕** TEG 1회 빌드 재사용(인간 통찰). PP의 모든 에이전트가 같은
  layout·H를 공유 → TEG 뼈대 동일, reservation만 다름. 그래서 ① TEG는 reservation
  무관(layout만)하게 BfsTeg 멤버로 **1회 빌드** ② reservation은 **탐색 시** BFS가 isFree로
  점유 정점 스킵해 흡수. 빌드 N회→1회. 소유: findPath 지역 RAII → BfsTeg 멤버. Planner
  인터페이스는 공통(graph,reservation,H) 유지(ϕ-BF 호환), TEG는 BfsTeg 내부 사정.
  실측(V=500,H=450): 빌드 ~300ms(1회) + 재사용 쿼리 ~85ms, found ~5/10(강연결 아님).
  "TEG=V·T 공간" 비교 가설은 유지(공간은 그대로, 시간만 N→1). bfs_teg.h/cpp·L3·L4 갱신.
- **결정 〔메타〕** Planner 전략 패턴 확정 — `Planner`(추상, 가상 findPath) ←
  BfsTeg/BellmanPhi 상속, PP가 Planner* 주입받아 호출. Path = 길이 H+1 노드 배열 RAII
  구조체(이동 시맨틱). graph에 originalCount/originalSize/isOriginal 추가(원본/가상=인덱스
  threshold, 플래그 없이). TEG 정점 = 정수 인코딩 node*(H+1)+t. 전부 ASan 무누수 검증.
- **발견 〔도메인〕** H=28 도달률 0.3% 재확인 — V/H를 "간선 하나(maxW≤H)"로 맞췄으나
  경로는 수백 홉(확장본 원본 노드 간 중앙값 405). H를 경로 홉 수만큼 키워야 도달.
  H=450 실측: 빌드 ~300ms(1회) + 쿼리 found 12~100ms / empty 100~200ms.
- **발견 〔도메인〕** empty(도달 실패) 쿼리가 found보다 느림 — found는 목표서 break(방문
  적음), empty는 horizon 내 도달가능 정점 전부 방문 후 실패. 세대 카운터로 *초기화*는
  없앴으나(found 12ms로 입증) *방문량*은 BFS 본질. 근본 원인 = 강연결 아님(found ~5/10,
  도달 불가 목표). → environment에서 도달 가능 목표 보장으로 완화 예정.
- **결정 〔도메인〕** 세대 카운터 최적화(인간 동의) — visitedGen/pred/q를 BfsTeg 멤버로
  1회 할당, 매 findPath `gen++`로 방문표시 O(1) 무효화(955만 칸 초기화 제거). "한 탐색에
  TEG 정점은 한 번씩 방문(물리 노드는 여러 time=여러 정점)"이라 세대 판정 안전. 정확성
  유지·ASan 무누수. L3·L4 갱신.
- **결정 〔도메인〕** **Receding Horizon 모델 확정**(인간) — H=계획 지평(멀리 봄, ~100),
  C=commit 실행 스텝(앞 C만 실행 후 재계획). H 커서 장기 계획, C로 자주 갱신해 동적
  목표에 적응. 도달 후 긴 정지 낭비(H 큰 1회 계획의 문제) 해소. PP는 매 라운드 reservation
  clear→우선순위 순 H스텝 계획→C스텝 실행. 목표: 도달 시 랜덤 새 목표(L1 명세). N=20~50.
  environment·planner·reservation README 전면 갱신. (TEG=V·T 비교 가설은 H=100에서도
  유지 — ϕ-BF는 원본 2858 노드만.)
- **결정 〔도메인〕** `L5_implementation/environment/`(Agent·Environment)와
  `L5_implementation/planner/prioritized_planning.{h,cpp}` 작성 —
  [[L4_mechanism/environment]]·[[L4_mechanism/prioritized_planning]] 의사코드의
  충실한 번역. Agent는 [[Interval]] 양식대로 *Data 묶음*(private 필드+getter,
  가변 current/goal만 setter, operator<=priority). PP는 reservation.clear→min_heap
  우선순위 순 findPath→recordPath([t,t+1) 점유). Environment는 receding step
  (plan→C스텝 전진→도달 카운트++·랜덤 새 목표→now+=C). 검증: `g++ -Wall -Wextra`
  무경고 + ASan/UBSan 무누수, ring+chord 강연결 layout에서 50라운드 238도달/300스텝.
  CMakeLists에 planner/environment 소스 일괄 추가. main.cpp를 데모로 교체(합치기).
- **막힘→결정 〔도메인〕** PP 계약 "out: Path n칸 배열을 받아 채움" ↔ 구현 "Path가
  비복사·**기본생성 불가**(길이 생성자뿐)"의 정보 모순. environment가 `new Path[n]`을
  못 해 처음엔 placement-new로 우회(코드 복잡). 인간과 대화로 해소 — Path 표현은
  "Graph가 정수 예외(명시적 노드 클래스 없음)이므로 Path도 int[] RAII 유지"가 일관(인간
  결정). 모순은 **Path 기본 생성자(빈 Path) 추가**로 해소 → environment는 `new Path[n]`로
  단순화. planner.{h,cpp} 갱신.
- **발견 〔메타〕** L4 환경/플래너 명세가 [미결]로 비워둔 *의미 결정*을 코드 작성 중 내가
  임의로 채웠던 것(에이전트 초기 random 배치, priority=id, 빈 Path=제자리). L2 "추측
  금지"상 막힘으로 보고했어야 함. 인간 결정으로 추인: ① random 시작·목표 = 수용
  (seed로 재현성) ② priority=id 잠정(생성자 인자로 외부 변경 가능한 상태라 그대로 진행,
  동적 우선순위는 추후) ③ 빈 Path=제자리(continue)는 L4 의사코드 본문과 일치. 해당 L4
  [미결] 항목들은 잠정 확정.
- **발견 〔메타〕** 빌드 검증 중 만든 임시 테스트 `smoke_env.cpp`가 vault 루트에 잔류
  (샌드박스가 마운트 폴더 삭제를 막아 자동 제거 실패). 인간이 `rm smoke_env.cpp`로
  정리 필요. 구조상 *L5 임시 테스트 파일의 자리* 미정의 — [[#미해결]] 후보(추후).
- **결정 〔도메인〕** `throughput()` 반환 타입 `int`→`double`(인간). 수식
  `arrived/now`(누적 도달 / 경과 스텝)는 옳았으나 정수 나눗셈이 거의 항상 0으로 절단됨.
  소규모 함대는 스텝당 1 미만이라 실수가 맞음. 관측용 `arrivedCount()`/`elapsed()`
  게터 추가. L3 environment README·L4 environment.md 시그니처·정책 갱신. main.cpp를
  가독성 있게 재작성(설정 배너: 알고리즘·N·H·C·라운드, 정렬된 상태, throughput
  goals/step). 실측 H=24·C=6·5에이전트·50라운드: 238도달/300스텝 = 0.793 goals/step.
- **결정 〔도메인〕** main.cpp를 *실 SMAT FAB layout 로더*로 교체(인간: 실데이터로 결과
  수집). `edges_expanded.txt`(헤더 `N M` + 0-index `from to`)를 fstream으로 읽어
  Graph(originalCount=2858) 구성. 실측: 강연결 확인(원본 전 노드가 2858개 전부 도달),
  ASan/UBSan 무누수. 결과 — throughput ∝ 에이전트 수(10→0.014, 20→0.032, 40→0.062),
  라운드 비용 ∝ 에이전트(≈70ms/agent); horizon H=300→0.014·500→0.032·700→0.032(포화),
  비용은 V·(H+1)로 증가. median 목표거리 ~405홉이라 H가 그만큼 커야 도달. 보고서 핵심
  결과로 사용.
- **결정 〔메타〕** 제출 보고서 작성 — Typst(`report/report.typ`)로 작성 후 PDF 컴파일
  (인간: typ→pdf). 과제 명세(datastructure_project_2026.pdf: WHERE/HOW/WHY 각
  자료구조·validity·efficiency)와 예시 양식(results/sub1.pdf: 표지+목차+과제 5단계
  구조)을 따름. 표지(학번 202214223·김도현·전기전자공학과, 제출일 없음), 목차, 섹션 =
  Introduction & Data Acquisition → Data Storage → Function Realization → Example Case
  → Conclusion. Bellman-Ford 제외(인간). 자료구조 근거는 data_structure_design.md 기반
  (graph 희소→adj list, AVL→reservation, min_heap→우선순위, queue/linked_list→BFS,
  dynamic_array→substrate, stack 미채택). 한글 표지용 NanumGothic 폰트 다운로드해
  font_path로 컴파일(샌드박스에 CJK 폰트 없음). 9쪽 산출.
- **발견 〔메타〕** 보고서 미리보기 PNG 디렉터리(`report/_preview*`, `report/_pv*`)가
  vault에 잔류(샌드박스 삭제 불가). `smoke_env.cpp`와 함께 인간이
  `rm -rf report/_preview* report/_pv* smoke_env.cpp`로 정리 필요.
- **결정 〔메타/도메인〕** 보고서 보강(인간) — ① 데이터 출처 논문 인용 추가: Choi et al.,
  PLOS ONE 19(7):e0307059 (2024), "Visualization system to identify structurally
  vulnerable links in OHT railway network…". 그 논문 abstract가 본 프로젝트 동기와
  일치(OHT 경로를 Dijkstra SSSP로 정하나 수백 대 동시 운행 시 congestion/deadlock).
  ② **Problem formulation 절 신설**(핵심): directed graph G=(V,E), 정수 edge weight
  w=max(1,round(d/V)), 간선을 가상노드 w−1개로 펼쳐 "엣지 통과 중 대기/점유" 모델링 →
  모든 간선 길이 1. conflict = 같은 (node,time) 점유만; **edge-swap은 directed라 불가**
  (역방향쌍 0 검증)이라 노드 conflict만 처리 — 이것이 구현 대상. ③ 의존성 그래프 도식
  (Data→빌딩블록→자료구조 3계층, Typst 네이티브, 영어). ④ TEG 필요성(단일 최적≠다중
  충돌회피) 서술. ⑤ 메타 발언 제거. References 절 추가. 11쪽.

### 2026-06-07

- **결정 〔도메인〕** 속도×시간 표기를 **V·Δt로 통일**(인간) — V는 양자화 상수가 아니라
  *한 스텝 이동거리(=속도 V × 이산간격 Δt)*이자 에이전트 최소 분리거리. 보고서의 수식·
  서술·표 헤더를 전부 V·Δt로 통일(혼용·"Δt=1이라 V" 문장 제거). 코드 측은 변수명 `V`
  유지하되 `data_preprocessor.py` 독스트링에 "V = per-step 이동거리 V·Δt" 의미 명시.
- **결정 〔도메인/메타〕** Path를 Data에서 **상속하지 않기로**(인간). 검토: `Data`는
  컨테이너(avl_tree·min_heap·dynamic_array)에 담겨 `operator<`로 비교되는 "한 칸"인데,
  Path는 어떤 컨테이너에도 담기지 않고(planRound가 `Path[]`를 직접 다룸) 비교도 없음 →
  Data 역할과 불일치. Agent·Interval과 달리 담길 곳이 없어 상속의 실익이 없음. "모든
  객체는 Data" 원칙의 경계 사례로, 컨테이너 원소가 아닌 값 타입은 예외로 둠. 현 int[]
  RAII 유지.
- **기록 〔메타〕** 인간이 `planner/planner.{cpp,h}` → `pathfinding.{cpp,h}` 리네임
  (Path·PathFinder가 사는 파일). CMakeLists·prioritized_planning.h include 반영됨.
- **결정 〔메타〕** L3 도메인 폴더의 `README.md`를 **폴더 이름과 같은 파일명**으로 리네임
  (인간) — `planner/README.md`→`planner.md`, `environment/README.md`→`environment.md`,
  `reservation_table/README.md`→`reservation_table.md`. 기존 본문은 이미 무자격 위키링크
  `[[planner]]`/`[[environment]]`/`[[reservation_table]]`를 쓰고 있었고, README는
  Obsidian에서 폴더마다 동명이라 링크가 모호 → 리네임으로 비로소 정확히 풀린다. 명명 규칙
  (folder_structure #명명 규칙)의 "폴더 대표 문서=README.md"를 "=<폴더이름>.md"로 갱신.
  `environment.md`는 L4와 basename 충돌하나 기존 규칙(무자격=L3, L4는 경로 명시)으로 해소.
