#!/usr/bin/env python3

import rospy
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy

def joy_callback(data):
    twist = Twist()
    twist.linear.x = 2*data.axes[1]   # Ajusta la velocidad lineal en el eje x
    twist.linear.y = 2*data.axes[0]   # Ajusta la velocidad lineal en el eje y
    twist.angular.z = 4*data.axes[3]  # Ajusta la velocidad angular
    pub.publish(twist)
    
if __name__ == '__main__':
    rospy.init_node("joystick_contro")
    rospy.loginfo("Test node has been started")
    pub = rospy.Publisher("/turtle1/cmd_vel", Twist, queue_size=10)
    sub = rospy.Subscriber("/joy", Joy, callback=joy_callback)
    rospy.spin()

