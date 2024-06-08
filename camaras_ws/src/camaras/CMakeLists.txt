cmake_minimum_required(VERSION 3.0.2)
project(camaras)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  image_view
  roscpp
  rospy
)

find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)

pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)
pkg_check_modules(TCAMLIB tcam)

message(${TCAMLIB_LIBRARIES})

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES camaras_ros
 CATKIN_DEPENDS cv_bridge image_transport image_view roscpp rospy
#  DEPENDS system_lib
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR}  
	${GSTREAMER_INCLUDE_DIRS} ${TCAM_INCLUDE_DIRS}  
	${OpenCV_INCLUDE_DIRS} 
    	${catkin_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/include/camaras
)

add_definitions(${GSTREAMER_CFLAGS_OTHER})  

add_executable(${PROJECT_NAME}_node src/main.cpp src/tcamcamera.cpp)

target_link_libraries(${PROJECT_NAME}_node 
	${TCAMLIB_LIBRARIES} 
	${GSTREAMER_LIBRARIES} 
	${OpenCV_LIBS} 
  	${catkin_LIBRARIES}
)
