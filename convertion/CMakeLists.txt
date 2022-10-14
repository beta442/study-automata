cmake_minimum_required(VERSION 3.20)

project(convert CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(ENTRY_POINT
               "main.cpp")
set(HEADERS_ROOT_PATH
                     "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_ROOT_PATH
                 "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(
    GLOB_RECURSE SOURCE_LIST
    "${SRC_ROOT_PATH}/*.c*"
    "${SRC_ROOT_PATH}/*.h*"
)

file(
    GLOB_RECURSE HEADERS_LIST
    "${HEADERS_ROOT_PATH}/*.h*"
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(
              ${PROJECT_NAME}
              ${ENTRY_POINT}
              ${SOURCE_LIST}
              ${HEADERS_LIST}
)

if(MSVC)
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
endif()
