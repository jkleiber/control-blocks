# =================== Eigen ====================
add_subdirectory(eigen3)

# =================== SDL2 ====================
set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SDL2)
add_subdirectory(${SDL2_DIR})

# =================== IMGUI ===================
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(IMGUI STATIC)

target_sources(IMGUI
    PRIVATE
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp

    PRIVATE
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl.cpp
)

target_include_directories(IMGUI
    PUBLIC ${IMGUI_DIR}
    PUBLIC ${IMGUI_DIR}/backends
    PUBLIC ${SDL2_DIR}/include
)

# OpenGL
find_package(OpenGL REQUIRED)

# Link the libraries for imgui
target_link_libraries(IMGUI PUBLIC SDL2-static ${CMAKE_DL_LIBS} ${OPENGL_LIBRARIES})

# =================== ImPlot ===================
set(IMPLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/implot)
add_library(IMPLOT STATIC)

target_sources(IMPLOT
    PRIVATE
    ${IMPLOT_DIR}/implot_demo.cpp
    ${IMPLOT_DIR}/implot_items.cpp
    ${IMPLOT_DIR}/implot.cpp
)

target_include_directories(IMPLOT
    PUBLIC ${IMPLOT_DIR}/
)

target_link_libraries(IMPLOT PUBLIC IMGUI)

# ============== ImNodes =======================
set(IMNODES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imnodes)
add_library(IMNODES STATIC)

target_sources(IMNODES
    PRIVATE
    ${IMNODES_DIR}/imnodes.cpp
)

target_include_directories(IMNODES
    PUBLIC ${IMNODES_DIR}/
)

target_link_libraries(IMNODES PUBLIC IMGUI)

# ================= TOML++ ================
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tomlplusplus)

# ========= ImGuiColorTextEdit ================
add_library(ImGuiColorTextEdit STATIC)

set(TEXTEDIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ImGuiColorTextEdit)
target_sources(ImGuiColorTextEdit
    PRIVATE
    ${TEXTEDIT_DIR}/TextEditor.cpp
)

target_include_directories(ImGuiColorTextEdit
    PUBLIC ${TEXTEDIT_DIR}/
)

target_link_libraries(ImGuiColorTextEdit PUBLIC IMGUI)

# ========= NFD ================
add_library(NFD STATIC)

set(NFD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nativefiledialog)

if(WIN32)
    target_sources(NFD
        PRIVATE
        ${NFD_DIR}/src/common.h
        ${NFD_DIR}/src/nfd_common.c
        ${NFD_DIR}/src/nfd_common.h
        ${NFD_DIR}/src/nfd_win.cpp
    )
else()
    target_sources(NFD
        PRIVATE
        ${NFD_DIR}/src/common.h
        ${NFD_DIR}/src/nfd_common.c
        ${NFD_DIR}/src/nfd_common.h
        ${NFD_DIR}/src/nfd_gtk.c
        ${NFD_DIR}/src/nfd_zenity.c
        ${NFD_DIR}/src/simple_exec.h
    )
endif(WIN32)

target_include_directories(NFD
    PUBLIC
    ${NFD_DIR}/src/include
)

# ================ PyBind11 ==============
# Find Python
find_package(Python COMPONENTS Interpreter Development REQUIRED)
message("PYTHON LIBS: ${Python_LIBRARIES} ")
message("PYTHON LIB DIRS: ${Python_LIBRARY_DIRS} ")

# Build PyBind11
add_subdirectory(pybind11)
