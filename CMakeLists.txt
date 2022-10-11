cmake_minimum_required(VERSION 3.14.0)
project(Stream VERSION 0.7 DESCRIPTION "Stream abstraction library for various data sources")

set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_LIST
		${INC_ROOT}/${PROJECT_NAME}/Buffer.hpp
		${INC_ROOT}/${PROJECT_NAME}/File.hpp
		${INC_ROOT}/${PROJECT_NAME}/InOut.hpp
		${INC_ROOT}/${PROJECT_NAME}/Pipe.hpp
		${INC_ROOT}/${PROJECT_NAME}/Socket.hpp
		${INC_ROOT}/${PROJECT_NAME}/Transform.hpp
		${INC_ROOT}/${PROJECT_NAME}/Transparent.hpp)
set(SRC_LIST
		${SRC_ROOT}/${PROJECT_NAME}/InOut.tpp
		${SRC_ROOT}/${PROJECT_NAME}/Transparent.tpp
		${SRC_ROOT}/${PROJECT_NAME}/Transform.tpp
		${SRC_ROOT}/Buffer.cpp
		${SRC_ROOT}/File.cpp
		${SRC_ROOT}/InOut.cpp
		${SRC_ROOT}/Pipe.cpp
		${SRC_ROOT}/Socket.cpp
		${SRC_ROOT}/Transform.cpp
		${SRC_ROOT}/Transparent.cpp)


add_library(${PROJECT_NAME} OBJECT)
target_include_directories(${PROJECT_NAME} PUBLIC inc)
target_sources(${PROJECT_NAME} PUBLIC ${INC_LIST} PRIVATE ${SRC_LIST})


if (CMAKE_BUILD_TYPE MATCHES Debug)
	enable_testing()
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test ${CMAKE_CURRENT_BINARY_DIR}/Testing)
endif()

if (CMAKE_BUILD_TYPE MATCHES Release)
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
		set(DOXYGEN_CFG ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.cfg)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cfg ${DOXYGEN_CFG} @ONLY)
		add_custom_target(${PROJECT_NAME}Doc
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CFG}
			COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen"
			VERBATIM)
	endif (DOXYGEN_FOUND)
endif()