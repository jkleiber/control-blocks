
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ================= Boost ===============
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Boost is installed platform-specific
if(WIN32)
    set(BOOST_ROOT "/boost_1_80_0/boost")
    set(Boost_INCLUDE_DIR "/boost_1_80_0/")
    set(Boost_LIBRARY_DIR "/boost_1_80_0/lib")
endif(WIN32)

find_package(Boost 1.80 REQUIRED)

# ============ Control Blocks ================

# Note that headers are optional, and do not affect add_library, but they will
# not show up in IDEs unless they are listed in add_library.
file(GLOB HEADER_LIST
    CONFIGURE_DEPENDS
    "${ControlBlocks_SOURCE_DIR}/include/controlblocks/*.h")

file(GLOB SOURCE_LIST
    CONFIGURE_DEPENDS
    "${ControlBlocks_SOURCE_DIR}/src/*.cpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(controlblocks_lib STATIC ${SOURCE_LIST} ${HEADER_LIST})
set_target_properties(controlblocks_lib PROPERTIES LINKER_LANGUAGE CXX)

# HACK: Remove when Boost fixes odeint deprecation warnings
target_compile_options(controlblocks_lib PRIVATE -Wno-deprecated)

# We need the top level include directory as well as the imgui ones.
get_target_property(IMGUI_INCLUDES IMGUI INCLUDE_DIRECTORIES)

if(WIN32)
    get_target_property(EIGEN_INCLUDES Eigen3::Eigen INCLUDE_DIRECTORIES)
    target_include_directories(controlblocks_lib
        PUBLIC
        ../include
        ${IMGUI_INCLUDES}
        ${EIGEN_INCLUDES}
        ${Boost_INCLUDE_DIRS})
else()
    target_include_directories(controlblocks_lib
        PUBLIC
        ../include
        ${IMGUI_INCLUDES}
        ${Boost_INCLUDE_DIRS})
endif(WIN32)

# Add all the packages that need to be linked.
target_link_libraries(controlblocks_lib
    PUBLIC
    Eigen3::Eigen
    IMGUI
    IMPLOT
    IMNODES
    tomlplusplus_tomlplusplus
    ${Boost_LIBRARIES}
    ImGuiColorTextEdit
    NFD)

# All users of this library will need at least C++17
target_compile_features(controlblocks_lib PUBLIC cxx_std_17)

# IDEs should put the headers in a nice place
source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST})
