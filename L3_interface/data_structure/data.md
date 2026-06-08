---
layer: L3
tags: [interface, ds]
---

# Data (베이스 클래스)

## 책임
오로지 타입 불문 객체 컨테이너. 자료구조와 빌딩 블록이 담는 "한 칸"이다.

이 모듈의 경계는 **타입 불문 / 타입 인지** 축으로 판정한다.

**들어온다 (이 모듈):** 구체 데이터 타입을 *모르고도* 성립하는 것.
- 다형적 수명 관리 (가상 소멸자)
- "비교 가능하다"는 계약의 *선언* (가상 `operator<` 시그니처와 기본값)

**나간다 (도메인 모듈):** 구체 타입을 *알아야* 성립하는 것.
- 각 구체 Data 자식의 정의 (Interval, Agent, …)
- `operator<`의 실제 비교 로직 (static_cast 후 필드 비교)
- 직렬화·출력처럼 구체 필드를 읽어야 하는 모든 것

타입 불문이라는 점에서 빌딩 블록([[avl_tree_node]], [[linked_list_node]],
[[dynamic_array]])과 닮았으나,
빌딩 블록은 Data를 *담아 구조를 만드는* 뼈대이고 Data는 *담기는* 한 칸이라는
점에서 다르다. 경계가 헷갈리면 [[data_structure_design]]의 화살표
의미("담는다" vs "부품")를 본다.

## 선택 근거
여러 자료구조가 동일한 컨테이너 메커니즘으로 다양한 데이터를 다루기 위해
다형성을 사용한다. 자료구조 코드를 데이터 타입마다 중복하지 않고 Data
포인터 하나로 통일한다.

비교가 필요한 자료구조([[avl_tree]], [[min_heap]])를 위해 `operator<`를
가상 함수로 제공한다. 연산자 오버로딩과 다형성을 결합한 패턴.

## 구현 방식

**베이스 클래스**:
- 가상 소멸자 (다형적 삭제)
- 가상 `operator<`: 비교 가능을 선언하지 않은 자식만 기본 구현에 도달한다.
  기본 구현은 이를 *계약 위반*으로 보고 `assert` 로 즉시 드러낸다
  (릴리스 빌드에서는 안전한 기본값 `false`). 비교가 필요한 자식은 override.

**구체 Data 자식**:
- 각 도메인 모듈에 위치. `data_structure/`에는 베이스만.
- 비교 필요 시 `operator<` override. 내부에서 `static_cast` 후 비교.

**타입 안전성 정책**:
- 한 인스턴스는 같은 타입 Data만 담는다 (호출자 책임).
- 다른 타입 간 비교는 정의되지 않은 동작. 템플릿으로 컴파일 시점 강제
  가능하나 본 프로젝트는 다형성으로 통일.
- 비교 미지원 Data 의 비교, 다른 타입 간 비교 — 둘 다 계약 위반이며
  `assert` 로 디버그 시 잡는다. `false` 로 조용히 삼키지 않는다.

## 외부 제공 기능
- 가상 소멸자
- 가상 비교 연산자 `operator<`

## 의존성
### 1. 의존: 없음 (최상위 베이스)
### 2. 의존됨 (직접 — Data를 담는 빌딩 블록)
 - [[avl_tree_node]]
 - [[linked_list_node]]
 - [[dynamic_array]]

자료구조(avl_tree, min_heap, graph, linked_list, queue)는 빌딩 블록을 통해
*간접적으로* Data를 담는다. 직접 의존이 아니다.
관계 전체는 [[data_structure_design]] 참조.

## 사용처

**빌딩 블록이 Data를 담는다** (타입 불문, ds 내부):
- [[avl_tree_node]] — `Data*` 한 칸 (avl_tree의 뼈대)
- [[linked_list_node]] — `Data*` 한 칸 (linked_list의 뼈대)
- [[dynamic_array]] — 각 칸이 Data*

**구체 Data 자식(흐르는 데이터 타입)이 도메인 모듈에서 정의되어 자료구조에 저장됨**
(타입 인지). 전체 조망은 [[L2_structure/data_types]]:
- [[interval]] — [[reservation_table]]가 정의·소유, [[avl_tree]]에 담김.
- [[agent]] — [[environment]] 소유, [[planner]]의 [[min_heap]]에 빌려 담김.
- (BFS+TEG의 TEG 정점은 정수 인코딩으로 다뤄 별도 Data 자식을 두지 않음.)

전체 의존 관계는 [[data_structure_design]] 참조.