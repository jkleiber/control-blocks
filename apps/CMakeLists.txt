# Compile the main executable
add_executable(controlblocks main.cpp)
target_compile_features(controlblocks PRIVATE cxx_std_17)

# HACK: Remove when Boost fixes odeint deprecation warnings
target_compile_options(controlblocks PRIVATE -Wno-deprecated)

target_link_libraries(controlblocks PRIVATE controlblocks_lib SDL2main)

# # Get the include directories for the target and print them out.
# get_target_property(CB_INCLUDES controlblocks_lib INCLUDE_DIRECTORIES)
# foreach(dir ${CB_INCLUDES})
# message("INCLUDE: ${dir} ")
# endforeach()