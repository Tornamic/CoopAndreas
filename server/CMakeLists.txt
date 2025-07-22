cmake_minimum_required(VERSION 3.24)

project(CoopAndreasServer)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	SET(GCC_COVERAGE_COMPILE_FLAGS "-static -Wno-deprecated-declarations")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
endif()


add_library(Network 	    OBJECT      "src/CNetwork.cpp")
add_library(Ped 	    	OBJECT      "src/CPed.cpp")
add_library(PedManager     	OBJECT     	"src/CPedManager.cpp")
add_library(Player 	    	OBJECT 	   	"src/CPlayer.cpp")
add_library(PlayerManager  	OBJECT  	"src/CPlayerManager.cpp")
add_library(Vehicle        	OBJECT 	  	"src/CVehicle.cpp")
add_library(VehicleManager 	OBJECT 		"src/CVehicleManager.cpp")
add_library(ConfigFile	   	OBJECT 		"src/CConfigFile.cpp")
add_library(CommandManager  OBJECT      "src/CCommandManager.cpp")
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/lib/libenet.a")
target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/lib/libiem-dini.a")
if(WIN32)
	target_link_libraries(${PROJECT_NAME} ws2_32.a)
	target_link_libraries(${PROJECT_NAME} winmm.a)
endif()


target_link_libraries(${PROJECT_NAME} Network)
target_link_libraries(${PROJECT_NAME} Ped)
target_link_libraries(${PROJECT_NAME} PedManager)
target_link_libraries(${PROJECT_NAME} Player)
target_link_libraries(${PROJECT_NAME} PlayerManager)
target_link_libraries(${PROJECT_NAME} Vehicle)
target_link_libraries(${PROJECT_NAME} VehicleManager)
target_link_libraries(${PROJECT_NAME} ConfigFile)
target_link_libraries(${PROJECT_NAME} CommandManager)


