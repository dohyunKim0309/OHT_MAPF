---
layer: L3
tags: [interface, domain, environment]
---

# Environment (시뮬레이션 세계)

## 위치
도메인 구성요소. `data_structure/`를 빌려 쓰고, [[planner]]에게 계획을
요청한다. 의존은 environment → {data_structure, planner} 방향.

## 책임
시뮬레이션 세계를 보유하고 시간을 진행시킨다. layout과 에이전트들을 들고,
**에이전트에게 새 목표가 생긴 순간** planner에게 그 에이전트의 경로 전체를
받아, 받은 경로를 따라 에이전트를 움직이며 throughput을 측정한다. 세계를
그리는 visualizer를 포함한다.

**아는 것:**
- layout([[graph]])과 전체 에이전트 집합(고정 N), 현재 시각.
- 시간을 한 스텝 진행시키는 법, 목표 도달 시 카운트 증가·새 목표 부여.
- throughput(도달 수 / 경과 시간) 측정. 세계 상태를 그림으로 출력(visualizer).

**하지 않는 것:**
- 경로를 *계획*하지 않는다 — 그건 [[planner]]에게 요청한다. environment는
  "어디로 갈지"를 planner에게 묻고, 받은 경로를 "실제로 실행"하는 일만 한다.
- 자료구조 내부를 모른다 — 토대를 빌려 쓸 뿐이다.
- 충돌을 *회피*하지 않는다 — 회피는 planner가 reservation으로 보장한 경로에
  이미 담겨 있다. environment는 그 경로를 그대로 실행한다.

## 구성

**빌려온 자료구조 (data_structure):**
- [[graph]] — layout(unweighted 확장본). planner에게 전달.
- 에이전트 집합: 고정 N개라 단순 배열(또는 dynamic_array)로 보관.

**자기 데이터 (구체 Data 자식):**
- [[#Agent]] — `id`, `priority`, `현재 위치`, `목표 위치`. **소유는
  environment**(planner는 우선순위 큐에 빌려 담을 뿐).

**자기 알고리즘:**
- 시뮬레이션 루프(시간 진행), throughput 측정, 목표 재부여.
- visualizer — 세계 상태를 그림으로.

### Agent (구체 Data 자식)
- **필드**: `int id`, `int priority`, `int current`(현재 노드), `int goal`(목표 노드).
- `operator<` override: `priority` 비교. PP의 min_heap이 이걸로 우선순위 추출.
- 소유: environment. min_heap은 비소유로 Agent*를 빌린다.

## 인터페이스 (environment ↔ planner)

- **호출 시점**: 에이전트가 **새 목표를 받은 순간에만** planner를 부른다.
  이동 중(경로 실행 중)에는 부르지 않는다.
- **요청/응답**: planner에게 (그 에이전트의 현재 위치·목표, graph, reservation)을
  넘기면, planner는 **경로 전체**(시각순 노드 시퀀스, horizon 내)를 반환한다.
- environment는 반환된 경로를 보관하고, 매 스텝 그 경로의 다음 위치로 에이전트를
  옮긴다.

## 시뮬레이션 흐름

1. 각 에이전트는 자기 경로를 **끝까지 실행**한다(매 스텝 다음 노드로).
2. **목표 도달** 시: 도달 카운트를 올리고, 그 에이전트는 **해당 horizon 끝까지
   정지**한다(현재 위치 유지).
3. horizon이 끝나면(또는 정지가 풀리면): 새 목표를 할당받고, planner를 다시
   불러 새 경로를 받아 이동을 재개한다.
4. throughput = 누적 도달 카운트 / 경과 시각.

> 한 에이전트의 경로가 reservation에 기록되어야 다음 에이전트(낮은 우선순위)가
> 그것을 피한다. PP의 우선순위 순서·reservation 기록은 planner의 일이다.

## 하위 구성요소
- visualizer — environment에 포함. 세계 상태(에이전트 위치)를 그림으로. 별도
  문서 여부는 구현 시 결정.

## 미결
- planner 인터페이스의 정확한 시그니처(경로 반환 형식: 노드 배열? (node,time)
  배열?) — planner 가지에서 확정.
- 새 목표 생성 방식(랜덤 노드, 도달 가능성 보장 여부).
- 여러 에이전트의 호출 순서가 PP 우선순위와 어떻게 맞물리는지 — planner와 함께.
- visualizer 출력 형식(텍스트/그래픽).
