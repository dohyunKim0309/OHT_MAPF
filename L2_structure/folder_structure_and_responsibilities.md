---
layer: L2
tags: [structure]
---

# Folder Structure and Responsibilities
이 문서는 MAPF 프로젝트의 물리적 폴더 구조와 각 위치의 책임을 정의한다.

## 전체 구조
```
mapf/
├── CLAUDE.md
├── L0_methodology/
├── L1_purpose/
├── L2_structure/
├── L3_interface/           (Black Box)
├── L4_mechanism/           (White Box, pseudocode)
├── L5_implementation/
├── data/
├── results/
└── work_log.md
```

## 계층 구조의 원칙
L0~L5는 추상화 층에 대응한다. 위로 갈수록 압축되어 있고, 아래로 갈수록
구체적이다. L0(`L0_methodology/`)은 프로젝트 위에 있는 메타 층으로,
작업 방식 자체를 정의한다. 코드(`L5_implementation/`)도 이 계층의 일부다.
빌드 관습상 `src/`로 부르는 것이 일반적이지만, 본 프로젝트는 층의 일관성을
우선시하여 `L5_implementation/`을 채택한다.

## 기술 스택
- 언어: C++
- IDE: CLion
- 빌드: CMake
- 메모리 관리: `new`/`delete`

## 명명 규칙
- 폴더: `L<숫자>_<영문이름>` (층) 또는 짧은 영문 (외부 자원)
- 폴더 내 순서 있는 문서: `_<숫자>_<이름>.md`
- 폴더 대표 문서: **폴더 이름과 같은** `<폴더이름>.md` (예: `environment/` →
  `environment.md`). 무자격 위키링크 `[[environment]]`가 곧바로 이 대표 문서로 풀린다
  (`README.md`는 Obsidian에서 폴더마다 동명이라 링크가 모호해 쓰지 않는다).
- 문서 도식 이미지: 같은 폴더의 `assets/` 하위 (상세 [[#문서 자산]])
- L4 모듈 문서는 L3와 basename이 겹친다(둘 다 `avl_tree.md`). L4로 링크할 땐 경로를
  명시(`[[L4_mechanism/avl_tree]]`)하고, 무자격 `[[avl_tree]]`는 L3 모듈 문서를 뜻한다.

## 문서 자산
- 문서에 임베드되는 도식 이미지는 **그 문서가 속한 층의 콘텐츠**다. 도식은
  대상을 손실 압축한 추상이므로, `data/`·`results/`(주제의 외부 I/O)와 달리
  층 외부 자원이 아니다.
- 임베드하는 문서와 **같은 폴더의 `assets/`**에 둔다.
  예: `L3_interface/data_structure/avl_tree.md` → `L3_interface/data_structure/assets/`.
- 환경 보강: Obsidian `attachmentFolderPath`를 `./assets`로 설정해, 붙여넣는
  이미지가 현재 문서 옆 `assets/`에 자동 저장되게 한다. 임베드는
  basename(`![[name.png]]`)으로 하면 위치와 무관하게 해석된다.

## 각 위치의 책임
### `L0_methodology/`
- 작업 방식 자체를 정의하는 메타 층. 프로젝트 외부의 일반 방법론.
- 변경 빈도: 매우 낮음

### `L1_purpose/`
- 프로젝트의 목적, 범위, 성공 기준
- 변경 빈도: 매우 낮음

### `L2_structure/`
- 프로젝트의 조직적 결정 (폴더 구조, 명명 규칙, 작업 흐름, 모듈 간 관계)
- 본 문서가 여기 속함
- `data_structure_design.md`: 자료구조 간 의존성·사용처 조망.
  모듈 간 관계는 조직적 결정이므로 L2에 둔다. 상위 모듈까지 포함한 전체
  조망(`module_dependency_map.md`)은 이를 흡수하여 추후 작성.
- 변경 빈도: 낮음

### `L3_interface/` — 계약(표면)
- 구성요소별 1문서. **각 구성요소의 계약** — 이걸 *쓰는* 데 필요한 것(black box).
- 책임, 선택 근거, 노출 인터페이스(필드·메서드 이름+역할), 의존성, 사용처.
- 상세 기전(알고리즘·의사코드)은 L4. 여기엔 압축된 스케치까지.
- 변경 빈도: 중간

**2계층 조직** (토대 / 도메인):
- `data_structure/` — 도메인 무관 **공용 토대**. 누구나 빌려 쓰는 자료구조
  부품 상자(data, node, dynamic_array, avl_tree, min_heap, graph,
  linked_list, queue). 도메인을 모른다.
- `domain/` — 그 위의 **도메인 구성요소**. 각 모듈은 *자기 데이터(구체 Data
  자식) + 자기 알고리즘 + 빌려온 자료구조*의 묶음이다.
  - `environment/` — 시뮬레이션 세계. layout·agents·시간 진행·throughput,
    그리고 visualizer(세계를 그림)를 포함한다.
  - `planner/` — Planner = PP로 플래닝하는 것. 내부에 단일 에이전트 경로
    탐색(BFS+TEG, ϕ-BF)과 `reservation_table/`(+ Interval)을 둔다.
- 토대/도메인의 경계: 구체 Data 자식과 도메인 로직은 `domain/`에, 그것을
  담는 범용 컨테이너는 `data_structure/`에. 의존은 domain → data_structure
  한 방향뿐이다(토대는 도메인을 모른다).

### `L4_mechanism/` — 기전(내부)
- 구성요소별 1문서 (`L4_mechanism/<component>.md`). **각 연산의 기전** — 이걸 *만드는* 데
  필요한 것(white box).
- 문서를 **1) Fields, 2) Methods**로 나누고 각각 **public/private** 하위에 항목별로 둔다.
  각 항목은 시그니처·사전/사후 조건·소유권·의사코드. (확정 2026-06-06, 첫 사례 [[L4_mechanism/avl_tree]])
- L3 작업 중 깊이 파야 할 부분만 비선형적으로 채움
- 변경 빈도: 높음

### `L5_implementation/`
- 실제 C++ 코드
- 내부 구조는 [[#L5 내부 구조]] 참조
- 변경 빈도: 매우 높음

### `data/`
- fab 레이아웃, 시나리오 입력
- 층 외부 자원

### `results/`
- 실험 결과, 측정 데이터
- 층 외부 자원

### `work_log.md`
- 막힘, 발견, 결정의 시간순 기록
- 도메인 신호와 메타 신호 분리 기록

## L5 내부 구조

자료구조는 두 단으로 조직된다 — Data를 담는 **빌딩 블록**(`avl_tree_node`,
`linked_list_node`, `dynamic_array`)과 그것을 조합한 **자료구조**.
`dynamic_array`는 min_heap·graph의 뼈대이자 dist/pred 배열이고,
`linked_list`(←linked_list_node)는 queue의 뼈대다. `avl_tree`는
avl_tree_node를 쓴다. stack은 미채택(근거는 [[data_structure_design]]).
계층 전체는 [[data_structure_design]] 참조.

```
L5_implementation/
├── CLAUDE.md
├── data_structure/
│   ├── data.{h,cpp}              ← 타입 불문 객체 컨테이너 (담기는 한 칸)
│   ├── node.{h,cpp}              ← 빌딩 블록 두 노드 클래스 (AvlTreeNode, LinkedListNode)
│   ├── dynamic_array/            ← 빌딩 블록 (min_heap·graph의 부품, dist/pred 배열)
│   ├── avl_tree/                 ← avl_tree_node 사용
│   ├── min_heap/                 ← dynamic_array 사용
│   ├── graph/                    ← dynamic_array 사용
│   ├── linked_list/              ← linked_list_node 사용
│   └── queue/                    ← linked_list 사용
├── planner/
├── pp/
├── simulator/
└── main.cpp
```
각 모듈의 파일 구성과 책임, 구체 Data 자식의 위치는 [[L3_interface]]의
해당 모듈 문서를 참조.

### 분리 원칙
- `data_structure/`: 도메인과 무관한 자료구조. 모든 자료구조는 `Data`
  포인터를 컨테이너로 다룬다. 구체 Data 자식은 여기 두지 않고 사용처 도메인
  모듈에 둔다.
- `planner/`: 교체 가능한 단일 에이전트 경로 탐색. Planner 추상 클래스 하에
  BfsTeg와 BellmanPhi가 평행 배치. PP가 Planner*를 받아 사용.
- `pp/`: 상위 프레임워크. ReservationTable이 여기 종속.
- `simulator/`: 시뮬레이션 루프, throughput 측정.

## CLAUDE.md 배치
CLAUDE.md는 작업 위치에 분산 배치된다. 전체 배치와 각 파일의 역할은
[[claude_md_index]] 참조.

## 작업 흐름
1. 어느 층에서 작업할지 결정.
2. 해당 층의 문서를 시야에 두고 작업 시작.
3. 막힘 발생 시 [[work_log]]에 기록.

## 미결 사항
- `results/`의 하위 구조: 실험이 시작되면 정의.
- TEG의 위치: `graph/` 내 별도 클래스 vs planner 내부. planner 작성 시 결정.