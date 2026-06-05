---
layer: L1
tags: [purpose]
---

# OHT MAPF 프로젝트

반도체 fab 또는 스마트 팩토리의 OHT(Overhead Hoist Transport) 시스템을
대상으로 한 단방향 가중 그래프 기반 MAPF(Multi-Agent Path Finding)
시뮬레이터. 두 가지 single-agent path planning 알고리즘을 PP(Prioritized
Planning) 프레임워크 안에서 자료구조 관점으로 비교한다.

- **베이스라인**: Dijkstra + Time-Expanded Graph (TEG)
- **비교 대상**: Parallel ϕ Bellman-Ford (그래프 복제 없이 시간 제약 처리)

---

## 토픽 선정 이유

**자료구조 관점:** MAPF는 다양한 자료구조의 자연스러운 사용처를 제공한다.
시간 제약을 가진 점유 정보(AVL Tree), 우선순위 기반 처리(Min Heap), 관계 기반
layout 표현(Graph), 경로 복원(Stack), 라운드 기반 전파(Queue) — 수업에서
다룬 여러 자료구조가 명확한 역할 분담으로 함께 사용된다.

**알고리즘 비교의 의미:** 두 알고리즘은 동일한 문제를 *자료구조 설계 선택의
차이*로 푼다. Dijkstra+TEG는 그래프를 시간으로 확장하여 V·T 공간을 사용하고,
ϕ Bellman-Ford는 그래프 복제 대신 노드별 점유 정보를 별도 자료구조로 관리한다.
자료구조 선택이 알고리즘의 시공간 특성을 어떻게 결정하는지가 직접 드러난다.

---

## 자료구조 사용 개요

| 자료구조 | 사용처 | 선택 근거 (요약) |
|---|---|---|
| Graph (인접 리스트) | Layout 표현 | 희소 그래프 공간 효율, 이웃 순회 효율 |
| AVL Tree | Reservation Table | 노드별 시간 구간의 균형 잡힌 삽입/조회 |
| Min Heap | 에이전트 우선순위, Dijkstra open set | 최솟값 추출 효율 |
| Stack | 경로 복원 | LIFO가 predecessor 역추적과 일치 |
| Queue | Bellman-Ford 라운드 전파 | FIFO로 라운드 순서 보장 |
| Linked List | 인접 리스트의 구성 요소 | 동적 크기, 삽입 효율 |

상세 정당화는 [[L3_modules]]에서.

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