#ifndef PANORAMA_HPP
#define PANORAMA_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <array>
#include <cstdlib>
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
std::vector<std::pair<cv::Point, cv::Point>> pair_corners(
    std::vector<corner<CornerSize>> corners1,
    std::vector<corner<CornerSize>> corners2
) {
    std::vector<std::pair<cv::Point, cv::Point>> pairs;
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
        pairs.push_back({ corner1.position, bestCorner.position });
    }
    return pairs;
}

cv::Point transform(cv::Point p, cv::Mat t) {
    cv::Mat m(3, 1, CV_32F);
    m.at<float>(0, 0) = (float)p.x;
    m.at<float>(1, 0) = (float)p.y;
    m.at<float>(2, 0) = 1;
    cv::Mat r = t * m;
    r /= r.at<float>(2, 0);
    cv::Point pp;
    pp.x = (int)r.at<float>(0, 0);
    pp.y = (int)r.at<float>(1, 0);
    return pp;
}

cv::Mat find_transform(std::vector<std::pair<cv::Point, cv::Point>> pairs, std::vector<std::pair<cv::Point, cv::Point>>& selectedPairs) {
    std::srand((uint)std::time(nullptr));

    int bestScore = 0;
    cv::Mat bestTransform(3, 3, CV_32F);
    for (int iter = 0; iter < 1000; iter++) {
        std::array<int, 5> indexes;
        for (int i = 0; i < indexes.size(); i++) {
            indexes[i] = std::rand() % pairs.size();
            bool exists = false;
            for (int j = 0; j < i && !exists; j++)
                if (indexes[i] == indexes[j])
                    exists = true;
            if (exists)
                i--;
        }

        selectedPairs.clear();
        for (int i = 0; i < indexes.size(); i++)
            selectedPairs.push_back(pairs[indexes[i]]);

        cv::Mat A(10, 9, CV_32F);
        for (int i = 0; i < selectedPairs.size(); i++) {
            const auto& pair = selectedPairs[i];
            A.at<float>(i*2, 0) = (float)pair.first.x;
            A.at<float>(i*2, 1) = (float)pair.first.y;
            A.at<float>(i*2, 2) = 1.;
            for (int j = 3; j < 6; j++)
                A.at<float>(i*2, j) = 0.;
            A.at<float>(i*2, 6) = (float)-pair.second.x * pair.first.x;
            A.at<float>(i*2, 7) = (float)-pair.second.x * pair.first.y;
            A.at<float>(i*2, 8) = (float)-pair.second.x;
            
            for (int j = 0; j < 3; j++)
                A.at<float>(i*2+1, j) = 0.;
            A.at<float>(i*2+1, 3) = (float)pair.first.x;
            A.at<float>(i*2+1, 4) = (float)pair.first.y;
            A.at<float>(i*2+1, 5) = 1.;
            A.at<float>(i*2+1, 6) = (float)-pair.second.y * pair.first.x;
            A.at<float>(i*2+1, 7) = (float)-pair.second.y * pair.first.y;
            A.at<float>(i*2+1, 8) = (float)-pair.second.y;
        }
        cv::Mat b(9, 1, CV_32F);
        for (int i = 0; i < b.size().height - 1; i++)
            b.at<float>(i, 0) = 0.;
        b.at<float>(b.size().height - 1, 0) = 1.;
        cv::Mat x;
        cv::solve(A, b, x, cv::DECOMP_SVD);
        
        cv::Mat transformMat(3, 3, CV_32F);
        auto* pTransformMat = transformMat.data;
        auto* pX = x.data;
        for (int i = 0; i < x.size().height * sizeof(float); i++, pTransformMat++, pX++)
            *pTransformMat = *pX;

        int score = 0;
        for (auto pair : pairs) {
            auto im = transform(pair.first, transformMat);
            double xd = im.x - pair.second.x;
            double yd = im.y - pair.second.y;
            if (xd*xd + yd*yd < 10)
                score++;
        }
        if (score >= bestScore) {
            std::cout << score << std::endl;
            bestScore = score;
            auto* pBestTransform = bestTransform.data;
            pX = x.data;
            for (int i = 0; i < x.size().height * sizeof(float); i++, pBestTransform++, pX++)
                *pBestTransform = *pX;
        }
    }
    
    return bestTransform;
}

#endif