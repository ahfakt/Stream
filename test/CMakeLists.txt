cmake_minimum_required(VERSION 3.20.0)
project(${PROJECT_NAME}_Test VERSION 0.1 DESCRIPTION "")


file(GLOB INC ${CMAKE_CURRENT_SOURCE_DIR}/inc/${PROJECT_NAME}/Test/*.hpp)
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} OBJECT)
target_sources(${PROJECT_NAME} PUBLIC ${INC} PRIVATE ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC inc)
target_link_libraries(${PROJECT_NAME} PUBLIC Stream)


file(GLOB Classes RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/[A-Z]*)
foreach(Class ${Classes})
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${Class})
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${Class} ${CMAKE_CURRENT_BINARY_DIR}/${Class})
	endif()
endforeach()