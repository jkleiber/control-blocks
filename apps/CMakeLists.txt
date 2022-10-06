# Compile the main executable
add_executable(controlblocks main.cpp)
target_compile_features(controlblocks PRIVATE cxx_std_17)

# Need to include python here as well
find_package(Python REQUIRED Development)

# HACK: Remove when Boost fixes odeint deprecation warnings
target_compile_options(controlblocks PRIVATE -Wno-deprecated)

target_link_libraries(controlblocks PRIVATE controlblocks_lib SDL2main ${Python_LIBRARIES})

if(WIN32)
    add_custom_command(TARGET controlblocks POST_BUILD # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different # which executes "cmake - E copy_if_different..."
        "${Python_LIBRARY_DIRS}/../python310.dll" # <--this is in-file
        "${PROJECT_SOURCE_DIR}/DLLs/")
endif(WIN32)

# # Get the include directories for the target and print them out.
# get_target_property(CB_INCLUDES controlblocks_lib INCLUDE_DIRECTORIES)
# foreach(dir ${CB_INCLUDES})
# message("INCLUDE: ${dir} ")
# endforeach()