cmake_minimum_required(VERSION 3.10)
project(Voxel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Voxel
	src/main.cpp

	src/util/defines.h
	src/util/util.h

	src/render/window.h
	src/render/window.cpp
)

target_link_libraries(Voxel
	SDL3
	GL
)
