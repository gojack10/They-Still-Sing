cmake_minimum_required(VERSION 3.10)
project(TheyStillSing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add debug symbols and stack trace support
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -rdynamic")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

# Enable backtracing
add_definitions(-DBACKTRACE_ENABLE)

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
    src/states/MainMenuState.cpp
    src/systems/animation/Animation.cpp
    src/systems/animation/AnimationManager.cpp
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

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMENT "Copying assets to build directory..."
)

# Set the working directory for the target
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
