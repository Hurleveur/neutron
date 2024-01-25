cmake_minimum_required(VERSION 3.20)

project(solarsystem C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
	link_directories(${CMAKE_SOURCE_DIR}/nge/lib/windows)
elseif (UNIX)
	link_directories(${CMAKE_SOURCE_DIR}/nge/lib/linux)
endif (WIN32)

if (WIN32)
	set(LIBS glfw3 opengl32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif (UNIX)
	set(LIBS stdc++ glfw epoxy)
endif (WIN32)

add_executable(neutron
	# nge engine files
	nge/build.hh
	nge/glad.c
	nge/nge_graphics.hh
	nge/nge_math.hh
	nge/nge_memory.hh
	nge/nge_timing.hh
	nge/nge_window.hh
	nge/types.hh

	# neutron game files
	neutron/game.hh
	neutron/import.cpp
	neutron/import.hh
	neutron/main.cc
	neutron/particle_generator.cpp
	neutron/particle_generator.hh
	neutron/planet.hh
	neutron/textures.hh
)
target_include_directories(neutron PRIVATE ${CMAKE_SOURCE_DIR}/nge ${CMAKE_SOURCE_DIR}/nge/include)
target_link_libraries(neutron PRIVATE ${LIBS})
if (WIN32)
	set_target_properties(neutron PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
elseif (UNIX)
	set_target_properties(neutron PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
endif (WIN32)
