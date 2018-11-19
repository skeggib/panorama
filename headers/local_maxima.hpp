#ifndef MAXIMA_HPP
#define MAXIMA_HPP

#include <opencv2/opencv.hpp>

cv::Mat local_maxima(const cv::Mat& image, int n);

cv::Mat local_maxima(const cv::Mat& image, int windowWidth, int windowHeight);

#endif