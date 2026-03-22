cmake_minimum_required(VERSION 3.10)
project(Voxel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ?
find_package(SDL3 REQUIRED)
# find_package(OpenGl REQUIRED)

add_executable(Voxel
	src/main.cpp

	src/util/defines.h
	src/util/util.h
	src/util/debug.h
	src/util/debug.cpp

	src/render/window.h
	src/render/window.cpp
	src/render/shader.h
	src/render/renderer.h
	src/render/renderer.cpp
	src/render/camera.h
	src/render/camera.cpp
	src/render/texture.h
	src/render/texture.cpp

	src/world/chunk.h
	src/world/chunk.cpp
	src/world/world.h
	src/world/world.cpp

	lib/glad/src/glad.c

	lib/imgui/imconfig.h
    lib/imgui/imgui.cpp
	lib/imgui/imgui.h
	lib/imgui/imgui_demo.cpp
	lib/imgui/imgui_draw.cpp
	lib/imgui/imgui_internal.h
	lib/imgui/imgui_tables.cpp
	lib/imgui/imgui_widgets.cpp
	lib/imgui/imstb_rectpack.h
	lib/imgui/imstb_textedit.h
	lib/imgui/imstb_truetype.h
    lib/imgui/backends/imgui_impl_sdl3.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(Voxel PRIVATE
	src
	lib
	lib/glad/include
	lib/imgui
)

target_link_libraries(Voxel PRIVATE
	SDL3
	GL
)
