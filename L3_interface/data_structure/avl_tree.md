---
layer: L3
tags: [interface, ds]
---

# AVL Tree

## 책임
정렬된 순서로 삽입해도 퇴화하지 않는 **균형 이진 탐색 트리**. 모든 노드에서
좌우 서브트리 높이 차를 1 이하로 유지해 삽입·조회를 O(log N)으로 보장한다.

## 선택 근거
일반 BST는 가장 흔한 삽입 시퀀스에서 최악의 트리가 된다. 특히 *정렬된 순서로
삽입하면* 한쪽으로 치우친 사슬이 되어 연산이 O(N)으로 퇴화한다.

AVL(Adelson-Velskii and Landis) Tree는 **Balance Condition**으로 이를 막는다:

- 각 노드의 height 정의: **leaf=1, NULL=0** ([[avl_tree_node]] 초기값과 통일한 구현
  규약). 코드상 `height(node) = node ? node->height : 0`. 고전 정의(leaf=0/NULL=−1)와는
  +1 평행이동이라 balance factor는 동일하다.
- 모든 노드에서 $|h_l - h_r| \le 1$ 을 유지한다.

[[reservation_table]]은 노드별 시간 구간(Interval)을 빈번히 삽입·조회하므로,
이 균형 보장이 자료구조 선택의 직접 근거다.

## 구현 방식

**public field**:
 - 없음 (`root`는 캡슐화한다).

**private field**:
- `AvlTreeNode* root` — 트리의 루트. 초기값 `nullptr`.

**methods** (잠정 — 사용처가 요구하는 연산이 확정되면 보강):

*공개(public):*
 - 생성자 — 빈 트리(`root=nullptr`).
 - 소멸자 — 모든 `AvlTreeNode`를 해제. 담은 `Data*`는 delete 하지 않음(비소유).
 - `void insert(Data* elem)` — `operator<` 기준 위치에 삽입 후, 경로를 거슬러
   높이 갱신·필요 시 회전으로 균형을 회복한다.
 - `Data* find(Data* key) const` — `operator<` 비교로 같은 키의 `Data*`를 탐색
   (없으면 `nullptr`).
 - `bool contains(Data* key) const` — 존재 여부만. (`find`/`contains` 중 사용처가
   쓰는 쪽으로 정리. 잠정)
 - `void remove(Data* key)` — 삭제 후 재균형. 사용처(재계획)가 요구하는지 미확정. 잠정.
 - `void clearAndDelete()` — 모든 노드 **및 담은 `Data*`까지** 삭제 후 빈 트리로
   리셋. 기본 소멸자는 비소유(노드만)지만, 이건 호출자가 `Data*`를 소유했고 지금
   비워도 됨을 확인하고 부르는 **명시적 옵트인**이다. delete 결정은 호출자,
   실행은 트리가 대행. (reservation_table이 Interval 일괄 정리에 사용.)

*내부(private) — 균형 유지 기계, 외부 비노출:*
 - `AvlTreeNode* insert(AvlTreeNode* node, Data* elem)` — 재귀 삽입 헬퍼. 반환값으로
   (회전된) 부분트리 루트를 위로 전달한다.
 - `int height(AvlTreeNode* node) const` — NULL이면 0, 아니면 노드 `height`(`friend`로
   직접 접근). 규약 leaf=1 / NULL=0.
 - `void updateHeight(AvlTreeNode* node)` — 자식 높이로 노드 높이를 재계산.
 - `int balanceFactor(AvlTreeNode* node) const` — 좌우 서브트리 높이 차.
 - `AvlTreeNode* rotateLeft(AvlTreeNode* node)`, `AvlTreeNode* rotateRight(AvlTreeNode* node)`
   — 단일 회전. 새 부분트리 루트를 반환.
 - `AvlTreeNode* rebalance(AvlTreeNode* node)` — balance factor로 LL/LR/RR/RL을 판정해
   알맞은 (단일/이중) 회전을 적용. 이중 회전은 `rotateLeft`+`rotateRight` 조합(아래 Case 2).

구체 알고리즘(의사코드)은 [[L4_mechanism]]에서 작성한다. 여기서는 이름과 역할까지.

### Insertion에서 Balance Condition 유지
삽입이 균형을 깨뜨리는 위치는 4가지로 나뉜다 — 새 노드가 (불균형이 생긴 노드
기준) left child의 left/right subtree, 또는 right child의 left/right subtree 중
어디로 들어갔는가. **left/right 대칭성**을 이용하면 2가지 Case로 줄일 수 있다
(`left child height > right child height` 쪽만 따지고, 반대는 대칭).

#### Case 1. Left Child – Left Subtree Insertion → Single Rotation
![[leftchild_leftsubtreeinsertion.png|500]]

**Solution: Single Rotation**
![[leftchild_leftsubtreeinsertion_solution.png|500]]

#### Case 2. Left Child – Right Subtree Insertion → Double Rotation
![[leftchild_rightsubtreeinsertion.png|300]]

**Solution: Double Rotation**
![[double_rotation_1.png|500]]
![[double_rotation_2.png|500]]

Single Rotation 한 번으로는 풀리지 않는다. 먼저 **첫 번째 Rotation**으로
*Single Rotation으로 해결되는 형태*를 만든 뒤, **두 번째(Single) Rotation**으로
Balance Condition을 만족시킨다. 아래에서 그 과정을 단계별로 본다.

불균형 노드를 `k3`, 그 왼쪽 자식을 `k1`, `k1`의 오른쪽 자식을 `k2`라 하자.
직관을 위해 서브트리 A·B·C·D를 모두 높이 `h`로 둔다.

```
          k3            ← 불균형 (좌 h+2 / 우 h, 차이 2)
         /  \
       k1    D(h)
      /  \
   A(h)   k2(h+1)
         /  \
      B(h)   C(h)
```

##### 왜 Single Rotation으로는 안 되는가
무거운 덩어리(`k2`)가 `k1`의 *오른쪽* — 즉 "안쪽" — 에 있다. 여기서 `k3`을
오른쪽으로 한 번 돌려(`k1`을 올려) 보면:

```
        k1            ← 여전히 불균형 (좌 h / 우 h+2, 차이 2)
       /  \
    A(h)   k3
          /  \
     k2(h+1)  D(h)
       /  \
      B(h)  C(h)
```

불균형이 **반대쪽으로 옮겨갔을 뿐**이다. Single Rotation은 무거운 쪽이
"바깥쪽"(left-left / right-right)일 때만 통한다. 지금은 "안쪽"(left-right)이라
한 번으로는 풀리지 않는다.

##### 두 번의 회전
**① `k1`에서 left rotation** — `k2`를 `k1` 위로 올린다(`k2`의 왼쪽 자식 `B`가
`k1`의 오른쪽으로 내려옴). 무거운 손자 `k1`이 *왼쪽의 왼쪽*으로 오면서,
Single Rotation으로 풀리는 left-left 형태가 된다:

```
          k3
         /  \
     k2(h+1) D(h)
      /  \
    k1    C(h)
   /  \
 A(h)  B(h)
```

**② `k3`에서 right rotation** — `k2`를 `k3` 위로 올린다(`k2`의 오른쪽 자식 `C`가
`k3`의 왼쪽으로 내려옴). 균형 회복:

```
            k2
          /    \
        k1      k3
       /  \    /  \
     A(h) B(h)C(h) D(h)
```

`k1 = k3 = h+1`, `k2`의 좌우 높이 차 0 — Balance Condition 만족.

##### 직관
- **가운데 값 `k2`가 꼭대기로 올라가고**, `k1`·`k3`이 그 양쪽 자식이 된다.
- in-order는 `A k1 B k2 C k3 D`로 처음부터 끝까지 불변 — 회전은 모양만 바꾸고
  대소 관계는 보존한다.
- 정리하면 LR 케이스 = `rotateLeft(k1)` → `rotateRight(k3)`. RL(right-left)은
  완전 대칭(`rotateRight` → `rotateLeft`).

> 위 예시는 A·B·C·D를 모두 같은 높이로 둬 결과가 정확히 균형(차이 0)이 됐다.
> 실제 한 번의 삽입에서는 B 또는 C 중 하나만 한 단계 깊지만, 두 회전은 동일하고
> 끝나면 부분트리 루트의 균형이 회복된다.

**소유권**: 저장한 `Data*`는 **기본적으로 비소유**. 소멸자는 `AvlTreeNode`만
해제하고 각 `Data*`는 delete 하지 않는다. 예외로 `clearAndDelete()`는 호출자가
명시적으로 부를 때만 `Data*`까지 정리한다(소유는 호출자에, 실행만 트리가 대행).
키 비교는 `Data::operator<`에 위임한다. 전체 정책은
[[data_structure_design#소유권]].

## Time Complexity
 - 삽입(insert): O(log N) — 탐색 + 경로상 상수 번의 회전(O(1)).
 - 탐색(find/contains): O(log N).
 - 삭제(remove): O(log N) (제공 시).
 - 단일/이중 회전: O(1).
 - 전체 순회: O(N).

## 의존성
### 1. 의존
- [[avl_tree_node]] (부품 — 트리의 뼈대)
- [[data]] (간접 — avl_tree_node가 `Data*`를 담는다)

### 2. 의존됨
- [[reservation_table]]

## 사용처
[[reservation_table]] — 노드별 시간 구간 Interval을 저장·조회.

선정 근거는 [[data_structure_design#선정 근거]].
