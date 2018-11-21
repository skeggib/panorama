#ifndef PANORAMA_HPP
#define PANORAMA_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <array>
#include "panorama.hpp"
#include "fast_detector.hpp"
#include "local_maxima.hpp"

template<int Size>
struct corner {
    cv::Point position;
    std::array<uchar, Size*Size> data;
};

template<int CornerSize>
std::vector<corner<CornerSize>> corners_detector(const cv::Mat & image) {
    cv::Mat result;
    cv::equalizeHist(image, result);
    result = fast_detector(image, 50);
    cv::normalize(result, result, 0, 255, cv::NORM_MINMAX);
    result = local_maxima(result, 4);
    cv::threshold(result, result, 255. * 0.8, 255, cv::THRESH_BINARY);
    
    std::vector<corner<CornerSize>> vec;
    auto height = result.size().height;
    auto width = result.size().width;
    auto* pResult = result.data;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++, pResult++) {
            if (*pResult != 0) {
                if (i >= 4 && i < height - 4 &&
                    j >= 4 && j < width - 4)
                {
                    std::array<uchar, CornerSize*CornerSize> array;
                    auto it = array.begin();
                    for (int di = -CornerSize / 2; di <= CornerSize / 2; di++) {
                        auto* pCorner = image.data + i * width + j + di * width - CornerSize;
                        for (int dj = -CornerSize / 2; dj <= CornerSize / 2; dj++, it++, pCorner++) {
                            *it = *pCorner;
                        }
                    }
                    vec.push_back({ cv::Point(j, i), array });
                }
            }
        }
    }
    return vec;
}

template<int CornerSize>
std::vector<std::pair<corner<CornerSize>, corner<CornerSize>>> pair_corners(
    std::vector<corner<CornerSize>> corners1,
    std::vector<corner<CornerSize>> corners2
) {
    std::vector<std::pair<corner<9>, corner<9>>> pairs;
    for (auto corner1 : corners1) {
        double bestScore = std::numeric_limits<double>::infinity();
        corner<9> bestCorner;
        for (auto corner2 : corners2) {
            double sum = 0;
            for (auto it1 = corner1.data.begin(), it2 = corner2.data.begin();
                it1 != corner1.data.end() && it2 != corner2.data.end();
                it1++, it2++)
            {
                double absDiff = std::abs(*it1 - *it2);
                sum += absDiff * absDiff;
            }
            if (sum < bestScore) {
                bestScore = sum;
                bestCorner = corner2;
            }
        }
        pairs.push_back({ corner1, bestCorner });
    }
    return pairs;
}

#endif