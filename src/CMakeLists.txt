
add_library(${LIB_NAME} STATIC
    ${CMAKE_SOURCE_DIR}/src/core/bus.cpp
    
    ${CMAKE_SOURCE_DIR}/src/devices/W65C02S/W65C02S.cpp
    ${CMAKE_SOURCE_DIR}/src/devices/EEPROM28C256/EEPROM28C256.cpp
    ${CMAKE_SOURCE_DIR}/src/devices/SRAM62256/SRAM62256.cpp
    ${CMAKE_SOURCE_DIR}/src/devices/ArduinoMega/ArduinoMega.cpp
)

target_include_directories(${LIB_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${LIB_NAME} PRIVATE 
    spdlog
)


add_executable(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    # Components
    ${LIB_NAME}

    spdlog
)