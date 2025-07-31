cmake_minimum_required(VERSION 3.22)

project(EaterEmulator LANGUAGES CXX)
set(LIB_NAME EaterEmulatorCore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

include(FetchContent)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.15.3 
)

FetchContent_MakeAvailable(spdlog)

add_subdirectory(src)
if (${BUILD_TESTS})
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    include(GoogleTest)
    enable_testing()
    target_compile_definitions(${LIB_NAME} PRIVATE UNIT_TEST)
    add_subdirectory(tests)
endif()