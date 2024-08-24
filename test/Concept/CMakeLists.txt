cmake_minimum_required(VERSION 3.20.0)
project(${PROJECT_NAME}_${Class} VERSION 0.1 DESCRIPTION "")


set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_Source)
target_sources(${PROJECT_NAME}_Source PRIVATE ${SRC_ROOT}/Source.cpp)
target_include_directories(${PROJECT_NAME}_Source PRIVATE)
target_link_libraries(${PROJECT_NAME}_Source PRIVATE Stream)
add_test(NAME ${PROJECT_NAME}_Source COMMAND ${PROJECT_NAME}_Source)

add_executable(${PROJECT_NAME}_HasExtraction)
target_sources(${PROJECT_NAME}_HasExtraction PRIVATE ${SRC_ROOT}/HasExtraction.cpp)
target_include_directories(${PROJECT_NAME}_HasExtraction PRIVATE)
target_link_libraries(${PROJECT_NAME}_HasExtraction PRIVATE Stream)
add_test(NAME ${PROJECT_NAME}_HasExtraction COMMAND ${PROJECT_NAME}_HasExtraction)
