#include "distance.hpp"

cv::Mat distance(const cv::Mat & image) {
    cv::Mat result(image.size(), CV_8UC1);
    auto* pImage = image.data;
    auto* pResult = result.data;
    auto width = image.size().width;
    for (int i = 0; i < image.size().height; i++) {
        for (int j = 0; j < width; j++, pImage++, pResult++) {
            if (*pImage != 0)
                *pResult = std::min(*(pResult - 1), *(pResult - width)) + 1;
            else
                *pResult = 0;
        }
    }
    for (int i = 0; i < image.size().height; i++) {
        for (int j = 0; j < width; j++, pImage--, pResult--) {
            if (*pImage != 0)
                *pResult = std::min(
                    std::min(*(pResult - 1), *(pResult - width)),
                    std::min(*(pResult + 1), *(pResult + width))) + 1;
            else
                *pResult = 0;
        }
    }
    return result;
}