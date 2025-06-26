

add_executable(${PROJECT_NAME} 
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

add_subdirectory(cpu)