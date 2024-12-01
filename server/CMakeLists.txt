cmake_minimum_required(VERSION 3.24)

project(CoopAndreasServer)

add_library(CNetwork OBJECT "src/CNetwork.cpp")
add_library(CPed OBJECT "src/CPed.cpp")
add_library(CPedManager OBJECT "src/CPedManager.cpp")
add_library(CPlayer OBJECT "src/CPlayer.cpp")
add_library(CPlayerManager OBJECT "src/CPlayerManager.cpp")
add_library(CVehicle OBJECT "src/CVehicle.cpp")
add_library(CVehicleManager OBJECT "src/CVehicleManager.cpp")



add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} enet)

target_link_libraries(${PROJECT_NAME} CNetwork)
target_link_libraries(${PROJECT_NAME} CPed)
target_link_libraries(${PROJECT_NAME} CPedManager)
target_link_libraries(${PROJECT_NAME} CPlayer)
target_link_libraries(${PROJECT_NAME} CPlayerManager)
target_link_libraries(${PROJECT_NAME} CVehicle)
target_link_libraries(${PROJECT_NAME} CVehicleManager)
