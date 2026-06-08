---
layer: L3
tags: [interface, domain, environment]
---

# Environment (시뮬레이션 세계)

> **구성 축** (axis-role: top)
> calls:: [[planner]]
> uses:: [[graph]]
> owns:: [[agent]]

## 위치
도메인 구성요소. `data_structure/`를 빌려 쓰고, [[planner]]에게 계획을
요청한다. 의존은 environment → {data_structure, planner} 방향.

## 책임
시뮬레이션 세계를 보유하고 시간을 진행시킨다. layout과 에이전트들을 들고,
**매 라운드(C스텝)마다** PP에게 전체 에이전트의 H스텝 계획을 받아, 그중 앞
C스텝을 실행하며 throughput을 측정한다(receding horizon). 세계를 그리는
visualizer를 포함한다.

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

### Agent (이 모듈이 소유하는 흐르는 데이터 타입)
상세 정의는 [[agent]]. 요약: `id`/`priority`/`current`/`goal`을 묶은 `Data` 자식
(`operator<`=priority). environment가 배열로 **소유**하고, [[planner]]의 [[min_heap]]은
비소유로 `Agent*`를 빌린다.

## Environment 인터페이스

세계를 보유하고 시뮬레이션을 돌리는 최상위 객체.

```
class Environment {
    Environment(const Graph& graph, int agentCount, PrioritizedPlanning* pp,
                int H, int C);
    void step();        // 한 라운드: PP로 계획 → 각 에이전트 C스텝 전진 →
                        //   목표 도달 처리(카운트++·새 목표) → 시각 += C
    void run(int rounds);    // step()을 rounds회
    double throughput() const; // 누적 도달 수 / 경과 시각 (실수)
    int  arrivedCount() const; // 누적 도달 수 (관측용)
    int  elapsed() const;      // 경과 시각 (관측용)
    void render() const;     // visualizer: 세계 상태 출력
};
```

- **보유**: Agent 배열(소유), [[graph]](빌림), [[planner|PrioritizedPlanning]]*
  (빌림), 현재 시각, 누적 도달 카운트, 파라미터 H(계획 지평)·C(commit 스텝).
- **step 한 번** = 한 라운드. PP에게 H스텝 계획을 받아 앞 C스텝만 실행.
- 새 목표: 도달 시 원본 범위 `[0, graph.originalSize())`에서 랜덤.

## 시뮬레이션 모델 — Receding Horizon

두 파라미터로 "멀리 계획, 조금 실행, 자주 재계획"한다:
- **H (계획 지평, planning horizon)**: 한 번 계획할 때 내다보는 타임스텝 수
  (예: 100). 멀리 봐서 좋은 경로를 찾는다.
- **C (실행 스텝, commit length)**: 계획한 경로 중 **앞 C스텝만 실제 실행**한 뒤
  재계획한다 (예: 10~20). C < H면 receding, C = H면 한 번에 다 실행.

H를 크게 두어 장기 계획이 가능하고, C로 자주 갱신해 동적 상황(도달·새 목표)에
적응한다.

## 인터페이스 (environment ↔ planner)

- **호출 시점**: **매 라운드**(C스텝마다) PP에게 전체 에이전트의 계획을 요청한다.
- **요청/응답**: PP는 현재 모든 에이전트 위치·목표로 우선순위 순 계획을 돌려,
  각 에이전트의 **H스텝 경로**를 반환한다(시각순 노드 배열, 길이 H+1).
- environment는 각 경로의 **앞 C스텝만 실행**하고, 다음 라운드에 다시 요청한다.

## 시뮬레이션 흐름 (한 라운드)

1. PP에게 현재 상태로 전체 계획을 요청 → 각 에이전트의 H스텝 경로 수신.
2. 각 에이전트를 그 경로대로 **C스텝 전진**시킨다.
3. C스텝 동안 **목표 도달**한 에이전트: 도달 카운트++ , **랜덤 새 목표** 부여
   (원본 범위 `[0, originalSize())`에서). 다음 라운드 계획에 새 목표 반영.
4. 라운드 종료. reservation은 라운드 단위로 비우고 다음 라운드에 다시 쌓는다.
5. throughput = 누적 도달 카운트 / 경과 시각.

> 한 라운드 안에서 높은 우선순위 에이전트의 H스텝 경로가 reservation에 먼저
> 기록되고, 낮은 우선순위가 그것을 피한다. 우선순위 순서·기록은 PP의 일.

## 하위 구성요소
- visualizer — environment에 포함. 세계 상태(에이전트 위치)를 그림으로. 별도
  문서 여부는 구현 시 결정.

## 미결
- 새 목표 도달 가능성 — 원본 범위 `[0, originalSize())`에서 랜덤으로 뽑되
  (가상노드 제외), 강연결이 아니면 도달 불가 목표가 나올 수 있음. 강연결 검증은
  추후. planner 실패 시 재추첨 등 처리 필요.
- visualizer 출력 형식(텍스트/그래픽).

(해소: 경로 반환 = 길이 H 노드 배열. 새 목표 = 원본 범위 랜덤. PP 우선순위
호출 순서는 [[planner]] PP 흐름에 확정.)
