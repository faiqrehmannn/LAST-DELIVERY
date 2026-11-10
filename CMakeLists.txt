cmake_minimum_required(VERSION 3.10)
project(LastDelivery LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(LastDelivery main.cpp)
target_link_libraries(LastDelivery PRIVATE sfml-graphics sfml-window sfml-system)
