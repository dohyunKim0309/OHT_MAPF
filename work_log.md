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
- 〔메타〕 `L5_implementation/CLAUDE.md` 부재 — [[claude_md_index]]가 임베드로 참조하나
  실재하지 않음. (2026-06-06 발견)
- 〔메타〕 빌딩 블록 명명 모순 — [[data_structure_dependency_map]]은 `tree_node`,
  [[folder_structure_and_responsibilities]]는 `avl_tree_node`. 같은 부품을 두 이름으로
  부른다. 정보 모순. (2026-06-06 발견)
- 〔도메인〕 AVL 중복 키 삽입 정책 미정 — 같은 키일 때 거부/덮어쓰기/중복허용 중
  무엇인지가 사용처 [[reservation_table]]의 Interval 키 설계에 종속. [[L4_functions/avl_tree]]는
  잠정으로 오른쪽 삽입. 정보 부족. (2026-06-06 발견)

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
  → 문서 옆 `assets/`에 co-locate. AVL png 5개를 `L3_modules/data_structure/assets/`로
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
  L4 링크는 경로 명시(`[[L4_functions/avl_tree]]`)하는 규칙도 추가.
- **결정 〔도메인〕** [[L4_functions/avl_tree]] 작성 — insert(공개+재귀)·rotateLeft/Right·
  rebalance·height/updateHeight/balanceFactor·find 의사코드. 높이 규약 leaf=1/NULL=0 반영.
- **결정 〔메타〕** L4 문서 내부 조직을 1) Fields 2) Methods, 각각 public/private 하위로
  통일(가독성). [[L4_functions/avl_tree]] 재구성, folder_structure L4 책임에 반영.
