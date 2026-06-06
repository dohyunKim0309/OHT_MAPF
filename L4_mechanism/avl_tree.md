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
- **사후**: `elem`의 키가 트리에 없으면 BST 위치에 삽입되고 AVL 불변(`|bf| ≤ 1`) 유지.
  **이미 있으면 삽입하지 않고 트리 불변**(중복 거부).
- **소유권**: 삽입 시 노드 1개 생성(트리 소유), `elem`은 비소유로 보관. 미삽입 시
  `elem`의 소유권은 호출자에 그대로 남는다.

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

#### `Data* predecessor(Data* key) const` — in-order 직전 (key 미존재 허용)
- **사전**: `key != nullptr`이고 비교 가능. `key`는 트리에 있어도/없어도 됨.
- **사후**: `key`보다 작은(`*x < *key`) 키 중 **최대**인 `Data*` 반환, 없으면
  `nullptr`. 트리 불변.
- **소유권**: 읽기 전용.
- **방법**: 루트에서 내려가며 "현재보다 작은 후보"를 갱신. `cur.data < key`이면
  cur이 후보가 되고 오른쪽으로(더 큰 작은 값 찾기), 아니면 왼쪽으로.

```
predecessor(key):
    cur = root; best = nullptr
    while cur != nullptr:
        if *cur.data < *key:           // cur < key → 후보, 더 큰 걸 우측에서
            best = cur.data
            cur = cur.right
        else:                          // cur >= key → 좌측으로
            cur = cur.left
    return best
```

#### `Data* successor(Data* key) const` — in-order 직후 (대칭)
- **사전/사후**: predecessor의 대칭. `key`보다 큰 키 중 **최소** `Data*`, 없으면 `nullptr`.
- **소유권**: 읽기 전용.

```
successor(key):
    cur = root; best = nullptr
    while cur != nullptr:
        if *key < *cur.data:           // cur > key → 후보, 더 작은 걸 좌측에서
            best = cur.data
            cur = cur.left
        else:                          // cur <= key → 우측으로
            cur = cur.right
    return best
```

#### `void remove(Data* key)` — 삭제 진입점
- **사전**: `key != nullptr`이고 비교 가능.
- **사후**: `key`와 동치인 키 노드가 있으면 삭제되고 경로가 재균형되어 AVL 불변
  유지. 없으면 트리 불변.
- **소유권**: 삭제된 노드(`AvlTreeNode`)는 트리가 해제. 담은 `Data*`는 **비소유**
  — delete하지 않는다(호출자가 받아 처리). 자식 둘인 노드는 successor의 `Data*`로
  *내용만* 대체하고 successor 노드를 삭제하므로, 원래 노드의 `Data*`는 호출자
  책임으로 남는다.

```
remove(key):
    root = removeNode(root, key)     // 아래 private 재귀 헬퍼
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
- **사후**: `elem`의 키가 없으면 새 노드가 BST 위치에 삽입되고, 경로상 노드들이 재균형된
  부분트리의 **새 루트**를 반환. **같은 키가 이미 있으면 삽입하지 않고 부분트리를 그대로
  반환**. 결과는 AVL 불변 만족.
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
        return node                          // 같은 키 — 추가하지 않음(부분트리 불변)

    return rebalance(node)
```

#### `AvlTreeNode* findMin(AvlTreeNode* node)` — 부분트리 최소 노드
- **사전**: `node != nullptr`.
- **사후**: 가장 왼쪽 노드(부분트리 최소 키)를 반환. 트리 불변.

```
findMin(node):
    while node.left != nullptr: node = node.left
    return node
```

#### `AvlTreeNode* removeNode(AvlTreeNode* node, Data* key)` — 재귀 삭제 헬퍼
표준 BST 삭제 + 경로 재균형. 자식 0/1개면 직접 잇고, 2개면 successor(우측
부분트리 최소)의 **내용(`Data*`)으로 대체** 후 그 successor 노드를 우측에서 삭제.
- **사전**: `node`는 AVL 부분트리 루트이거나 `nullptr`. `key != nullptr`.
- **사후**: `key` 동치 노드를 제거(없으면 불변)하고 재균형된 부분트리 **새 루트**
  반환. AVL 불변 유지.
- **소유권**: 제거되는 `AvlTreeNode`만 `delete`(트리 소유). 담은 `Data*`는 비소유라
  손대지 않는다. 자식 둘 케이스에서 대체는 successor의 `Data*` *포인터 복사*뿐 —
  실제 삭제되는 노드는 successor 노드이고, 그 `Data*`는 이미 대체로 위에 올라갔다.

```
removeNode(node, key):
    if node == nullptr: return nullptr           // 없음 — 불변

    if *key < *node.data:
        node.left = removeNode(node.left, key)
    else if *node.data < *key:
        node.right = removeNode(node.right, key)
    else:                                        // 동치 — 이 노드 삭제
        if node.left == nullptr or node.right == nullptr:
            child = (node.left != nullptr) ? node.left : node.right  // 0 또는 1
            delete node                          // 노드만; Data*는 비소유
            return child                         // child(또는 nullptr)로 대체
        else:
            succ = findMin(node.right)           // 우측 최소 = in-order 직후
            node.data = succ.data                // 내용만 대체(포인터 복사)
            node.right = removeNode(node.right, succ.data)  // succ 노드 삭제

    return rebalance(node)                        // 경로 거슬러 재균형
```

> 주의: 자식 둘 케이스에서 `node.data`를 succ.data로 덮으면 원래 노드가 담던
> `Data*`는 트리에서 사라진다. remove는 비소유라 그것을 delete하지 않으므로,
> **호출자가 remove 전에 그 `Data*`를 확보**해야 한다(reservation 병합은 흡수 전
> predecessor/successor로 이미 그 포인터를 들고 있다).

---

## 결정된 정책
- **중복 키**: 같은 키는 삽입하지 않는다(거부). 한 정점의 동일 시간 구간 키는 유일하므로.
  (2026-06-06)
- **predecessor/successor/remove 추가**: reservation_table의 구간 병합이 좌·우 이웃을
  찾아 흡수(삭제)해야 하므로 추가. predecessor/successor는 key 미존재도 허용(삽입 전
  이웃 확인). remove는 비소유 — 호출자가 `Data*`를 책임진다. (2026-06-06)
