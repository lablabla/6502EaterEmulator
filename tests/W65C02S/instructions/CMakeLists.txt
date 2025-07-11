# CMakeLists.txt for instruction tests
file(GLOB INSTRUCTION_TESTS "*.cpp")

set(TEST_NAME W65C02S_instruction_tests)

add_executable(${TEST_NAME} ${INSTRUCTION_TESTS})
target_link_libraries(${TEST_NAME} PRIVATE ${LIB_NAME} spdlog gtest_main)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_definitions(${TEST_NAME} PRIVATE UNIT_TEST)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})