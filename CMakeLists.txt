cmake_minimum_required(VERSION 4.0)

# Declare our project
project(FallingBlocks CXX)

# CMake settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(BOOST_INCLUDE_LIBRARIES serialization core)
set(BOOST_ENABLE_CMAKE ON)


include(FetchContent)

# Fetch Boost Library
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.88.0
)
FetchContent_MakeAvailable(Boost)

# Fetch SFML Library
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Add our executable
add_executable(FallingBlocks main.cpp)
add_subdirectory(src)

# Link SFML library
target_link_libraries(FallingBlocks PUBLIC SFML::Graphics SFML::Audio)

# Link Boost libraries
target_link_libraries(FallingBlocks PUBLIC Boost::serialization Boost::core)

# Link our header files
target_include_directories(FallingBlocks PRIVATE ${PROJECT_SOURCE_DIR}/include)


# Define our assets folder, and assets folder "copy" (actually a link)
set(assetsDir "${CMAKE_SOURCE_DIR}/assets")
set(assetsCopy "${CMAKE_BINARY_DIR}/bin/assets")

# Now we have to link our assets, but copying them wouldn't be extremely convenient
if(UNIX)
    # Create a symlink to the actual assets directory
    add_custom_target(link_assets_target ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ${assetsDir} ${assetsCopy})
else()
    # Convert to Windows path (with backslashes)
    file(TO_NATIVE_PATH assetsCopy winAssetsCopy)
    file(TO_NATIVE_PATH assetsDir winAssetsDir)
    # Do a directory junction (requires no access privileges) because Windows is fucking annoying
    execute_process(COMMAND cmd.exe /c mklink /J ${winAssetsCopy} ${winAssetsDir})
endif()