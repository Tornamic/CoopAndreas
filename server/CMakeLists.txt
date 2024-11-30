cmake_minimum_required(VERSION 3.24)

project(CoopAndreasServer)

add_executable(${PROJECT_NAME} server.cpp)

target_link_libraries(${PROJECT_NAME} enet)