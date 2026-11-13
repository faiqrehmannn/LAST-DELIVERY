cmake_minimum_required(VERSION 3.20)
project(LastDelivery LANGUAGES CXX)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SFML)

add_executable(LastDelivery main.cpp)
target_compile_features(LastDelivery PRIVATE cxx_std_11)
target_compile_definitions(LastDelivery PRIVATE SFML_STATIC)
target_link_libraries(LastDelivery PRIVATE sfml-graphics sfml-window sfml-system)
