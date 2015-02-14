cmake_minimum_required(VERSION 2.6)
project(nummerntafelerstellung)



# Opencv Package
SET(OpenCV_DIR /usr/opencv2/share/OpenCV)
FIND_PACKAGE( OpenCV REQUIRED )
IF (${OpenCV_VERSION} VERSION_LESS 2.3.0)
	MESSAGE(FATAL_ERROR "OpenCV version is not compatible : ${OpenCV_VERSION}")
ENDIF()



add_executable(nummerntafelerstellung main.cpp)

install(TARGETS nummerntafelerstellung RUNTIME DESTINATION bin)
