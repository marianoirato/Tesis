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
    std::atomic<bool> new_frame1;
    std::atomic<bool> new_frame2;
    cv::Mat frame1;
    cv::Mat frame2;
    cv::Mat frame;
    std::mutex frame_mutex;
} CUSTOMDATA;

CUSTOMDATA custom_data;

GstFlowReturn new_frame_cb(GstAppSink *appsink, gpointer data)
{
    int width, height;
    const GstStructure *str;

    CUSTOMDATA *pCustomData = (CUSTOMDATA*)data;

    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if (!sample) {
        return GST_FLOW_ERROR;
    }

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
            cv::Mat bgr_frame;
            cv::cvtColor(tmp_frame, bgr_frame, cv::COLOR_BGRA2BGR);

            std::lock_guard<std::mutex> lock(pCustomData->frame_mutex);
            if (strcmp(gst_structure_get_name(str), "camera1") == 0)
            {
                pCustomData->frame1 = bgr_frame.clone();
                pCustomData->new_frame1.store(true);
            }
            else if (strcmp(gst_structure_get_name(str), "camera2") == 0)
            {
                pCustomData->frame2 = bgr_frame.clone();
                pCustomData->new_frame2.store(true);
            }

            if (pCustomData->new_frame1.load() && pCustomData->new_frame2.load())
            {
                cv::hconcat(pCustomData->frame1, pCustomData->frame2, pCustomData->frame);
                pCustomData->new_frame1.store(false);
                pCustomData->new_frame2.store(false);
            }
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
        if (!CustomData.frame.empty())
        {
            cv::Mat frame_copy;
            {
                std::lock_guard<std::mutex> lock(CustomData.frame_mutex);
                frame_copy = CustomData.frame.clone();
            }

            sensor_msgs::ImagePtr merged_image = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame_copy).toImageMsg();
            image_pub.publish(merged_image);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(33)); // Aim for ~30 fps
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "cameras_node");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Publisher merged_image_pub = it.advertise("/merged_image", 1);

    gst_init(&argc, &argv);

    custom_data.new_frame1.store(false);
    custom_data.new_frame2.store(false);

    std::vector<gsttcam::CameraInfo> camera_list = gsttcam::get_device_list();
    if (camera_list.size() < 2)
    {
        printf("At least two cameras are required.\n");
        return 1;
    }

    std::string serialnumber1 = camera_list[0].serial;
    std::string serialnumber2 = camera_list[1].serial;

    gsttcam::TcamCamera cam1(serialnumber1);
    cam1.set_capture_format("BGRx", gsttcam::FrameSize{640, 480}, gsttcam::FrameRate{30, 1});
    cam1.set_new_frame_callback(new_frame_cb, &custom_data);
    cam1.start();

    gsttcam::TcamCamera cam2(serialnumber2);
    cam2.set_capture_format("BGRx", gsttcam::FrameSize{640, 480}, gsttcam::FrameRate{30, 1});
    cam2.set_new_frame_callback(new_frame_cb, &custom_data);
    cam2.start();

    std::thread frame_thread(process_frame, std::ref(custom_data), std::ref(merged_image_pub), "Merged Image");

    ros::spin();

    printf("Press Enter to end the program");
    getchar();

    cam1.stop();
    cam2.stop();
    frame_thread.join();

    return 0;
}

