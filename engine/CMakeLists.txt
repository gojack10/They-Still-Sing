cmake_minimum_required(VERSION 3.10)
project(TheyStillSing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add debug symbols and stack trace support
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -rdynamic")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

# Enable backtracing
add_definitions(-DBACKTRACE_ENABLE)

# Include FetchContent for downloading dependencies
include(FetchContent)

# Add nlohmann_json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

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
    src/utils/UIScaler.hpp
    ${ASSET_HEADERS}
    src/ui/MenuHitbox.cpp
    src/ui/MenuManager.cpp
    src/systems/ui/ScalingManager.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    sfml-graphics 
    sfml-window 
    sfml-system
    nlohmann_json::nlohmann_json
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
