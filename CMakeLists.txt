cmake_minimum_required(VERSION 3.16)

project(Tournament VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

file(GLOB SOURCES
    "*.cpp"
    "src/*.cpp"
)

file(GLOB HEADERS
    "headers/*.h"
    "headers/*.hpp"
)

file(GLOB UI_FILES
    "ui/*.ui"
)

# Создать исполняемый файл
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# Установить свойства цели для UIC
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui"
)

# Include директории
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/external  
    ${CMAKE_CURRENT_SOURCE_DIR}/build/Tournament_autogen/include    
    ${CMAKE_CURRENT_BINARY_DIR}              
)

# Связать с библиотеками Qt
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
)

# Для правильной работы UIC
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_NO_KEYWORDS
)

file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
