
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

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

# We need the top level include directory as well as the imgui ones.
get_target_property(IMGUI_INCLUDES IMGUI INCLUDE_DIRECTORIES)

if(WIN32)
    get_target_property(EIGEN_INCLUDES Eigen3::Eigen INCLUDE_DIRECTORIES)
    target_include_directories(controlblocks_lib PUBLIC ../include ${IMGUI_INCLUDES} ${EIGEN_INCLUDES})
else()
    target_include_directories(controlblocks_lib PUBLIC ../include ${IMGUI_INCLUDES})
endif(WIN32)

# This depends on imgui
target_link_libraries(controlblocks_lib PUBLIC Eigen3::Eigen IMGUI IMPLOT IMNODES)

# All users of this library will need at least C++17
target_compile_features(controlblocks_lib PUBLIC cxx_std_17)

# IDEs should put the headers in a nice place
source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST})
