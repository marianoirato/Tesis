#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "tcamcamera.h"
#include <unistd.h>

#include "opencv2/opencv.hpp"

#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "cv_bridge/cv_bridge.h"
#include <opencv2/highgui/highgui.hpp>
#include <image_transport/image_transport.h>

using namespace gsttcam;

// ROS node handle
ros::NodeHandle *nh;

// ROS publisher
ros::Publisher image_pub;

// Create a custom data structure to be passed to the callback function.
typedef struct
{
    int ImageCounter;
    bool SaveNextImage;
    bool busy;
    cv::Mat frame;
} CUSTOMDATA;

////////////////////////////////////////////////////////////////////
// Callback called for new images by the internal appsink
GstFlowReturn new_frame_cb(GstAppSink *appsink, gpointer data)
{
    int width, height;
    const GstStructure *str;

    // Cast gpointer to CUSTOMDATA*
    CUSTOMDATA *pCustomData = (CUSTOMDATA *)data;

    pCustomData->ImageCounter++;

    // The following lines demonstrate how to access the image
    // data in the GstSample.
    GstSample *sample = gst_app_sink_pull_sample(appsink);

    GstBuffer *buffer = gst_sample_get_buffer(sample);

    GstMapInfo info;

    gst_buffer_map(buffer, &info, GST_MAP_READ);

    if (info.data != NULL)
    {
        // info.data contains the image data as blob of unsigned char

        GstCaps *caps = gst_sample_get_caps(sample);
        // Get a string containing the pixel format, width and height of the image
        str = gst_caps_get_structure(caps, 0);

        if (strcmp(gst_structure_get_string(str, "format"), "BGRx") == 0)
        {
            // Now query the width and height of the image
            gst_structure_get_int(str, "width", &width);
            gst_structure_get_int(str, "height", &height);

            // Create a cv::Mat, copy image data into that
            pCustomData->frame.create(height, width, CV_8UC(4));
            memcpy(pCustomData->frame.data, info.data, width * height * 4);

            // Convert cv::Mat to ROS image message
            //sensor_msgs::ImagePtr image = cv_bridge::CvImage(std_msgs::Header(), "bgr8", pCustomData->frame).toImageMsg();
	    
	    //std::string serialnumber = TcamCamera::getFirstDeviceSerialNumber();
	    //image->header.frame_id = serialnumber;

            sensor_msgs::ImagePtr image = cv_bridge::CvImage(std_msgs::Header(), "bgr8", pCustomData->frame).toImageMsg();

	    // Publish the ROS image message
            image_pub.publish(image);
        }
    }

    // Calling Unref is important!
    gst_buffer_unmap(buffer, &info);
    gst_sample_unref(sample);

    // Set our flag of new image to true, so our main thread knows about a new image.
    return GST_FLOW_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    // Initialize ROS node
    ros::init(argc, argv, "image_publisher");
    nh = new ros::NodeHandle();

    image_transport::ImageTransport it(*nh);
    image_transport::Publisher pub = it.advertise("camera/image", 1);

    // Declare custom data structure for the callback
    CUSTOMDATA CustomData;

    CustomData.ImageCounter = 0;
    CustomData.SaveNextImage = false;

    // ROS publisher for the image topic
    image_pub = nh->advertise<sensor_msgs::Image>("image_topic", 1);

    gst_init(&argc, &argv);

    // Query the serial number of the first found device.
    // will return an empty string, if no device was found.
    std::string serialnumber = TcamCamera::getFirstDeviceSerialNumber();
    printf("Tcam OpenCV Image Sample\n");
    if (serialnumber != "")
    {
        printf("Found serial number \"%s\".\n", serialnumber.c_str());
    }

    // Check, whether the passed serial number matches a device.
    // End program, if the serial number does not exist.
    if (!TcamCamera::SerialExists(serialnumber))
    {
        printf("Serial number \"%s\" does not exist.\n", serialnumber.c_str());
        return 1;
    }

    // Open camera by serial number
    TcamCamera cam(serialnumber);
    // Set video format, resolution and frame rate
    cam.set_capture_format("BGRx", FrameSize{640, 480}, FrameRate{30, 1});
    // Register a callback to be called for each new frame
    cam.set_new_frame_callback(new_frame_cb, &CustomData);
    // Start the camera
    cam.start();
            
    // Simple implementation of "getch()"
    printf("Press Enter to end the program");
    char dummyvalue[10];
    scanf("%c",dummyvalue);

    cam.stop();
    // Shutdown ROS
    delete nh;

    return 0;
}

