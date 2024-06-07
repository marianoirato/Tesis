cmake_minimum_required(VERSION 3.0.2)
project(my_robot_controller)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  std_msgs
)
set(LibSerial_DIR /some/ubuntu/libserial)

include_directories(/usr/include)
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES my_robot_controller
#  CATKIN_DEPENDS roscpp rospy turtlesim
#  DEPENDS system_lib
)

add_executable(robot_uart_control src/robot_uart_control.cpp)
target_link_libraries(robot_uart_control 
    ${catkin_LIBRARIES}
    ${Boost_LIBRARIES}
)

install(TARGETS robot_uart_control
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  )

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

