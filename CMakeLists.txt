cmake_minimum_required(VERSION 3.22)

project(EaterEmulator LANGUAGES CXX)
set(LIB_NAME EaterEmulatorCore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Third party fetches
# Include the FetchContent module
include(FetchContent)

# Define the spdlog repository details
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.13.0 # or a specific commit hash
)

# Populate the spdlog content
FetchContent_MakeAvailable(spdlog)

add_subdirectory(src)

if (${BUILD_TESTS})   
    # GoogleTest
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    target_compile_definitions(${LIB_NAME} PRIVATE UNIT_TEST)
    add_subdirectory(tests)
endif()