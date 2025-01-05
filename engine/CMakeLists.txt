cmake_minimum_required(VERSION 3.10)
project(TheyStillSing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Find all asset headers
file(GLOB_RECURSE ASSET_HEADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/**/*.hpp
)

# Source files
set(SOURCES 
    src/main.cpp
    src/states/WarningState.cpp
    ${ASSET_HEADERS}
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link SFML
target_link_libraries(${PROJECT_NAME} 
    sfml-graphics 
    sfml-window 
    sfml-system
)
