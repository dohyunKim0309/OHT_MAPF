---
tags: [meta, claude_md]
---

# CLAUDE.md — Project Entry Point

본 프로젝트는 *명시적 방법론*에 따라 계층적으로 조직된다.
작업 시작 전, 다음 순서로 문서를 읽는다.

## 1. 방법론 (필수)

- [[L1]] — 왜 계층적 양방향 흐름인가
- [[L2]] — 그것을 어떻게 운영하는가

위 두 문서를 *반드시 먼저 읽는다*. 본 CLAUDE.md는 인덱스이지 방법론의
요약이 아니다. 작업 원칙, 막힘의 정의, 인간/AI 분담 등은 모두 위 문서에서
확인한다.

방법론 문서는 본 프로젝트 적용을 통해 발전 중이다. 작업하다 방법론
자체의 결함이나 부족을 발견하면 인간에게 보고한다.

## 2. 프로젝트 구조

- [[L1_purpose/README]] — 프로젝트의 목적, 범위, 성공 기준
- [[L2_structure/folder_structure_and_responsibilities]] — 폴더 구조, 작업 흐름
- [[L2_structure/claude_md_index]] — 본 문서를 포함한 모든 CLAUDE.md 추적

## 3. 작업 종류별 진입점

작업 종류에 따라 위 문서들 외에 추가로 참조할 곳:

- **L3 모듈 설계**: `L3_modules/` 내 기존 문서 (양식 일관성)
- **L4 함수 명세**: 해당 L3 모듈 문서
- **L5 코드 작성**: 해당 L4 함수 문서, `L5_implementation/CLAUDE.md`