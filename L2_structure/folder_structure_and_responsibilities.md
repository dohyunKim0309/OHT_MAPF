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
├── L1_purpose/
├── L2_structure/
├── L3_modules/
├── L4_functions/
├── L5_implementation/
├── data/
├── results/
└── work_log.md
```

## 계층 구조의 원칙

L1~L5는 추상화 층에 대응한다. 위로 갈수록 압축되어 있고, 아래로 갈수록
구체적이다. 코드(`L5_implementation/`)도 이 계층의 일부다. 빌드 관습상
`src/`로 부르는 것이 일반적이지만, 본 프로젝트는 층의 일관성을 우선시하여
`L5_implementation/`을 채택한다.

## 기술 스택

- 언어: C++
- IDE: CLion
- 빌드: CMake

## 각 위치의 책임

### `L1_purpose/`
- 프로젝트의 목적, 범위, 성공 기준
- 변경 빈도: 매우 낮음

### `L2_structure/`
- 프로젝트의 조직적 결정 (폴더 구조, 명명 규칙, 작업 흐름)
- 본 문서가 여기 속함
- 변경 빈도: 낮음

### `L3_modules/`
- 모듈별 1문서 원칙
- 각 모듈: 책임, 의존성, 외부 인터페이스, 내부 구성(파일 포함), 미결 사항
- `module_dependency_map.md`: 모듈 간 데이터 흐름 전체 조망
- 변경 빈도: 중간

### `L4_functions/`
- 함수 시그니처, 사전/사후 조건, 소유권, 필요시 의사코드
- 모듈별 1문서 또는 함수별 1문서
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

```
L5_implementation/
├── CLAUDE.md
├── ds/              ← 수업 자료구조 (AVL, MinHeap, Stack, Queue, LinkedList)
├── graph/           ← Graph (base) + TEG (derived)
├── planner/         ← Planner (base) + DijkstraTEG + BellmanPhi
├── pp/              ← PP 프레임워크 + ReservationTable
└── main.cpp
```

각 모듈의 파일 구성과 책임은 [[L3_modules]]의 해당 모듈 문서를 참조.

### 분리 원칙

- `ds/`: 재사용 가능한 일반 자료구조. 수업에서 가져옴. 도메인 무관.
- `graph/`: 프로젝트 도메인 자료구조. `ds/`의 LinkedList를 응용한 인접 리스트
  기반. TEG는 Graph를 상속.
- `planner/`: 교체 가능한 단일 에이전트 경로 탐색. Planner 추상 클래스 하에
  DijkstraTEG와 BellmanPhi가 평행 배치. PP가 Planner*를 받아 사용.
- `pp/`: 상위 프레임워크. ReservationTable은 PP에 종속되므로 여기 둠.

## CLAUDE.md 배치

CLAUDE.md는 작업 위치에 분산 배치된다. 전체 배치와 각 파일의 역할은
[[claude_md_index]] 참조.

## 작업 흐름

1. 어느 층에서 작업할지 결정.
2. 해당 층의 문서를 시야에 두고 작업 시작.
3. 막힘 발생 시 [[work_log]]에 기록.

## 미결 사항

- L4 문서들의 양식: L3 모듈 문서가 자리 잡은 후 결정.
- `results/`의 하위 구조: 실험이 시작되면 정의.