cmake_minimum_required(VERSION 3.20.0)
project(${PROJECT_NAME}_${Class} VERSION 0.1 DESCRIPTION "")


set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_Char)
target_sources(${PROJECT_NAME}_Char PRIVATE ${SRC_ROOT}/Char.cpp)
target_link_libraries(${PROJECT_NAME}_Char PRIVATE Stream)
add_test(NAME ${PROJECT_NAME}_Char COMMAND ${PROJECT_NAME}_Char)

add_executable(${PROJECT_NAME}_HasExtraction)
target_sources(${PROJECT_NAME}_HasExtraction PRIVATE ${SRC_ROOT}/HasExtraction.cpp)
target_link_libraries(${PROJECT_NAME}_HasExtraction PRIVATE Stream)
add_test(NAME ${PROJECT_NAME}_HasExtraction COMMAND ${PROJECT_NAME}_HasExtraction)

add_executable(${PROJECT_NAME}_SignedInteger)
target_sources(${PROJECT_NAME}_SignedInteger PRIVATE ${SRC_ROOT}/SignedInteger.cpp)
target_link_libraries(${PROJECT_NAME}_SignedInteger PRIVATE Stream)
add_test(NAME ${PROJECT_NAME}_SignedInteger COMMAND ${PROJECT_NAME}_SignedInteger)

add_executable(${PROJECT_NAME}_Source)
target_sources(${PROJECT_NAME}_Source PRIVATE ${SRC_ROOT}/Source.cpp)
target_link_libraries(${PROJECT_NAME}_Source PRIVATE Stream)
add_test(NAME ${PROJECT_NAME}_Source COMMAND ${PROJECT_NAME}_Source)

add_executable(${PROJECT_NAME}_UnsignedInteger)
target_sources(${PROJECT_NAME}_UnsignedInteger PRIVATE ${SRC_ROOT}/UnsignedInteger.cpp)
target_link_libraries(${PROJECT_NAME}_UnsignedInteger PRIVATE Stream)
add_test(NAME ${PROJECT_NAME}_UnsignedInteger COMMAND ${PROJECT_NAME}_UnsignedInteger)
