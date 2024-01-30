cmake_minimum_required(VERSION 3.10)
project(horcrux)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++2a")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CXX_FILESYSTEM_LIBRARIES "stdc++fs")
endif()

find_package(GTest REQUIRED)

include_directories(
    ../include
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Add source files under test
set(UNIT_TEST_SRC_FILES "${${CMAKE_PROJECT_NAME}_SRC_FILES}")
# Exclude non-testable source files
list(REMOVE_ITEM UNIT_TEST_SRC_FILES EXCLUDE REGEX  "main[.]cpp")

add_executable(test_${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/horcrux_unit_test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_split_merge_tests.cpp
    ../src/file.cpp
    ../src/file_split_merge.cpp

    )

include(GoogleTest)
target_link_libraries(
    test_${CMAKE_PROJECT_NAME}
    pthread
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    GTest::GTest
    stdc++fs
)

target_include_directories(test_${CMAKE_PROJECT_NAME}
                           PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
 
gtest_discover_tests(test_${PROJECT_NAME})
