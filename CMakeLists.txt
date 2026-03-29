cmake_minimum_required(VERSION 3.10)
project(Voxel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(Voxel
	src/main.cpp

	src/util/defines.h
	src/util/util.h

	src/render/window.h
	src/render/window.cpp
	src/render/shader.h
	src/render/renderer.h
	src/render/renderer.cpp
	src/render/camera.h
	src/render/camera.cpp
	src/render/texture.h
	src/render/texture.cpp
	src/render/culling.h
	src/render/culling.cpp

	src/world/chunk.h
	src/world/chunk.cpp
	src/world/world.h
	src/world/world.cpp

	src/ui/ui.h
	src/ui/ui.cpp
	src/ui/text.h
	src/ui/text.cpp
	src/ui/debug.h
	src/ui/debug.cpp

	lib/glad/src/glad.c
)

target_include_directories(Voxel PRIVATE
	src
	lib
	lib/glad/include

	lib/freetype/include
)

target_link_libraries(Voxel PRIVATE
	SDL3::SDL3
	OpenGL::GL
	freetype
)
