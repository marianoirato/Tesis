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

void merge_and_publish_frames(CUSTOMDATA &CustomData1, CUSTOMDATA &CustomData2, image_transport::Publisher &image_pub)
{
    while (ros::ok())
    {
        if (CustomData1.ready_to_show_frame.load() && CustomData2.ready_to_show_frame.load())
        {
            cv::Mat frame1_copy, frame2_copy;
            {
                std::lock_guard<std::mutex> lock1(CustomData1.frame_mutex);
                frame1_copy = CustomData1.frame.clone();
            }
            {
                std::lock_guard<std::mutex> lock2(CustomData2.frame_mutex);
                frame2_copy = CustomData2.frame.clone();
            }

            // Ensure both frames have the same dimensions
            if (frame1_copy.size() != frame2_copy.size())
            {
                cv::resize(frame2_copy, frame2_copy, frame1_copy.size());
            }

            // Create a new image with double the width to concatenate frames horizontally
            cv::Mat merged_frame(frame1_copy.rows, frame1_copy.cols + frame2_copy.cols, frame1_copy.type());

            // Copy frames to the merged frame
            frame1_copy.copyTo(merged_frame(cv::Rect(0, 0, frame1_copy.cols, frame1_copy.rows)));
            frame2_copy.copyTo(merged_frame(cv::Rect(frame1_copy.cols, 0, frame2_copy.cols, frame2_copy.rows)));

            sensor_msgs::ImagePtr image = cv_bridge::CvImage(std_msgs::Header(), "bgr8", merged_frame).toImageMsg();
            image_pub.publish(image);

            CustomData1.ready_to_show_frame.store(false);
            CustomData2.ready_to_show_frame.store(false);
            CustomData1.ready_to_process_frame.store(true);
            CustomData2.ready_to_process_frame.store(true);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(17)); // Aim for ~30 fps
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "camaras_node");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Publisher image_pub = it.advertise("/merged_image", 1);

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
    cam1.set_capture_format("BGRx", FrameSize{640, 480}, FrameRate{60, 1});
    cam1.set_new_frame_callback(new_frame_cb, &CustomData1);
    cam1.start();

    TcamCamera cam2(serialnumber2);
    cam2.set_capture_format("BGRx", FrameSize{640, 480}, FrameRate{60, 1});
    cam2.set_new_frame_callback(new_frame_cb, &CustomData2);
    cam2.start();

    std::thread merge_thread(merge_and_publish_frames, std::ref(CustomData1), std::ref(CustomData2), std::ref(image_pub));

    ros::Rate rate(60); // 60 Hz
   
    while (ros::ok())
    {
        ros::spinOnce(); // Process ROS callbacks
        rate.sleep();    // Sleep for the remainder of the loop cycle to maintain 30 Hz
    }
    
    // Cleanup
    cam1.stop();
    cam2.stop();
    merge_thread.join();

    return 0;
}
