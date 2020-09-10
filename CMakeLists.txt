cmake_minimum_required(VERSION 3.14)
project(Stream VERSION 0.1 DESCRIPTION "")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} SHARED)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_sources(${PROJECT_NAME}
	PUBLIC
	include/${PROJECT_NAME}/Input.h
	include/${PROJECT_NAME}/InputFile.h
	include/${PROJECT_NAME}/Output.h
	include/${PROJECT_NAME}/OutputFile.h
	include/${PROJECT_NAME}/Raw.h
	PRIVATE
	src/Input.cpp
	src/InputFile.cpp
	src/Output.cpp
	src/OutputFile.cpp
	src/Raw.cpp)
