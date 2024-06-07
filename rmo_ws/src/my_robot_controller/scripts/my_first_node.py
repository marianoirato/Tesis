#!/usr/bin/env python3
import rospy

i=0

if __name__=='__main__':
    rospy.init_node("test_node")
    rospy.loginfo("Testt node has been started")
   
    rate = rospy.Rate(10)

    while not rospy.is_shutdown():
        rospy.loginfo("Hello: %d " % i)
        rate.sleep()
        i=i+1
