cmake_minimum_required (VERSION 2.6)
project (RobotControlDaemon)

include_directories ("${PROJECT_SOURCE_DIR}/fcgiServer" ${PROJECT_SOURCE_DIR})
add_subdirectory (fcgiServer) 

# add the executable
add_executable (RobotControlDaemon main.cpp base16message.cpp MessageParser.cpp ControlLogic.cpp robotboard2interpreter.cpp debuglog.cpp)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

target_link_libraries(RobotControlDaemon fcgi fcgiServer pthread)

