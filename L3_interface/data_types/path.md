---
layer: L3
kind: data-type
tags: [interface, domain, data-type, path]
---

# Path (흐르는 데이터 타입)

> **구성 축**
> produced-by:: [[planner|PathFinder]]
> passed-through:: [[planner]], [[environment]]

## 무엇
한 에이전트의 시각순 위치 배열 — `path.at(t)` = 시각 t에 에이전트가 있는 노드.
길이는 **H+1**(시각 0..H). PathFinder([[bfs_teg]])가 만들어 [[planner|PP]]가
받고, [[environment]]가 앞 C스텝만 실행한다. 세 모듈을 **관통해 흐르는 값**이다.

- 같은 노드가 연속되면 그 시각엔 머무름(대기). 목표 도달 후 남은 시각은 목표
  노드로 채워진다(정지).
- **빈 Path**(길이 0) = "horizon 내 경로 없음"(도달 실패). `path.empty()`.

## 왜 [[data]]를 상속하지 않는가
[[interval]]·[[agent]]와 달리 Path는 [[data]]를 **상속하지 않는다**. `Data`는
컨테이너([[avl_tree]]·[[min_heap]])에 담겨 `operator<`로 비교되는 "한 칸"인데,
Path는 어떤 컨테이너에도 담기지 않고(PP가 `Path[]` 배열을 직접 다룸) 비교도 없다.
컨테이너 원소가 아닌 값 타입이라 Data 계약이 무의미하다 — "모든 객체는 Data"의
의도된 경계 사례. (결정 기록: [[work_log]] 2026-06-07.)

## 연산
- `at(t)` 읽기 / `set(t, node)` 빌더 측 쓰기 / `length()` / `empty()`.

## 소유권/수명
내부 `int[]` 버퍼를 **소유하는 RAII** — 소멸자가 해제. 복사 불가, **이동(move)
가능**이라 `findPath`가 값으로 싸게 반환한다. 받은 쪽([[planner]]/[[environment]])이
소유한다. 빈 Path를 기본 생성할 수 있어 호출자가 `Path[]` 버퍼를 만든다.

## 좌표
- 추상화: L3 (계약). 코드는 planner 가지의 L5(`planner/pathfinding.{h,cpp}`).
- 구성: [[planner|PathFinder]]에서 생산 → [[planner]] → [[environment]]로 관통.
