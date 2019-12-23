set(TEST_NAME StringFragmentatorUnitTests)

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/tests/*.cpp)

add_executable(${TEST_NAME} ${TEST_SRC_FILES})

target_link_libraries(${TEST_NAME} gtest gtest_main)
target_link_libraries(${TEST_NAME} StringFragmentatorLibs)
