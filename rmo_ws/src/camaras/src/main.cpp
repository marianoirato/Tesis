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

using namespace gsttcam;

typedef struct
{
    std::atomic<bool> ready_to_process_frame;
    std::atomic<bool> ready_to_show_frame;
    cv::Mat frame;
    std::mutex frame_mutex; // Mutex for synchronizing frame access
} CUSTOMDATA;

GstFlowReturn new_frame_cb(GstAppSink *appsink, gpointer data)
{
    int width, height;
    const GstStructure *str;

    CUSTOMDATA *pCustomData = (CUSTOMDATA*)data;

    if (!pCustomData->ready_to_process_frame.load())
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
            {
                std::lock_guard<std::mutex> lock(pCustomData->frame_mutex);
                cv::cvtColor(tmp_frame, pCustomData->frame, cv::COLOR_BGRA2BGR);
            }

            pCustomData->ready_to_show_frame.store(true);
            pCustomData->ready_to_process_frame.store(false);
        }
    }

    gst_buffer_unmap(buffer, &info);
    gst_sample_unref(sample);

    return GST_FLOW_OK;
}

void process_frame(CUSTOMDATA &CustomData, image_transport::Publisher &image_pub, const std::string &camera_name)
{
    while (ros::ok())
    {
        if (CustomData.ready_to_show_frame.load())
        {
            cv::Mat frame_copy;
            {
                std::lock_guard<std::mutex> lock(CustomData.frame_mutex);
                frame_copy = CustomData.frame.clone();
            }

            std::vector<uchar> buf;
            cv::imencode(".jpg", frame_copy, buf);  // Compress the image
            cv::Mat compressed_frame = cv::imdecode(buf, cv::IMREAD_COLOR);

            sensor_msgs::ImagePtr image = cv_bridge::CvImage(std_msgs::Header(), "bgr8", compressed_frame).toImageMsg();
            image_pub.publish(image);
            CustomData.ready_to_show_frame.store(false);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(32));
    }
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
    CustomData1.ready_to_process_frame.store(true);
    CustomData1.ready_to_show_frame.store(false);

    CUSTOMDATA CustomData2;
    CustomData2.ready_to_process_frame.store(true);
    CustomData2.ready_to_show_frame.store(false);

    std::vector<CameraInfo> camera_list = get_device_list();
    if (camera_list.size() < 2)
    {
        printf("At least two cameras are required.\n");
        return 1;
    }

    std::string serialnumber1 = camera_list[0].serial;
    std::string serialnumber2 = camera_list[1].serial;

    // Print the serial numbers of the cameras
    std::cout << "Camera 1 Serial Number: " << serialnumber1 << std::endl;
    std::cout << "Camera 2 Serial Number: " << serialnumber2 << std::endl;

    TcamCamera cam1(serialnumber1);
    cam1.set_capture_format("BGRx", FrameSize{640, 480}, FrameRate{30, 1});
    cam1.set_new_frame_callback(new_frame_cb, &CustomData1);
    cam1.start();

    TcamCamera cam2(serialnumber2);
    cam2.set_capture_format("BGRx", FrameSize{640, 480}, FrameRate{30, 1});
    cam2.set_new_frame_callback(new_frame_cb, &CustomData2);
    cam2.start();

    std::thread frame_thread1(process_frame, std::ref(CustomData1), std::ref(image_pub1), "Camera 1");
    std::thread frame_thread2(process_frame, std::ref(CustomData2), std::ref(image_pub2), "Camera 2");

    while (ros::ok())
    {
        CustomData1.ready_to_process_frame.store(true);
        CustomData2.ready_to_process_frame.store(true);
        ros::spinOnce();
    }

    printf("Press Enter to end the program");
    getchar();

    cam1.stop();
    cam2.stop();
    frame_thread1.join();
    frame_thread2.join();

    return 0;
}
