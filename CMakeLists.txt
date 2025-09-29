cmake_minimum_required(VERSION 3.22)
project(cpp_p)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(bit_manipulation bit_manipulation.cpp)   # 有 main()
add_executable(queue_stack_demo queue_and_stack.cpp)    # 也有 main()
# add_executable(others_demo others.cpp)                # 依需求增加
