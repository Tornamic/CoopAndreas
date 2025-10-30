cmake_minimum_required(VERSION 3.24)

project(CoopAndreasServer)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	SET(GCC_COVERAGE_COMPILE_FLAGS "-Wno-deprecated-declarations")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
endif()


add_library(CNetwork OBJECT "src/CNetwork.cpp")
add_library(CPed OBJECT "src/CPed.cpp")
add_library(CPedManager OBJECT "src/CPedManager.cpp")
add_library(CPlayer OBJECT "src/CPlayer.cpp")
add_library(CPlayerManager OBJECT "src/CPlayerManager.cpp")
add_library(CVehicle OBJECT "src/CVehicle.cpp")
add_library(CVehicleManager OBJECT "src/CVehicleManager.cpp")

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/build/libenet.a")
target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/build/libinicpp.a")

if(WIN32)
	target_link_libraries(${PROJECT_NAME} ws2_32.a)
	target_link_libraries(${PROJECT_NAME} winmm.a)
endif()


target_link_libraries(${PROJECT_NAME} CNetwork)
target_link_libraries(${PROJECT_NAME} CPed)
target_link_libraries(${PROJECT_NAME} CPedManager)
target_link_libraries(${PROJECT_NAME} CPlayer)
target_link_libraries(${PROJECT_NAME} CPlayerManager)
target_link_libraries(${PROJECT_NAME} CVehicle)
target_link_libraries(${PROJECT_NAME} CVehicleManager)