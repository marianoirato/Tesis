#include <ros/ros.h>
#include <my_robot_controller/TramaDatos.h>

// Callback function for the subscriber
void robot_info_callback(const my_robot_controller::TramaDatos::ConstPtr& msg) {
    ROS_INFO("Datos del RMO: Tensión_BB = %d", msg->v_bat);
    for (int i = 0; i < 4; i++) {
        ROS_INFO("RPM[%d] = %f", i, msg->u_m[i]);
    }
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "robot_info");
    ros::NodeHandle nh;

    ros::Subscriber sub = nh.subscribe("robot_info", 10, robot_info_callback);

    ros::spin();

    return 0;
}

