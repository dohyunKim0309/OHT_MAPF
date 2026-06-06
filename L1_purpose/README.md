---
layer: L1
tags: [purpose]
---

# OHT MAPF 프로젝트

반도체 fab 또는 스마트 팩토리의 OHT(Overhead Hoist Transport) 시스템을
대상으로 한 단방향 가중 그래프 기반 MAPF(Multi-Agent Path Finding)
시뮬레이터. 두 가지 single-agent path planning 알고리즘을 PP(Prioritized
Planning) 프레임워크 안에서 자료구조 관점으로 비교한다.

- **베이스라인**: BFS + Time-Expanded Graph (TEG). reservation을 TEG 빌드 시
  점유 정점 제거로 흡수하고, unweighted라 BFS로 시간 제약 최단경로를 푼다.
- **비교 대상**: Parallel ϕ Bellman-Ford (그래프 복제 없이 시간 제약 처리)

---

## 토픽 선정 이유

**자료구조 관점:** MAPF는 다양한 자료구조의 자연스러운 사용처를 제공한다.
시간 제약을 가진 점유 정보(AVL Tree), 우선순위 기반 처리(Min Heap), 관계 기반
layout 표현(Graph), FIFO 전파(Queue ← Linked List) — 수업에서 다룬 여러
자료구조가 명확한 역할 분담으로 함께 사용된다. 어떤 자료구조가 *왜 이 문제에
맞지 않는가*도 분석 대상이다 — 정적 layout·고정 N 특성상 LIFO(stack)를
배제한 근거가 그 예다.

**알고리즘 비교의 의미:** 두 알고리즘은 동일한 문제를 *자료구조 활용 방식의
차이*로 푼다. BFS+TEG는 reservation을 그래프 구조에 구워넣는다 — 점유된
(node,time) 정점을 TEG에서 제거하면 충돌 회피가 그래프에 박히고, unweighted라
BFS로 풀린다(공간은 V·T). ϕ Bellman-Ford는 그래프를 복제하지 않고 같은
reservation을 interval query(ϕ)로 런타임에 회피한다. 같은 reservation table을
한쪽은 빌드 필터로, 한쪽은 런타임 질의로 쓰는 것 — 자료구조 활용이 알고리즘의
시공간 특성을 어떻게 결정하는지가 직접 드러난다.

---

## 자료구조 사용 개요

자료구조는 두 단으로 조직된다 — Data를 담는 **빌딩 블록**과 그것을 조합한
**자료구조**. 관계 전체는 [[data_structure_design]] 참조.

**Data 컨테이너 및 빌딩 블록**

| 구성 요소 | 역할 | 선택 근거 (요약) |
|---|---|---|
| Data | 타입 불문 객체 컨테이너 (담기는 한 칸) | 다형성으로 자료구조 코드를 타입마다 중복하지 않음 |
| AVL Tree Node | avl_tree의 뼈대 부품 | left/right/height, 균형 트리 구성 |
| Linked List Node | linked_list의 뼈대 부품 | Data*+next, 단일 연결 |
| Dynamic Array | min_heap·graph의 뼈대 + 노드별 dist/predecessor 배열 | 연속 저장, 임의 접근 효율 |

**자료구조**

| 자료구조 | 빌딩 블록 | 사용처 | 선택 근거 (요약) |
|---|---|---|---|
| Graph (인접 리스트) | Dynamic Array | Layout 표현 (weighted 원본 + unweighted 확장본) | 희소 그래프 공간 효율, 이웃 순회 효율 |
| AVL Tree | AVL Tree Node | Reservation Table | 노드별 시간 구간의 균형 잡힌 삽입/조회 |
| Min Heap | Dynamic Array | PP의 에이전트 우선순위 큐 | 최솟값 추출 효율 |
| Linked List | Linked List Node | queue의 뼈대 | head/tail 양끝 O(1) |
| Queue | Linked List | BFS+TEG 탐색, ϕ-BF 라운드 전파 | FIFO로 레벨/라운드 순서 보장 |

avl_tree reservation table은 두 알고리즘이 공유한다 — 노드별 사용 불가
구간 `[a,b)`를 동일하게 저장하되, BFS+TEG는 TEG 빌드 시 점유 정점 제거
필터로, ϕ-BF는 런타임 interval query(ϕ)로 *읽는 방식만* 달리한다. 이것이
두 알고리즘을 자료구조 활용 차이로 비교하는 핵심 축이다.

LIFO 구조(stack)는 채택하지 않는다. 경로 복원의 LIFO가 유일한 후보였으나,
predecessor 배열의 역순 순회로 충분해 고유 사용처가 없다. (linked_list는
queue의 양끝 O(1) 뼈대로 채택 — 중간 삽입은 없지만 queue 양끝 연산이 있다.)
의존성·사용처·선정 근거 상세는 [[data_structure_design]]에서.

---

## 범위

### 다룬다
- 단방향 가중 그래프 위의 MAPF
- 이산 시간 시뮬레이션
- PP 프레임워크 하에서 두 single-agent planner 비교
- 노드 동시 점유 충돌만 처리
- N개 에이전트, 각자의 목표 노드 — 도달 시 카운트 증가, 새 목표 랜덤 생성
- 정해진 layout과 엣지 변경 시나리오에서의 동작 비교

### 다루지 않는다
- 엣지 교차(edge swap) 충돌 — 단방향 그래프 가정으로 발생하지 않음
- 최적성 보장 — PP는 incomplete
- 연속 시간 시뮬레이션
- 실시간 deployment 또는 산업 수준 성능 목표

---

## 성공 기준

### Tier 1 (기본)
- 두 planner가 작은 layout에서 정확히 동작
- 경로 유효성 검증 (노드 충돌 없음, reservation 준수)
- Layout 파일 로딩, 이산 시간 시뮬레이션 작동
- 보고서에서 자료구조 선택 정당화 (수업 평가 기준)

### Tier 2 (목표)
- 동일 layout, 동일 에이전트 수에 대한 정량 비교:
    - 계획 시간
    - 메모리 사용량
- 여러 layout/시나리오에서의 비교

### Tier 3 (여유 있을 시)
- 엣지 변경 시나리오에서 throughput 측정 및 비교
- 재계획 비용 비교

---

## 제출물

- **코드** (*.cpp): GitHub에도 업로드 예정
- **보고서** (*.pdf): 토픽 선정 이유, 자료구조 선택 정당화, 핵심 함수의
  작동 원리, 수업 자료구조 사용처와 이유 (validity & efficiency)
- **발표 영상** (*.mp4, <5분): 토픽, 코드, 선택 이유, 구현 결과
- **발표자료** (ppt/pdf)
- 묶음: `Project_NAME_ID.zip`, LearnUs 제출
- **마감**: 6/6 (토) 23:59

---

## 진행 순서 (수업 명세)

1. **Data Acquisition**: directed graph OHT 사용 스마트 팩토리 layout 수집
2. **Data Storage**: 수집 데이터를 directed weighted graph로 저장
3. **Function Realization**: 자료구조의 basic operation 함수 + useful function 구현

본 프로젝트에서는 (3)이 MAPF simulator 구현으로 구체화된다:
top level(시뮬레이션 + throughput 측정) → high level(PP) → algorithm level
(두 single-agent planner).

---

## 하위 층 인덱스

- [[L2_structure/folder_structure_and_responsibilities]]
- [[L3_modules]] (모듈별 설계)
- [[L4_functions]] (함수 설계)
- [[work_log]] (작업 기록)