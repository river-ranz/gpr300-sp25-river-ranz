file(
 GLOB_RECURSE ASSIGNMENT6_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE ASSIGNMENT6_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this assignment1's asset folder to bin when it is built
add_custom_target(copyAssetsA6 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${ASSIGNMENT6_INC} DESTINATION include/assignment6)
add_executable(assignment6 ${ASSIGNMENT6_SRC} ${ASSIGNMENT6_INC})
target_link_libraries(assignment6 PUBLIC core IMGUI assimp)
target_include_directories(assignment6 PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when assignment1 is built
add_dependencies(assignment6 copyAssetsA6)