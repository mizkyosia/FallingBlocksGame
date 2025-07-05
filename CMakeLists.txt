cmake_minimum_required(VERSION 3.16)

# Declare our project
project(SFMLTest CXX)

# CMake settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# Fetch SFML Library
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Add our executable
add_executable(SFMLTest main.cpp)
add_subdirectory(src)

# Link executable & SFML
target_link_libraries(SFMLTest SFML::Graphics)

# Link our header files
target_include_directories(SFMLTest PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Copy assets into build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin)
