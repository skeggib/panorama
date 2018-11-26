#include "transformation.hpp"

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

cv::Mat find_transform(
    std::vector<std::pair<cv::Point, cv::Point>> pairs, 
    std::vector<std::pair<cv::Point, cv::Point>>& selectedPairs
) {
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
        cv::Mat b(9, 1, CV_32F, 0.);
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
            bestScore = score;
            auto* pBestTransform = bestTransform.data;
            pX = x.data;
            for (int i = 0; i < x.size().height * sizeof(float); i++, pBestTransform++, pX++)
                *pBestTransform = *pX;
        }
    }
    
    return bestTransform;
}