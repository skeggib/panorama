#include "local_maxima.hpp"

cv::Mat local_maxima(const cv::Mat& image, int n) {
    cv::Mat result(image.size(), image.type());
    auto height = image.size().height;
    auto width = image.size().width;
    auto* pImage = image.data;
    auto* pResult = result.data;
    
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++, pImage++, pResult++) {
            bool isLocalMax = true;
            for (int di = std::max(-n, -i); di <= std::min(n, height - 1 - i) && isLocalMax; di++) {
                for (int dj = std::max(-n, -j); dj <= std::min(n, width - 1 - j) && isLocalMax; dj++) {
                    if (di == 0 && dj == 0)
                        continue;
                    if (*pImage < *(pImage + di * width + dj))
                        isLocalMax = false;
                }
            }
            if (isLocalMax)
                *pResult = *pImage;
            else
                *pResult = 0;
        }
    }
    
    return result;
}

cv::Mat local_maxima(const cv::Mat& image, int windowWidth, int windowHeight) {
    cv::Mat result(image);
    auto height = image.size().height;
    auto width = image.size().width;
    for (int wi = 0; wi < height / windowHeight + 1; wi++) {
        for (int wj = 0; wj < width / windowWidth + 1; wj++) {
            int blockOffset = width * wi * windowHeight + wj * windowWidth;
            uchar max = 0;
            for (int i = 0; i < windowHeight && wi * windowHeight + i < height; i++) {
                int lineOffset = width * i;
                auto* pImage = image.data + blockOffset + lineOffset;
                for (int j = 0; j < windowWidth && wj * windowWidth + j < width; j++) {
                    max = std::max(max, *pImage);
                    pImage++;
                }
            }
            bool maxFound = false;
            for (int i = 0; i < windowHeight && wi * windowHeight + i < height; i++) {
                int lineOffset = width * i;
                auto* pImage = image.data + blockOffset + lineOffset;
                auto* pResult = result.data + blockOffset + lineOffset;
                for (int j = 0; j < windowWidth && wj * windowWidth + j < width; j++) {
                    if (!maxFound && *pImage == max) {
                        maxFound = true;
                        *pResult = *pImage;
                    }
                    else
                        *pResult = 0;
                    pImage++;
                    pResult++;
                }
            }
        }
    }
    return result;
}