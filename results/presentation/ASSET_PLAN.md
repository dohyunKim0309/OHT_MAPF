# 발표 시각 자료 계획 (10분 영어 발표, ~12 슬라이드)

각 슬라이드에 필요한 시각 자료를 전수 식별하고, **누가 만드는지**와 **정확한 사양**을
확정한다. 제작 후 이 표의 status를 갱신한다.

범례 — Owner: **AI**(내가 제작) / **YOU**(직접 캡처·제공) / **DONE**(이미 있음)

| # | 슬라이드 | 필요 시각 자료 | Owner | 사양 / 출처 | status |
|---|---|---|---|---|---|
| 1 | Title | (텍스트만) | AI | 제목·이름(김도현/202214223)·한 줄 요약 | todo |
| 2 | The problem | fab OHT 개념도 또는 사진 | YOU/AI | 유튜브/논문의 OHT fab 이미지(저작권 주의) 또는 AI 단순 모식도 | todo |
| 3 | Data acquisition | **논문 1p 캡처** + **GitHub repo 캡처** | AI(논문) / YOU(repo) | 논문 PDF 1p→PNG; repo는 JS라 직접 캡처 | todo |
| 4 | Problem setup | **양자화 옵션 테이블** + 가상노드 모식도 | AI | data/PREPROCESSING.md §5 표; 간선→가상노드 펼침 그림 | todo |
| 5 | Why time matters | **그림 A**(공간만→충돌, 시간축 필요) | AI | 작은 그래프, 두 에이전트 동시 점유=충돌 → 시간 빠짐 | todo |
| 6 | Baseline BFS+TEG | **그림 B-1**(시간 복제 격자 + isFree로 점유칸 제거) | AI | (node,t) 격자, 점유 칸 X, BFS 경로. V×T 강조 | todo |
| 7 | Proposed ϕ-BF | **그림 B-2**(확장 없이 phi 시간 점프) | AI | 노드 하나 + [a,b) 점유 → phi(t)=b 점프. B-1과 나란히 대비 | todo |
| 8 | Data structures | **자료구조 WHERE/HOW/WHY 표** | AI | graph/AVL/min-heap/queue/dyn-array + stack 미채택 | todo |
| 9 | Reservation 핵심 | reserve 병합 + isFree/phi/phiWindow 그림 | AI | AVL 구간 병합 [2,4)+[4,7)→[2,7), dwell 창 | todo |
| 10 | Code | **핵심 코드 발췌**(findPath/phi/recordPath) | AI | 코드 캡처 또는 슬라이드 코드블록(영어 주석) | todo |
| 11 | Results | **compare 실행 스크린샷**(H2000) + 비교 표 | YOU(스샷) / AI(표) | 132배·throughput; 같은 H 도달범위 차이 | todo |
| 12 | Conclusion | (텍스트 + 핵심 그림 1개 재활용) | AI | ϕ-BF 제안·검증, 자료구조가 효율 가른다 | todo |

## 내가(AI) 만들 것
- 논문 1p PNG 캡처 (data/ PDF에서)
- 양자화 옵션 테이블 (이미지 또는 슬라이드 표)
- **그림 A** — 왜 시간축이 필요한가 (충돌 모식도)
- **그림 B-1 / B-2** — TEG 격자 vs ϕ 시간점프 (핵심 대비)
- 자료구조 WHERE/HOW/WHY 표
- reservation 병합·질의 그림
- 핵심 코드 발췌 (슬라이드용)
- 가상노드 펼침 모식도
- 최종 .pptx 조립

## 사용자(YOU)가 가져올 것
- **GitHub repo 페이지 스크린샷** (JS 렌더링이라 직접 캡처가 확실)
- **compare 실행 스크린샷** (H2000 — 지금 돌리는 중)
- (선택) fab OHT 실제 사진/영상 캡처 — 저작권 OK인 것만

## 이미 있음 (DONE, 재활용 가능)
- AVL 회전 그림 5개 (L3_interface/data_structure/assets/) — 자료구조 슬라이드 보조
- 보고서 본문(영어) — 슬라이드 텍스트 출처

## 확정 필요 (제작 전)
- 슬라이드 테마/색(보고서와 통일? 다크/라이트?)
- 그림 A·B 스타일(손그림 느낌 vs 정밀 격자)
- 코드 발췌를 "스크린샷"으로 할지 "슬라이드 코드블록"으로 할지
