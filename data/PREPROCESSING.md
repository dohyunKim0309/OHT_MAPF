# SMAT 2022 OHT 네트워크 — 그래프 입력 전처리 정리

## 1. 데이터 출처

- **Repo**: [jja8989/visualization-for-OHT-network](https://github.com/jja8989/visualization-for-OHT-network)
- **경로**: `data/SMAT 2022/` → `SMAT2022_node.csv`, `SMAT2022_link.csv`
- port CSV는 토폴로지만 사용하므로 미포함.

## 2. 입력 포맷

| 파일 | 컬럼 | 사용 |
|---|---|---|
| node | (index), `ENCODED_ID`, `count` | `ENCODED_ID`만 사용. `count`(과거 통행량) 무시 |
| link | (index), `ENCODED_ID`, `ENCODED_FROM_NODE`, `ENCODED_TO_NODE`, `ENCODED_SPEED`, `ENCODED_DISTANCE` | `FROM→TO`(방향), `DISTANCE`만 사용. `SPEED`(전부 1) 무시 |

- 노드 2,858개 / 엣지 3,424개 / 순수 단방향(directed) 그래프.

## 3. 전처리 단계

1. **노드 ID 매핑** — `ENCODED_ID`(문자열) → 0부터 시작하는 정수 인덱스. node CSV의 등장 순서를 그대로 인덱스로 사용 → C++에서 문자열 파싱 없이 adjacency list로 직접 로드 가능.
2. **엣지 변환** — `FROM/TO`를 정수 인덱스로 치환, `DISTANCE`를 float로 보존.
3. **거리 → 이동 시간(스텝 수)** — V는 에이전트 속도(한 스텝당 거리). 상세는 §5.
   ```
   int_weight = max(1, round(distance / V))   # 그 간선을 건너는 타임스텝 수
   ```
   `max(1, ...)`로 zero-weight 엣지를 방지(0-cost 엣지는 무한 루프·시간층 붕괴 유발).
4. **unweighted 확장** (BFS+TEG용) — weight `w` 간선을 가상노드 `w-1`개로 펼쳐
   모든 엣지를 길이 1로 만든다(`from→v1→…→v(w-1)→to`). 가상노드는 "간선 위
   중간 시각의 위치"이며, 이로써 "최소 거리 충돌"이 "(node,time) 점유 충돌"로
   통일된다. `edges_expanded.txt`로 출력.
5. **토폴로지 검증** — 카운트·self-loop·역방향 쌍·중복 엣지·인덱스 범위 확인.

## 4. 선택 이유

| 결정 | 이유 |
|---|---|
| `SPEED` 무시 | 전 엣지 값이 1 → cost에 영향 없음 |
| `DISTANCE`만 weight | 단일 비용 척도로 충분, 그래프 단순화 |
| 정수 weight 양자화 | **TEG(Time-Expanded Graph)** 는 시간을 정수 단위로 이산화 → weight가 정수여야 시간층 인덱싱 가능 |
| `max(1, ...)` 클램핑 | 0-cost 엣지 제거 (탐색 정확성) |
| node CSV 순서 = 인덱스 | 재현성 + C++ 로딩 단순화 |

## 5. V의 의미 — 에이전트 속도, 그리고 horizon

**V는 양자화 상수가 아니라 에이전트의 *속도*다.** 한 타임스텝(Δt)에 에이전트는
거리 V만큼 전진하거나(이동) 제자리에 머문다(대기). 따라서

```
int_weight = max(1, round(distance / V)) = 그 간선을 건너는 데 걸리는 타임스텝 수
```

weight는 "거리의 양자화"가 아니라 **이동에 드는 시간(스텝 수)**이다. V를 키우면
에이전트가 빠른 것이고, 간선당 스텝 수(weight)가 줄어 TEG가 작아진다.

계획 **horizon H**는 한 번에 내다보는 타임스텝 수다. weight가 H를 넘는 간선은
horizon 안에 다 건너지 못하므로, **maxW ≤ H**가 되도록 V·H를 맞추면 모든
간선이 horizon 안에서 통과 가능하다.

### 오차는 두 종류 — 경로 오차가 진짜 척도

| V | maxW | 간선 평균오차 | 간선 최대오차 | **경로 평균오차** | 확장본 노드 |
|---:|---:|---:|---:|---:|---:|
| 1000 | 14 | 11.8% | 33.3% | **1.6%** | 10,312 |
| 500 (채택) | **28** | 7.1% | 20.0% | **3.6%** | 21,178 |
| 1417 | 10 | 16.8% | 60.7% | **2.8%** | 7,578 |
| 1800 | 8 | 31.3% | 104% | **8.1%** | 6,146 |

- **간선별 상대오차** `|w·V − d|/d`는 커 보이지만, **경로는 간선 여러 개의 합**이라
  올림/내림 오차가 상쇄된다. MAPF가 실제로 쓰는 건 경로 시간이므로 **경로 오차**가
  의미 있는 척도다 — 랜덤 노드쌍 200개의 최단경로로 측정.
- 상대오차 정의: 간선 `|int_weight·V − distance|/distance`, 경로 `|양자화경로 − 실경로|/실경로`.

### 채택: V = 500, H = 28

- **maxW = 28 = H** → 모든 간선을 horizon 안에 건넌다(H 초과 간선 0개).
- **경로 오차 3.6%** — 실제 fab 거리 구조가 경로 수준에서 거의 보존된다.
- 확장본 21,178 노드 — 메모리 부담 없음. 희소 그래프라 BFS+TEG는 horizon
  반경(수천 정점)만 방문해 한 에이전트 탐색이 가볍다.
- 1순위는 "전체 그래프로 계산이 잘 됨", 오차는 경로 3.6%로 수용 가능한 트레이드오프.

> 목표 우선순위: 전체 그래프 계산 가능 > horizon 내 간선 통과 > 거리 정밀도.
> 더 정밀히 원하면 V=1000/H=14(경로오차 1.6%)가 대안.

## 6. 검증 결과 (V=500)

| 항목 | 기대값 | 결과 |
|---|---|---|
| 노드 | 2,858 | ✅ |
| 엣지 | 3,424 | ✅ |
| self-loop | 0 | ✅ |
| 역방향 쌍 | 0 | ✅ |
| 중복 directed 엣지 | 0 | ✅ |
| 인덱스 범위 | 0 ~ 2,857 | ✅ |
| weight 범위 | ≥ 1 | ✅ (2 ~ 28) |
| `max(1,...)` 클램핑 발생 | 0건 | ✅ |
| 확장본 노드 | — | 21,178 (가상 18,320 + 원본 2,858) |
| 확장본 엣지 | — | 21,744 (전부 길이 1) |

## 7. 출력물 (`data/`)

| 파일 | 포맷 |
|---|---|
| `edges.txt` | 1행 `노드수 엣지수`, 이후 `from_idx to_idx int_weight` (공백 구분). **weighted 원본** — ϕ-BF가 사용 |
| `edges_expanded.txt` | 1행 `노드수 엣지수`, 이후 `from_idx to_idx` (weight 생략, 전부 1). **unweighted 확장본** — BFS+TEG가 사용. 가상노드는 원본(0~2857) 뒤 인덱스 |
| `node_map.csv` | `idx,ENCODED_ID` (인덱스 ↔ 원본 ID 역추적) |

재생성: `python3 data/data_preprocessor.py --V 500` · V 비교: `--scan`

## 8. 참고 논문

> ⚠️ 아래는 TEG 기반 MAPF의 **대표 문헌 후보**입니다. 본 repo/방법론 문서가 명시한 인용이 아니라 일반적으로 인용되는 논문이므로, 실제 인용 전 직접 확인을 권합니다.

- **Time-Expanded Graph / 흐름 기반 MAPF**
  J. Yu and S. M. LaValle, *"Multi-agent Path Planning and Network Flow,"* WAFR 2012/2013 — MAPF를 시간 확장 그래프 위의 네트워크 흐름 문제로 정식화.
- **Conflict-Based Search (CBS)**
  G. Sharon et al., *"Conflict-Based Search for Optimal Multi-Agent Pathfinding,"* Artificial Intelligence, 2015.
- **MAPF 서베이**
  R. Stern et al., *"Multi-Agent Pathfinding: Definitions, Variants, and Benchmarks,"* SoCS 2019.

> 정확한 인용 목록은 프로젝트 `L1_purpose/README` 등 방법론 문서를 확인하는 편이 안전합니다.
