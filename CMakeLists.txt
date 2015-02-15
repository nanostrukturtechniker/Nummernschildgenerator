cmake_minimum_required(VERSION 2.6)
project(nummerntafelerstellung)


# Opencv Package
SET(OpenCV_DIR /usr/opencv2/share/OpenCV)
FIND_PACKAGE( OpenCV REQUIRED )
IF (${OpenCV_VERSION} VERSION_LESS 2.3.0)
	MESSAGE(FATAL_ERROR "OpenCV version is not compatible : ${OpenCV_VERSION}")
ENDIF()

# BOOST libraries 
FIND_PACKAGE(Boost 1.36.0 COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})


# Main Exec
add_executable(nummerntafelerstellung main.cpp)
TARGET_LINK_LIBRARIES( nummerntafelerstellung ${Boost_LIBRARIES} )
TARGET_LINK_LIBRARIES( nummerntafelerstellung ${OpenCV_LIBS} )
install(TARGETS nummerntafelerstellung RUNTIME DESTINATION bin)
