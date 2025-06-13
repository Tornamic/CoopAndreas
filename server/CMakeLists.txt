cmake_minimum_required(VERSION 3.24)

project(CoopAndreasServer)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	SET(GCC_COVERAGE_COMPILE_FLAGS "-static -Wno-deprecated-declarations")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
endif()


add_library(Network OBJECT "Sources/Network/CNetwork.cpp")

add_library(Ped OBJECT "Sources/Ped/CPed.cpp")
add_library(PedManager OBJECT "Sources/Ped/CPedManager.cpp")

add_library(Player OBJECT "Sources/Player/CPlayer.cpp")
add_library(PlayerManager OBJECT "Sources/Player/CPlayerManager.cpp")

#add_library(Server_ServerCommandManager OBJECT "Sources/Server/CLI/Commands/ServerCommandManager.cpp")
#add_library(Server_ServerCommand OBJECT "Sources/Server/CLI/Commands/ServerCommand.cpp")

add_library(Vehicle OBJECT "Sources/Vehicle/CVehicle.cpp")
add_library(VehicleManager OBJECT "Sources/Vehicle/CVehicleManager.cpp")


add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/Lib/libenet.a")
target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/Lib/libdini.a")
if(WIN32)
	target_link_libraries(${PROJECT_NAME} ws2_32.a)
	target_link_libraries(${PROJECT_NAME} winmm.a)
endif()


target_link_libraries(${PROJECT_NAME} Network)
target_link_libraries(${PROJECT_NAME} Ped)
target_link_libraries(${PROJECT_NAME} PedManager)
target_link_libraries(${PROJECT_NAME} Player)
target_link_libraries(${PROJECT_NAME} PlayerManager)
#target_link_libraries(${PROJECT_NAME} Server_ServerCommandManager)
#target_link_libraries(${PROJECT_NAME} Server_ServerCommand)
target_link_libraries(${PROJECT_NAME} Vehicle)
target_link_libraries(${PROJECT_NAME} VehicleManager)


