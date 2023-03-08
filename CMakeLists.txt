cmake_minimum_required(VERSION 3.5)

project(my_program)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES
    main.cpp
    utils/similarity.cpp
    utils/search.cpp
    include/nlohmann/json.hpp
    store/store_data.cpp)

add_executable(my_program ${SOURCES})
target_include_directories(my_program PRIVATE utils include)
