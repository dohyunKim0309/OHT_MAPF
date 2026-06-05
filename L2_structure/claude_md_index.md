---
layer: L2
tags: [structure, claude_md]
---

# CLAUDE.md Index

각 CLAUDE.md의 위치와 역할을 추적한다. 임베드를 통해 전체 내용을 한 페이지에서
조망할 수 있다.

## 배치 원칙

CLAUDE.md는 *작업 위치의 컨텍스트*다. 작업 디렉토리에 있는 CLAUDE.md가
자동으로 시야에 들어오므로, *그 위치에서 필요한 안내만* 담는다.

## 전체 목록

### `/CLAUDE.md`
- 위치: 프로젝트 루트
- 역할: 프로젝트 전체 메타 안내. 층 구조 개요, 문서 작성 원칙,
  [[work_log]] 사용법.
- 작업 시 시야에 들어오는 경우: 모든 작업.

![[../CLAUDE]]

### `/L5_implementation/CLAUDE.md`
- 위치: 코드 루트
- 역할: C++ 코딩 컨벤션, 빌드 명령, 코드 스타일, 함수 작성 시 참조해야 할
  L4 문서 안내.
- 작업 시 시야에 들어오는 경우: 코드 작성/수정.

![[../L5_implementation/CLAUDE]]

## 향후 추가 가능 위치

하위 폴더 CLAUDE.md는 *필요해질 때만* 추가한다. 예시:

- `/L5_implementation/planner/CLAUDE.md`: planner 모듈 특유의 강한 제약이
  생길 경우 (예: Planner 추상 클래스의 계약 명세).
- `/L5_implementation/ds/CLAUDE.md`: 자료구조 구현 시 따라야 할 별도 규칙이
  필요할 경우.

추가 시 본 인덱스에 위치, 역할, 임베드를 추가한다.