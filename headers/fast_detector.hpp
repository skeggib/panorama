#ifndef FAST_DETECTOR_HPP
#define FAST_DETECTOR_HPP

#include <opencv2/opencv.hpp>

cv::Mat fast_detector(const cv::Mat & image, int threshold);

#endif