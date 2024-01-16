cmake_minimum_required(VERSION 3.20)

project(neutron C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
	link_directories(${CMAKE_SOURCE_DIR}/lib/windows)
elseif (UNIX)
	link_directories(${CMAKE_SOURCE_DIR}/lib/linux)
endif (WIN32)

if (WIN32)
	set(LIBS glfw3 opengl32 assimp freetype irrKlang)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif (UNIX)
	set(LIBS stdc++ glfw epoxy) # freetype irrKlang X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype
endif (WIN32)

macro(makeLink src dest target)
	add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest} DEPENDS ${dest} COMMENT "mklink ${src} -> ${dest}")
endmacro()

add_executable(neutron
	src/neutron/game.cpp
	src/neutron/import.cpp
	src/neutron/particle_generator.cpp
	src/neutron/textures.cpp
	src/glad.c
)
target_include_directories(neutron PRIVATE ${CMAKE_SOURCE_DIR}/includes)
target_link_libraries(neutron PRIVATE ${LIBS})
if (WIN32)
	set_target_properties(neutron PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
elseif (UNIX)
	set_target_properties(neutron PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
endif (WIN32)
