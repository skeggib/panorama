#include "skeletonization.hpp"

cv::Mat skeletonize(const cv::Mat& image) {
    cv::Mat result(image);
    auto* pResult = result.data;
    auto width = image.size().width;
    for (int i = 1; i < image.size().height - 1; i++) {
        for (int j = 1; j < width - 1; j++, pResult++) {
            if (*(pResult - 1) > 0 && *pResult > *(pResult - 1))
                continue;
            if (*(pResult + 1) > 0 && *pResult > *(pResult + 1))
                continue;
            if (*(pResult - width) > 0 && *pResult > *(pResult - width))
                continue;
            if (*(pResult + width) > 0 && *pResult > *(pResult + width))
                continue;
            if (*(pResult - 1) != 0 ||
                *(pResult + 1) != 0 ||
                *(pResult - width) != 0 ||
                *(pResult + width) != 0)
                *pResult = 0;
        }
    }
    for (int i = image.size().height - 2; i > 1; i--) {
        for (int j = width - 2; j > 1; j--, pResult--) {
            if (*(pResult - 1) > 0 && *pResult > *(pResult - 1))
                continue;
            if (*(pResult + 1) > 0 && *pResult > *(pResult + 1))
                continue;
            if (*(pResult - width) > 0 && *pResult > *(pResult - width))
                continue;
            if (*(pResult + width) > 0 && *pResult > *(pResult + width))
                continue;
            if (*(pResult - 1) != 0 ||
                *(pResult + 1) != 0 ||
                *(pResult - width) != 0 ||
                *(pResult + width) != 0)
                *pResult = 0;
        }
    }
    return result;
}