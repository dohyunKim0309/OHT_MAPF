---
layer: L3
kind: data-type
tags: [interface, domain, data-type, agent]
---

# Agent (흐르는 데이터 타입)

> **구성 축**
> inherits:: [[data]]
> stored-in:: [[min_heap]]
> owned-by:: [[environment]]
> read-by:: [[planner]]

## 무엇
한 대의 OHT(이동체) 상태를 묶은 값 — `id`, `priority`, 현재 노드, **목표 시퀀스**.
부품이 아니라 **부품 사이를 흐르는 값**이다: [[environment]]가 소유하며, 매 라운드
[[planner|PrioritizedPlanning]]이 우선순위 순으로 읽어 [[min_heap]]에 빌려 담고, 그
목표 시퀀스를 findPath에 넘긴다.

[[interval]]처럼 단순한 데이터 묶음이다 — 필드를 묶고 비교 기준 하나를 정의할 뿐,
스스로 행동(시뮬레이션·계획)하지 않는다.

## 필드
- `int id` — environment의 에이전트 배열 인덱스(안정적).
- `int priority` — PP 처리 순서(작을수록 먼저). 기본 `priority = id`(잠정).
- `int current` — 현재 노드(원본 인덱스). environment가 전진시킬 때 갱신.
- **목표 시퀀스** `int goals[K]` + `int goalCount` — 앞으로 방문할 목표들(순서대로).
  단일 목표가 아니라 *큐*인 이유: 한 라운드(H스텝) 안에 목표에 도달해도 **무한 정지하지
  않고** 다음 목표로 이어가야 target conflict를 피하기 때문(도달 시 `dwell`스텝만 작업
  정지 후 다음 목표). environment가 무한 스트림으로 채운다(도달 시 다음 랜덤 목표 보충).
  K는 H를 채우기에 충분한 lookahead.

`current`·목표 시퀀스는 가변(environment가 전진·보충), `id`/`priority`는 고정.

## 연산
- `operator<` **override** — `priority` 비교. [[min_heap]]이 우선순위 추출에
  쓴다. [[data]]의 가상 비교 계약을 구체화 (연산자 오버로딩 + 다형성).

## 소유권
[[environment]]가 `Agent` 배열을 소유한다. [[planner]]의 [[min_heap]]은 비소유로
`Agent*`를 빌려 담을 뿐이다.

## 좌표
- 추상화: L3 (계약). 코드는 [[environment]] 가지의 L5(`environment/agent.{h,cpp}`).
- 구성: [[environment]]가 소유, [[planner]]가 읽음 — 두 모듈 사이를 흐른다.
