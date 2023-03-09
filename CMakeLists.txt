cmake_minimum_required(VERSION 3.5)

project(my_program)


set(CMAKE_CXX_STANDARD 14)


find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

include_directories(include ${Boost_INCLUDE_DIRS})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)

set(SOURCES
    main.cpp
    utils/similarity.cpp
    utils/search.cpp
    include/nlohmann/json.hpp
    store/store_data.cpp)

add_executable(my_program ${SOURCES})

target_link_libraries(my_program
    ${Boost_LIBRARIES}
    -pthread
)
target_include_directories(my_program PRIVATE utils include)
