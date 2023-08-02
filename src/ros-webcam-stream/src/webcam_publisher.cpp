// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/header.hpp"

#include "opencv2/opencv.hpp"
#include "cv_bridge/cv_bridge.h"
#include <image_transport/image_transport.hpp>

#include <iostream>

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
 * member function as a callback from the timer. */

class MinimalPublisher : public rclcpp::Node
{
      public:
	MinimalPublisher() : Node("webcam_publisher"), count_(0)
	{
		cap_.open(0);
		if (!cap_.isOpened()) {
			std::cout << "cannot open camera";
			// rclcpp::cancel();
		}

		publisher_ = this->create_publisher<sensor_msgs::msg::Image>(
			"webcam_image", 10);

		timer_ = this->create_wall_timer(
			10ms,
			std::bind(&MinimalPublisher::timer_callback, this));

#if 0
		while (true) {
			cap >> image;
			cv::imshow("test", image);
			cv::waitKey(25);
		}
#endif
	}

      private:
	void timer_callback()
	{
		cap_ >> image_;
		// into a ROS image message
		msg_ = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8",
					  image_)
			       .toImageMsg();

		// Publish the image to the topic defined in the publisher
		publisher_->publish(*msg_.get());

		auto message = std_msgs::msg::String();
		RCLCPP_INFO(this->get_logger(), "Image %ld published", count_);
		count_++;
	}

	rclcpp::TimerBase::SharedPtr timer_;
	sensor_msgs::msg::Image::SharedPtr msg_;
	rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;

	cv::VideoCapture cap_;
	cv::Mat image_;

	size_t count_;
};

int main(int argc, char *argv[])
{
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<MinimalPublisher>());
	rclcpp::shutdown();
	return 0;
}
