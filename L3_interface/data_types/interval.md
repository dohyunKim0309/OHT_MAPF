---
layer: L3
kind: data-type
tags: [interface, domain, data-type, interval]
---

# Interval (흐르는 데이터 타입)

> **구성 축**
> inherits:: [[data]]
> stored-in:: [[avl_tree]]
> defined-for:: [[reservation_table]]

## 무엇
시간 점유 구간 `[start, end)` — 정수 반열림 구간. 어떤 그래프 노드가 *언제부터
언제까지 사용 불가인지*를 한 조각으로 나타낸다. [[reservation_table]]이 노드별로
이 조각들을 모아 정렬·병합 상태로 보관한다.

부품이 아니라 **부품 사이를 흐르는 값**이다 — `reserve`로 만들어져 [[avl_tree]]에
담기고, 병합되며, `isFree`/`phi` 질의의 대상이 된다.

## 필드
- `int start` — 포함(inclusive)
- `int end` — 제외(exclusive)

(간단 버전. agent id 등은 병합과 충돌하므로 두지 않는다 — 같은 구간이면 누가
점유했든 "막혔다"는 사실만 남긴다.)

## 연산
- `operator<` **override** — 시작점(`start`) 비교. [[avl_tree]]가 정렬·탐색하는
  근거다. [[data]]가 선언한 가상 비교 계약을 구체화한 것 (연산자 오버로딩 + 다형성).
- `touches(const Interval&)` — 두 구간이 닿거나 겹치는지. 경계가 만나도(`[2,4)` +
  `[4,5)`) 병합 대상으로 친다.
- `merge(const Interval&)` — 합친 구간 `[min start, max end)`.

`touches`/`merge`는 **Interval 고유 메서드**다([[data]] 베이스의 가상함수가 아님).
[[avl_tree]]는 이를 모르고, [[reservation_table]]만 호출해 "닿으면 병합"을 조율한다.

## 소유권
[[reservation_table]]이 `Interval*`를 소유(`new`/`delete`)한다. [[avl_tree]]는
비소유로 담기만 한다 — 단, 옵트인 `clearAndDelete()`로 일괄 정리를 위임받을 수 있다.

## 좌표
- 추상화: L3 (계약). 기전·코드는 [[reservation_table]] 가지의 L4/L5.
- 구성: [[reservation_table]]의 데이터(그 모듈이 정의·소유).
