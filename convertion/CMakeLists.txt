cmake_minimum_required(VERSION 3.20)

project(convert)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -O2")

set(ENTRY_POINT
               "main.cpp")
set(HEADERS_ROOT_PATH
                     "${convert_SOURCE_DIR}/include")
set(SRC_ROOT_PATH
                 "${convert_SOURCE_DIR}/src")
file(
    GLOB_RECURSE SOURCE_LIST
    "${SRC_ROOT_PATH}/*.c*"
    "${SRC_ROOT_PATH}/*.h*"
)

file(
    GLOB_RECURSE HEADERS_LIST
    "${HEADERS_ROOT_PATH}/*.h*"
)

add_executable(
              ${PROJECT_NAME}
              ${ENTRY_POINT}
              ${SOURCE_LIST}
              ${HEADERS_LIST}
)

source_group(
            TREE "${SRC_ROOT_PATH}"
            PREFIX "Source Files"
            FILES ${SOURCE_LIST}
)
source_group(
            TREE "${HEADERS_ROOT_PATH}"
            PREFIX "Header Files"
            FILES ${HEADERS_LIST}
)
