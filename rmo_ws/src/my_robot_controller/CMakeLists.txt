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
  message_generation  # Añadido para mensajes personalizados
)

set(LibSerial_DIR /some/ubuntu/libserial)

## Añadir la generación de archivos de mensajes
add_message_files(
  FILES
  TramaDatos.msg
)

## Añadir dependencias de mensajes
generate_messages(
  DEPENDENCIES
  std_msgs
)

include_directories(/usr/include)

catkin_package(
  CATKIN_DEPENDS roscpp rospy sensor_msgs std_msgs message_runtime  # Añadido message_runtime
  #  INCLUDE_DIRS include
  #  LIBRARIES my_robot_controller
  #  DEPENDS system_lib
)

add_executable(robot_uart_control src/robot_uart_control.cpp)
target_link_libraries(robot_uart_control 
    ${catkin_LIBRARIES}
    ${Boost_LIBRARIES}
)

add_executable(robot_info_suscriber src/robot_info_suscriber.cpp) 
target_link_libraries(robot_info_suscriber 
    ${catkin_LIBRARIES}
)

install(TARGETS robot_uart_control
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(TARGETS robot_uart_control robot_info_suscriber
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

