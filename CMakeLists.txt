cmake_minimum_required(VERSION 3.11)

project(Liberty)

set(CMAKE_CXX_STANDARD 14)
find_package(Boost 1.60)

add_library(${PROJECT_NAME} LibertyLibrary.cpp)

target_link_libraries(${PROJECT_NAME} Boost::boost)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

option(BUILD_TEST "Build test" ON)

if(BUILD_TEST)
    Include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2
        GIT_TAG v2.13.8
    )

    FetchContent_MakeAvailable(Catch2)

    add_executable(tests test.cpp)
    target_link_libraries(tests PRIVATE
        Catch2::Catch2
        ${PROJECT_NAME}
    )

    target_compile_definitions(${PROJECT_NAME} PUBLIC LIBERTY_TEST_RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test")
endif()
