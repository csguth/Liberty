project(Liberty)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)
find_package(Boost 1.60 REQUIRED COMPONENTS system)

aux_source_directory(. SRC_LIST)
list(REMOVE_ITEM SRC_LIST "test.cpp")
add_library(${PROJECT_NAME} ${SRC_LIST} libertygrammar_private.h)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})


add_executable(${PROJECT_NAME}_test test.cpp)
include_directories(${PROJECT_NAME}_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/Catch/single_include/.)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})

add_definitions("-DLIBERTY_TEST_RESOURCE_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/test\"")
