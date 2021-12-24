cmake_minimum_required(VERSION 3.20.0)
project(${PROJECT_NAME}_${Class} VERSION 0.1 DESCRIPTION "")


set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_Integer)
target_sources(${PROJECT_NAME}_Integer PRIVATE ${SRC_ROOT}/Integer.cpp)
target_link_libraries(${PROJECT_NAME}_Integer PRIVATE Stream)
add_test(NAME ${PROJECT_NAME}_Integer COMMAND ${PROJECT_NAME}_Integer)

add_executable(${PROJECT_NAME}_Float)
target_sources(${PROJECT_NAME}_Float PRIVATE ${SRC_ROOT}/Float.cpp)
target_link_libraries(${PROJECT_NAME}_Float PRIVATE Stream)
add_test(NAME ${PROJECT_NAME}_Float COMMAND ${PROJECT_NAME}_Float)

add_executable(${PROJECT_NAME}_Line)
target_sources(${PROJECT_NAME}_Line PRIVATE ${SRC_ROOT}/Line.cpp)
target_link_libraries(${PROJECT_NAME}_Line PRIVATE Stream)
add_test(NAME ${PROJECT_NAME}_Line COMMAND ${PROJECT_NAME}_Line)
