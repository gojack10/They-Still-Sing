cmake_minimum_required(VERSION 3.10)
project(TheyStillSing)

# Enable ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Optimize build speed
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Disable verbose output
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add debug symbols and stack trace support
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -rdynamic")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

# Enable backtracing
add_definitions(-DBACKTRACE_ENABLE)

# Download CPM.cmake
set(CPM_DOWNLOAD_VERSION 0.38.6)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake...")
    file(DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
        ${CPM_DOWNLOAD_LOCATION}
    )
endif()
include(${CPM_DOWNLOAD_LOCATION})

# Add nlohmann_json using CPM
CPMAddPackage(
    NAME nlohmann_json
    VERSION 3.11.2
    GITHUB_REPOSITORY nlohmann/json
    OPTIONS 
        "JSON_BuildTests OFF"
)

# Find SFML with more detailed error reporting
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)
if(NOT SFML_FOUND)
    message(FATAL_ERROR "SFML not found. Please install SFML 2.5 or later.")
endif()

# Find all asset headers
file(GLOB_RECURSE ASSET_HEADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/**/*.hpp
)

# Source files
set(SOURCES 
    src/main.cpp
    src/states/WarningState.cpp
    src/states/MainMenuState.cpp
    src/states/OptionsState.cpp
    src/systems/animation/Animation.cpp
    src/systems/animation/AnimationManager.cpp
    src/systems/audio_systems/AudioSystem.cpp
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
    sfml-audio
    nlohmann_json::nlohmann_json
)

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)

# Set the working directory for the target
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
