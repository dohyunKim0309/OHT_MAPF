#ifndef OHT_MAPF_DATA_H
#define OHT_MAPF_DATA_H

// Data: 타입 불문 객체 컨테이너.
// 자료구조와 빌딩 블록은 구체 타입을 모른 채 Data* 로 객체를 다룬다.
// 구체 Data 자식(Interval, OpenState, Agent ...)은 각 도메인 모듈에 위치한다.
// 설계 근거: L3_interface/data_structure/data.md
class Data {
public:
    // 다형적 삭제: Data* 로 자식을 delete 해도 동적 타입의 소멸자가 돈다.
    virtual ~Data();

    // 비교 계약의 선언. 비교가 필요한 자식(avl_tree, min_heap의 원소)만
    // override 하여 static_cast 후 필드를 비교한다.
    // override 하지 않은 채 호출하면 계약 위반 (구현은 data.cpp 참조).
    virtual bool operator<(const Data& other) const;
};

#endif // OHT_MAPF_DATA_H
