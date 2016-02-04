
add_library(fcgiServer fcgiserver.cpp)
target_link_libraries(fcgiServer fcgi pthread)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

