#ifndef CORNER_DETECTION_HPP
#define CORNER_DETECTION_HPP

#include <vector>
#include <opencv2/opencv.hpp>

template <int Size>
struct corner
{
    cv::Point position;
    std::array<uchar, Size * Size> data;
};

cv::Mat local_maxima(const cv::Mat &image, int n);
cv::Mat fast_detector(const cv::Mat &image, int threshold);

template <int CornerSize>
std::vector<corner<CornerSize>> find_corners(const cv::Mat &image, int threshold)
{
    cv::Mat result;
    cv::equalizeHist(image, result);
    result = fast_detector(image, threshold);
    cv::normalize(result, result, 0, 255, cv::NORM_MINMAX);
    result = local_maxima(result, 4);
    cv::threshold(result, result, 1, 255, cv::THRESH_BINARY);

    std::vector<corner<CornerSize>> vec;
    auto height = result.size().height;
    auto width = result.size().width;
    auto *pResult = result.data;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++, pResult++)
        {
            if (*pResult != 0)
            {
                if (i >= 4 && i < height - 4 &&
                    j >= 4 && j < width - 4)
                {
                    std::array<uchar, CornerSize * CornerSize> array;
                    auto it = array.begin();
                    for (int di = -CornerSize / 2; di <= CornerSize / 2; di++)
                    {
                        auto *pCorner = image.data + i * width + j + di * width - CornerSize;
                        for (int dj = -CornerSize / 2; dj <= CornerSize / 2; dj++, it++, pCorner++)
                        {
                            *it = *pCorner;
                        }
                    }
                    vec.push_back({cv::Point(j, i), array});
                }
            }
        }
    }
    return vec;
}

template <int CornerSize>
std::vector<std::pair<cv::Point, cv::Point>> pair_corners(
    std::vector<corner<CornerSize>> corners1,
    std::vector<corner<CornerSize>> corners2)
{
    auto a2b = _pair_corners(corners1, corners2);
    auto b2a = _pair_corners(corners2, corners1);

    std::vector<std::pair<cv::Point, cv::Point>> pairs;
    for (auto pair1 : a2b)
    {
        auto pair2 = std::find_if(b2a.begin(), b2a.end(), [pair1](auto p) -> bool { return p.second == pair1.first; });
        if (pair2 != b2a.end() && pair2->first == pair1.second)
            pairs.push_back(pair1);
    }

    return pairs;
}

template <typename T, size_t Size>
double mean(std::array<T, Size> vec)
{
    T sum = 0;
    for (auto val : vec)
        sum += val;
    return (double)sum / Size;
}

template <int CornerSize>
std::vector<std::pair<cv::Point, cv::Point>> _pair_corners(
    std::vector<corner<CornerSize>> corners1,
    std::vector<corner<CornerSize>> corners2)
{
    std::vector<std::pair<cv::Point, cv::Point>> pairs;
    for (auto corner1 : corners1)
    {
        double mean1 = mean(corner1.data);
        double bestScore = std::numeric_limits<double>::infinity();
        double secondBestScore = bestScore;
        corner<9> bestCorner;
        for (auto corner2 : corners2)
        {
            double mean2 = mean(corner2.data);
            double sum = 0;
            for (auto it1 = corner1.data.begin(), it2 = corner2.data.begin();
                 it1 != corner1.data.end() && it2 != corner2.data.end();
                 it1++, it2++)
            {
                double val1 = *it1;
                double val2 = *it2;
                double diff = (val1 - mean1) - (val2 - mean2);
                sum += diff * diff;
            }
            if (sum < bestScore)
            {
                secondBestScore = bestScore;
                bestScore = sum;
                bestCorner = corner2;
            }
        }

        if (bestScore * 2 < secondBestScore)
            pairs.push_back({corner1.position, bestCorner.position});
    }
    return pairs;
}

#endif