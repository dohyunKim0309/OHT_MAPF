#include "data.h"

#include <cassert>

// 빈 소멸자지만 .cpp 에 정의를 둔다 (key function).
// vtable 을 이 번역 단위 하나에 고정해 중복 생성과 링크 문제를 막는다.
Data::~Data() {}

// operator< 를 override 하지 않은 Data 를 비교하는 것은 계약 위반이다.
// false 로 조용히 삼키지 않고 assert 로 즉시 드러낸다.
// 릴리스 빌드(NDEBUG)에서는 assert 가 제거되어 false 를 반환한다.
bool Data::operator<(const Data& /*other*/) const {
    assert(false && "operator< called on a Data without comparison support");
    return false;
}
