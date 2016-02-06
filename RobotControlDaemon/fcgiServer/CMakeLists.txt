
add_library(fcgiServer fcgiserver.cpp)

find_library(myFcgi fcgi)
#find_library(myPthread pthread PATH_SUFFIXES arm-linux-gnueabi)

#target_link_libraries(fcgiServer fcgi pthread)
target_link_libraries(fcgiServer ${myFcgi} pthread)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

