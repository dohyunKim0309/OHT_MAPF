//
// Created by DohyunKim on 26. 6. 6..
//

#ifndef OHT_MAPF_DYNAMIC_ARRAY_H
#define OHT_MAPF_DYNAMIC_ARRAY_H

#include "data.h"

// DynamicArray: Data* 를 연속 메모리에 저장하는 동적 배열.
// 인덱스 기반 임의 접근과 끝에서의 추가를 제공하고, 용량이 차면 스스로 확장. 저장한 Data* 는 비소유 — delete 하지 않는다.
// 설계 근거: L3_interface/data_structure/dynamic_array.md
class DynamicArray {
public:
    // 초기 용량으로 버퍼 할당
    explicit DynamicArray(int initialCapacity = 8);

    // 내부 buffer만 해제. 저장된 각 Data*는 delete 하지 않음.
    ~DynamicArray();

    // index 위치의 Data* 읽기
    Data* get(int index) const;

    // index 위치 Data 덮어쓰기 (0 <= index < size)
    void set(int index, Data* elem);

    // 끝에 Data 추가, 가득 차면 용량 확장 O(1)
    void pushBack(Data* elem);

    // 현재 저장된 개수
    int getSize() const;

private:
    // size==capacity에서 추가 시 더 큰 버퍼로 재할당(2배)
    void sizeUp();

    Data** buffer;            // Data* 들을 담는 연속 버퍼
    int size;
    int capacity;
};



#endif //OHT_MAPF_DYNAMIC_ARRAY_H
