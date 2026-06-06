---
layer: L4
tags: [mechanism, ds]
---

# AVL Tree — 함수 명세 (L4)

상위 설계는 [[avl_tree]] (L3). 이 문서는 **1) Fields, 2) Methods**로 나누고, 각각을
**public / private** 하위에서 항목별로 서술한다. 함수 항목은 **시그니처 · 사전/사후
조건 · 소유권 · 의사코드**를 담는다. 높이 규약은 **leaf=1, NULL=0**
(즉 `height(nullptr)=0`).

**공통 소유권**: 트리는 `AvlTreeNode`를 소유(생성·삭제)하고, 담는 `Data*`는
**비소유**(보관만, delete 안 함). 키 비교는 `Data::operator<`에 위임한다(한 트리의
원소는 같은 구체 타입 가정). 각 항목의 "소유권"에는 함수별 차이만 적는다.

`*a < *b`는 `Data::operator<` 호출, "동치"는 `!(*a < *b) && !(*b < *a)`를 뜻한다.

---

## 1. Fields

### public
- 없음 (`root`는 캡슐화한다).

### private
- `AvlTreeNode* root` — 트리의 루트. 초기값 `nullptr`(빈 트리).

---

## 2. Methods

### public

#### `void insert(Data* elem)` — 삽입 진입점
- **사전**: `elem != nullptr`이고 트리 키들과 `operator<`로 비교 가능.
- **사후**: `elem`이 BST 위치에 삽입되고 트리 전체가 AVL 불변(`|bf| ≤ 1`) 유지.
- **소유권**: 노드 1개 생성(트리 소유), `elem`은 비소유로 보관.

```
insert(elem):
    root = insert(root, elem)        // 아래 private 재귀 헬퍼
```

#### `Data* find(Data* key) const` — 조회
- **사전**: `key != nullptr`이고 비교 가능.
- **사후**: `key`와 동치인 키를 담은 `Data*` 반환, 없으면 `nullptr`. 트리 불변.
- **소유권**: 읽기 전용. 반환 `Data*`의 소유권 이전 없음(트리도 비소유).

```
find(key):
    cur = root
    while cur != nullptr:
        if *key < *cur.data:      cur = cur.left
        else if *cur.data < *key: cur = cur.right
        else:                     return cur.data     // 동치
    return nullptr
```

### private

균형 유지 기계. 외부에 노출하지 않는다. `friend`로 노드 `height`에 직접 접근한다.

#### `height` / `updateHeight` / `balanceFactor` — 높이 보조
- **시그니처**:
  - `int height(AvlTreeNode* node) const`
  - `void updateHeight(AvlTreeNode* node)`
  - `int balanceFactor(AvlTreeNode* node) const`
- **사전/사후**:
  - `height`: `node==nullptr`이면 `0`, 아니면 노드 `height`. 트리 불변.
  - `updateHeight`: (사전: 두 자식 `height` 정확) `node.height = 1 + max(자식 height)`.
  - `balanceFactor`: (사전: `node != nullptr`) 반환 `height(left) − height(right)`,
    양수면 왼쪽이 높음. 트리 불변.
- **소유권**: 읽기/필드 갱신만.

```
height(node):
    if node == nullptr: return 0
    return node.height

updateHeight(node):
    node.height = 1 + max(height(node.left), height(node.right))

balanceFactor(node):
    return height(node.left) - height(node.right)
```

#### `rotateRight` / `rotateLeft` — 단일 회전 (좌우 대칭)
- **시그니처**:
  - `AvlTreeNode* rotateRight(AvlTreeNode* node)` — 왼쪽이 무거운 경우(LL).
  - `AvlTreeNode* rotateLeft(AvlTreeNode* node)` — 오른쪽이 무거운 경우(RR).
- **사전**: `node != nullptr`이고 회전 방향의 자식(`rotateRight`는 `node.left`,
  `rotateLeft`는 `node.right`)이 `!= nullptr`.
- **사후**: 1회 회전한 부분트리의 **새 루트**를 반환. `node`와 새 루트의 `height`가
  갱신됨. in-order(대소 순서) 불변.
- **소유권**: 노드 링크 재배치만. 생성·삭제 없음. `Data*` 손대지 않음.

```
rotateRight(node):           // node = k3, pivot = k1 (왼쪽 자식)
    pivot = node.left
    node.left = pivot.right
    pivot.right = node
    updateHeight(node)       // 아래로 내려간 옛 루트를 먼저
    updateHeight(pivot)
    return pivot

rotateLeft(node):            // 대칭
    pivot = node.right
    node.right = pivot.left
    pivot.left = node
    updateHeight(node)
    updateHeight(pivot)
    return pivot
```

#### `AvlTreeNode* rebalance(AvlTreeNode* node)` — 균형 회복
LR/RL(안쪽) 케이스는 자식을 먼저 단일 회전해 LL/RR(바깥쪽)로 만든 뒤 해소한다(이중 회전).
- **사전**: `node != nullptr`. 두 자식 부분트리는 이미 AVL, `node` 자신만 한 번의
  삽입으로 최대 ±2까지 어긋났을 수 있다.
- **사후**: `node` 위치를 `|balanceFactor| ≤ 1`로 만든 부분트리의 새 루트 반환.
  높이 갱신. in-order 불변.
- **참고**: 삽입 직후 무거운 자식의 balance factor는 정확히 +1(바깥) 또는 −1(안쪽)이다
  (0일 수 없음). 그래서 안쪽 판정에 `< 0` / `> 0`을 쓴다.

```
rebalance(node):
    updateHeight(node)
    bf = balanceFactor(node)

    if bf > 1:                              // 왼쪽이 무거움
        if balanceFactor(node.left) < 0:    // LR: 왼자식이 오른쪽으로 기움
            node.left = rotateLeft(node.left)   // 먼저 LL 형태로
        return rotateRight(node)            // LL 해소

    if bf < -1:                             // 오른쪽이 무거움 (대칭)
        if balanceFactor(node.right) > 0:   // RL
            node.right = rotateRight(node.right)
        return rotateLeft(node)

    return node                             // 이미 균형
```

#### `AvlTreeNode* insert(AvlTreeNode* node, Data* elem)` — 재귀 삽입 헬퍼
반환값으로 (회전된) 부분트리 루트를 위에 전달하고, 경로를 거슬러 오르며 각 노드에서
`rebalance`가 호출된다.
- **사전**: `node`는 AVL 부분트리 루트이거나 `nullptr`. `elem != nullptr`이고 비교 가능.
- **사후**: `elem`을 담은 새 노드가 BST 위치에 삽입되고, 경로상 노드들이 재균형된
  부분트리의 **새 루트**를 반환. 결과는 AVL 불변 만족.
- **소유권**: leaf 자리에서 `new AvlTreeNode(elem)`로 노드 1개 생성(트리 소유).
  `elem`은 비소유로 보관만.

```
insert(node, elem):
    if node == nullptr:
        return new AvlTreeNode(elem)        // leaf, height=1

    if *elem < *node.data:
        node.left = insert(node.left, elem)
    else if *node.data < *elem:
        node.right = insert(node.right, elem)
    else:
        // 같은 키 — 중복 정책 미결(아래 [[#미결]]). 잠정: 오른쪽으로.
        node.right = insert(node.right, elem)

    return rebalance(node)
```

---

## 미결

- **중복 키 정책**: 같은 키 삽입 시 동작(거부 / 덮어쓰기 / 중복 허용)은 사용처인
  [[reservation_table]]의 Interval 키 설계에 종속된다. 그 가지를 내려갈 때 확정.
  현재 의사코드는 잠정으로 오른쪽 삽입. → [[work_log#미해결]].
- **remove**: 사용처(재계획)가 요구하면 추가. 삭제 후 재균형은 `rebalance`를 경로에
  재사용한다.
