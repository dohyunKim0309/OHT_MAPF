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
