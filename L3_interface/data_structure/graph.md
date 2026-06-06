---
layer: L3
tags: [interface, ds]
---

# graph (유향 그래프 + 시간 확장)

## 책임
OHT layout을 **유향(directed) 그래프**로 표현한다. 인접 리스트로 각 정점의
나가는 간선을 보관하고, 이웃 순회를 효율적으로 제공한다. `Graph`(기본)와
이를 상속한 `TimeExpandedGraph`(TEG) 두 단으로 구성된다.

**아는 것:**
- 정점은 정수 인덱스(0..N-1)로 식별된다.
- 각 정점에서 나가는 간선들(유향) — `Graph`로부터 이웃을 순회하는 법.
- (TEG) 정점을 `(node, time)`으로 확장해 시간축을 더하는 법, 그리고 점유된
  `(node, time)`을 제거하는 법.

**하지 않는 것:**
- 경로를 찾지 않는다 — 탐색(BFS/ϕ-BF)은 planner의 일이다. graph는 구조만 준다.
- layout 데이터를 *어디서* 읽는지 모른다 — 파일 로딩은 사용처(environment의
  layout 로더)가 한다. graph는 "채워질 준비"만 한다.
- 충돌·reservation 의미를 모른다 — 어떤 `(node,time)`이 점유됐는지는 planner가
  알려주고, TEG는 그것을 받아 정점에서 빼기만 한다.

> 참고: 다른 자료구조와 달리 graph는 `Data*`를 담는 범용 컨테이너가 아니다.
> 정점=정수 인덱스, 간선=정수(이웃 인덱스)라 다형성·힙할당이 불필요하다.
> "모든 자료구조는 Data 컨테이너"의 **명시적 예외**다.

## 구현 방식

### Graph (기본 클래스)
유향 그래프. 인접 리스트로 표현한다.

모든 그래프는 **unweighted**다(간선 길이 = 1). 거리는 전처리에서 가상노드로
펼쳐져 흡수되므로(`data/edges_expanded.txt`), graph가 weight를 들 필요가 없다.

**필드** (private):
- `int n` — 정점 수.
- 인접 리스트 — 정점별 나가는 간선 목록. `dynamic_array`(정점 수만큼) ×
  각 칸이 그 정점의 이웃 인덱스 목록(`dynamic_array<int>`).

**간선 표현**: 이웃 = `to_idx`(정수) 하나. 모든 간선 길이 1이라 weight를
저장하지 않는다.

**methods**:
- 생성자 `Graph(int n)` — 정점 n개, 간선 없음.
- `void addEdge(int from, int to)` — 단위 간선 추가.
- `const 이웃목록& neighbors(int v) const` — 정점 v의 나가는 간선들. 순회용.
- `int size() const` — 정점 수.

### TimeExpandedGraph (Graph 상속)
기본 `Graph`(unweighted 확장본)를 시간축으로 확장한 그래프. BFS+TEG가 쓴다.

**확장 방식**: 정점이 `(node, time)` 쌍이 된다. 전이는 두 가지:
- **이동**: `(u, t) → (v, t+1)`  — 기본 그래프에 간선 `u→v`가 있을 때.
- **대기**: `(u, t) → (u, t+1)`  — 제자리 한 스텝.

시간은 `0..H`(horizon)까지. 모든 간선이 길이 1이라 시간 1스텝 = 간선 1개로
일치한다(unweighted 확장본을 쓰는 이유).

**reservation 흡수**: 점유된 `(node, time)` 정점을 TEG에서 **제거**한다.
node x가 시각 t에 사용 불가면 `(x, t)`로의 전이를 막는다. 이로써 충돌 회피가
그래프 구조에 구워지고, 그 위 탐색은 reservation을 모른 채 순수 BFS가 된다.
점유 정보는 planner가 reservation_table에서 읽어 TEG 빌드 시 넘긴다.

**TEG를 물질화할지 여부**(명시적 vs 암묵적)와 그 메모리·시간 비용은 planner
가지에서 확정한다. horizon H=28 제한 시 BFS는 시작점 H-hop 반경만 방문하므로
한 에이전트 탐색은 가볍다(자세한 분석은 [[work_log]]).

**소유권**: graph는 정수만 담으므로 `Data*` 비소유 정책의 대상이 아니다.
내부 인접 리스트(dynamic_array)는 graph가 소유·해제한다.


## Time Complexity
- 간선 추가(addEdge): 분할상환 O(1)
- 이웃 순회(neighbors): O(deg(v)) — 희소 그래프라 작음
- 전체 순회: O(V + E)
- TEG 빌드: O((V + E) × H) — 시간층 H배

## 의존성
### 1. 의존
- [[dynamic_array]] — 인접 리스트의 저장소.
### 2. 의존됨
- planner — ϕ-BF는 기본 `Graph`, BFS+TEG는 `TimeExpandedGraph`.
  상세는 [[data_structure_design#사용처]].

## 사용처
- **ϕ-BF**: 기본 `Graph`(unweighted 확장본). 모든 간선 길이 1이므로 한 홉 =
  한 스텝으로 ϕ를 전파한다.
- **BFS+TEG**: 같은 확장본을 `TimeExpandedGraph`로 시간 확장해 탐색.
- 둘 다 `data/edges_expanded.txt`(unweighted 확장본)에서 로드된다. 로딩
  주체는 environment의 layout 로더.

선정 근거는 [[data_structure_design#선정 근거]].

## 미결
- TEG 물질화 전략(명시적/암묵적) — planner 가지에서.
- 가상노드 ↔ 원본 노드/간선 매핑 추적이 필요한지 — 경로 복원·시뮬레이션 시
  확정.
