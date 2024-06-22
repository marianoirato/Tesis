#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <ros/ros.h>

#include "tcamcamera.h"
#include <unistd.h>
#include <thread>
#include <atomic>
#include <mutex>

#include "opencv2/opencv.hpp"
#include "sensor_msgs/Image.h"
#include "cv_bridge/cv_bridge.h"
#include "image_transport/image_transport.h"

#define WIDTH  640
#define HEIGHT 480
#define FPS 30
using namespace gsttcam;
using namespace std;
typedef struct
{
   bool ready_to_process_frame;
   bool ready_to_show_frame;
   cv::Mat frame;
} CUSTOMDATA;

GstFlowReturn new_frame_cb(GstAppSink *appsink, gpointer data)
{
    int width, height;
    const GstStructure *str;

    CUSTOMDATA *pCustomData = (CUSTOMDATA*)data;

    if (!pCustomData->ready_to_process_frame)
    {
        return GST_FLOW_OK;
    }

    GstSample *sample = gst_app_sink_pull_sample(appsink);

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo info;

    gst_buffer_map(buffer, &info, GST_MAP_READ);
    if (info.data != NULL)
    {
        GstCaps *caps = gst_sample_get_caps(sample);
        str = gst_caps_get_structure(caps, 0);

        if (strcmp(gst_structure_get_string(str, "format"), "BGRx") == 0)
        {
            gst_structure_get_int(str, "width", &width);
            gst_structure_get_int(str, "height", &height);

            cv::Mat tmp_frame(height, width, CV_8UC4, (void*)info.data);
            cv::cvtColor(tmp_frame, pCustomData->frame, cv::COLOR_BGRA2BGR);
            pCustomData->ready_to_show_frame = true;
            pCustomData->ready_to_process_frame = false;
        }
        else
        {
            cout<< "asd"<<endl;
        }
    }

    gst_buffer_unmap(buffer, &info);
    gst_sample_unref(sample);

    return GST_FLOW_OK;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "camaras_node");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Publisher image_pub1 = it.advertise("/camera1", 1);
    image_transport::Publisher image_pub2 = it.advertise("/camera2", 1);

    gst_init(&argc, &argv);

    CUSTOMDATA CustomData1;
    CustomData1.ready_to_process_frame = true;
    CustomData1.ready_to_show_frame = false;

    CUSTOMDATA CustomData2;
    CustomData2.ready_to_process_frame = true;
    CustomData2.ready_to_show_frame = false;

    ros::Rate loop_rate(10);

// { //INICIALIZACION DE LAS CAMARAS
    std::vector<CameraInfo> camera_list = get_device_list();
    if (camera_list.size() < 2)
    {
        printf("At least two cameras are required.\n");
        //return 1;
    }

    std::string serialnumber1 = camera_list[0].serial;
    std::string serialnumber2 = camera_list[1].serial;

    // Print the serial numbers of the cameras
    std::cout << "Camera 1 Serial Number: " << serialnumber1 << std::endl;
    std::cout << "Camera 2 Serial Number: " << serialnumber2 << std::endl;

    TcamCamera cam1(serialnumber1);
    cam1.set_capture_format("BGRx", FrameSize{WIDTH, HEIGHT}, FrameRate{FPS, 1});
    cam1.set_new_frame_callback(new_frame_cb, &CustomData1);
    cam1.start();

    TcamCamera cam2(serialnumber2);
    cam2.set_capture_format("BGRx", FrameSize{WIDTH, HEIGHT}, FrameRate{FPS, 1});
    cam2.set_new_frame_callback(new_frame_cb, &CustomData2);
    cam2.start();
// }

    while (ros::ok())
    {
        CustomData1.ready_to_process_frame = true;
        CustomData2.ready_to_process_frame = true;
        ros::spinOnce();
        loop_rate.sleep();

        if (CustomData1.ready_to_show_frame)
        {
            cv::Mat frame_copy = CustomData1.frame.clone();

            std::vector<uchar> buf;
            cv::imencode(".jpg", frame_copy, buf);  // Compress the image
            cv::Mat compressed_frame = cv::imdecode(buf, cv::IMREAD_COLOR);

            sensor_msgs::ImagePtr image1 = cv_bridge::CvImage(std_msgs::Header(), "bgr8", compressed_frame).toImageMsg();
            image_pub1.publish(image1);
            CustomData1.ready_to_show_frame = false;
        }
        
        if (CustomData2.ready_to_show_frame)
        {
            cv::Mat frame_copy = CustomData2.frame.clone();

            std::vector<uchar> buf;
            cv::imencode(".jpg", frame_copy, buf);  // Compress the image
            cv::Mat compressed_frame = cv::imdecode(buf, cv::IMREAD_COLOR);

            sensor_msgs::ImagePtr image2 = cv_bridge::CvImage(std_msgs::Header(), "bgr8", compressed_frame).toImageMsg();
            image_pub2.publish(image2);
            CustomData2.ready_to_show_frame = false;
        }
    }

    printf("Press Enter to end the program");
    getchar();

    cam1.stop();
    //cam2.stop();

    return 0;
}
