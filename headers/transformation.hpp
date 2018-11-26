#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <vector>
#include <opencv2/opencv.hpp>

cv::Point transform(cv::Point p, cv::Mat t);

cv::Mat find_transform(
    std::vector<std::pair<cv::Point, cv::Point>> pairs, 
    std::vector<std::pair<cv::Point, cv::Point>>& selectedPairs
);

#endif