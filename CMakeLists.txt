cmake_minimum_required(VERSION 3.21)
project(project2)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread")
add_executable(project2 main.cpp TaskQueue.cpp TaskQueue.h ThreadPool.cpp ThreadPool.h)
