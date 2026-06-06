# CLAUDE.md — L5 Implementation (코드 루트)

여기는 실제 C++ 코드의 루트다. 어떤 구성요소를 구현하기 전에 그 **계약**
([[L3_interface]])과 **기전**([[L4_mechanism]]) 문서를 읽는다. 구체 알고리즘은 L4
기전 문서가 의사코드로 정해두었다 — 코드는 그 충실한 번역이다.

## 주석
- **코드 주석은 영어로 쓴다.** (2026-06-06 통일)

## 코딩 컨벤션 (기존 코드 기준)
- 클래스: PascalCase (`AvlTree`, `LinkedList`, `AvlTreeNode`).
- 메서드: camelCase (`insert`, `getData`, `pushBack`).
- 헤더 가드: `OHT_MAPF_<NAME>_H`.
- 파일 상단 주석: 클래스 한 줄 요약 + 상위 문서 경로 — `Contract:`(L3) / `Mechanism:`(L4).
- 메모리: `new`/`delete`. 자료구조는 담은 `Data*`를 **비소유**로 다룬다 — 소멸자는
  노드만 해제하고 `Data*`는 delete 하지 않는다(수명은 도메인 소유자가 쥔다).
- 비교가 필요하면 `Data::operator<`에 위임한다(구체 Data 자식이 override).
- 사전조건은 `assert(cond && "message")`로 표현한다.

## 빌드
- C++20, CMake (CLion). 새 `.cpp`/`.h`는 `CMakeLists.txt`의 `add_executable(...)`
  목록에 추가한다.
