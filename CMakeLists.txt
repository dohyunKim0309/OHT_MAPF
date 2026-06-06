cmake_minimum_required(VERSION 4.2)
project(OHT_MAPF)

set(CMAKE_CXX_STANDARD 20)

add_executable(OHT_MAPF L5_implementation/main.cpp
        L5_implementation/data_structure/data.cpp
        L5_implementation/data_structure/data.h
        L5_implementation/data_structure/node.cpp
        L5_implementation/data_structure/node.h
        L5_implementation/data_structure/dynamic_array.cpp
        L5_implementation/data_structure/dynamic_array.h
        L5_implementation/data_structure/linked_list.cpp
        L5_implementation/data_structure/linked_list.h
        L5_implementation/data_structure/queue.cpp
        L5_implementation/data_structure/queue.h
        L5_implementation/data_structure/avl_tree.cpp
        L5_implementation/data_structure/avl_tree.h
        L5_implementation/data_structure/min_heap.cpp
        L5_implementation/data_structure/min_heap.h)
