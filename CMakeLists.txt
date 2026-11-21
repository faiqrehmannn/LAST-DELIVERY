cmake_minimum_required(VERSION 3.20)
project(LastDelivery LANGUAGES CXX)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
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
FetchContent_GetProperties(SFML SOURCE_DIR SFML_SOURCE_DIR)

add_executable(LastDelivery main.cpp)
target_compile_features(LastDelivery PRIVATE cxx_std_11)
target_compile_definitions(LastDelivery PRIVATE SFML_STATIC)
target_link_libraries(LastDelivery PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# Keep images and other runtime files beside the executable. This lets the
# game find assets/ when it is launched from the build folder.
add_custom_command(
    TARGET LastDelivery POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:LastDelivery>/assets"
)

# SFML Audio needs OpenAL when the Windows game starts.
if(WIN32 AND SFML_BUILD_AUDIO)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SFML_OPENAL_DLL "${SFML_SOURCE_DIR}/extlibs/bin/x64/openal32.dll")
    else()
        set(SFML_OPENAL_DLL "${SFML_SOURCE_DIR}/extlibs/bin/x86/openal32.dll")
    endif()

    add_custom_command(
        TARGET LastDelivery POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SFML_OPENAL_DLL}"
                "$<TARGET_FILE_DIR:LastDelivery>"
    )
endif()
