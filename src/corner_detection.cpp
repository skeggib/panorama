#include "corner_detection.hpp"

#include <vector>

uchar is_corner(const std::array<uchar, 16> & circle, uchar pixel, int threshold);
std::array<uchar, 16> circle(uchar* pixel, int width);

cv::Mat fast_detector(const cv::Mat & image, int threshold) {

    cv::Mat corners(image.size(), image.type());

    auto* pImage = image.data;
    auto* pCorners = corners.data;
    for (int i = 0; i < image.size().height; i++) {
        for (int j = 0; j < image.size().width; j++, pImage++, pCorners++) {
            if (i >= 3 && i < image.size().height - 3 &&
                j >= 3 && j < image.size().width - 3) {
                auto array = circle(pImage, image.size().width);
                auto value = is_corner(array, *pImage, threshold);
                if (value >= 12)
                    *pCorners = value;
                else
                    *pCorners = 0;
            }
            else
                *pCorners = 0;
        }
    }

    return corners;
}

uchar is_corner(const std::array<uchar, 16> & circle, uchar pixel, int threshold) {
    int maxCounter = 0;
    for (int i = 0; i < circle.size(); i++) {
        uchar min, max;
        if (circle[i] < pixel - threshold) {
            min = 0;
            max = pixel - threshold;
        }

        else if (circle[i] > pixel + threshold) {
            min = pixel + threshold;
            max = 255;
        }

        else
            continue;

        int counter = 1;
        for (int j = 1; j < circle.size(); j++) {
            int index = (i + j) % circle.size();
            if (circle[index] < min || circle[index] > max)
                break;
            counter++;
        }

        maxCounter = std::max(maxCounter, counter);
    }
    return maxCounter;
}

std::array<uchar, 16> circle(uchar* pixel, int width) {
    std::array<uchar, 16> array;
    pixel -= 3*width;
    
    array[0] = *pixel++;
    array[1] = *pixel++;
    
    pixel += width;
    array[2] = *pixel++;
    
    pixel += width;
    array[3] = *pixel;
    pixel += width;
    array[4] = *pixel;
    pixel += width;
    array[5] = *pixel--;
    
    pixel += width;
    array[6] = *pixel--;
    
    pixel += width;
    array[7] = *pixel--;
    array[8] = *pixel--;
    array[9] = *pixel--;

    pixel -= width;
    array[10] = *pixel--;
    
    pixel -= width;
    array[11] = *pixel;
    pixel -= width;
    array[12] = *pixel;
    pixel -= width;
    array[13] = *pixel++;

    pixel -= width;
    array[14] = *pixel++;
    
    pixel -= width;
    array[15] = *pixel;
    
    return array;
}

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