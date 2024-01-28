enable_testing()

add_executable(test_nge_physics
	test_nge_physics.cc
)
target_include_directories(test_nge_physics PRIVATE ${CMAKE_SOURCE_DIR}/nge ${CMAKE_SOURCE_DIR}/nge/include)
target_link_libraries(test_nge_physics PRIVATE ${LIBS})

add_test(NAME test_nge_physics COMMAND test_nge_physics)
