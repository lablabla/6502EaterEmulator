# CMakeLists.txt for instruction tests
file(GLOB INSTRUCTION_TESTS "*.cpp")

add_executable(cpu_instruction_tests ${INSTRUCTION_TESTS})
target_link_libraries(cpu_instruction_tests PRIVATE ${LIB_NAME} gtest_main)
target_include_directories(cpu_instruction_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_definitions(cpu_instruction_tests PRIVATE UNIT_TEST)
add_test(NAME cpu_instruction_tests COMMAND cpu_instruction_tests)
