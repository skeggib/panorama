#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include "arguments.hpp"
#include "panorama.hpp"

int main(int argc, char** argv) {
    arguments_t args(argc, argv);
    if (args.parametersSize() < 2) {
        std::cout << "Usage: " << argv[0] << " <image1> <image2>" << std::endl;
        return 0;
    }

    // Open image 1
    auto image1path = args[0];
    auto image1 = cv::imread(image1path, cv::IMREAD_GRAYSCALE);
    if (!image1.data) {
        std::cout << "Could not load '" << image1path << "'." << std::endl;
        return 0;
    }

    // Open image 2
    auto image2path = args[1];
    auto image2 = cv::imread(image2path, cv::IMREAD_GRAYSCALE);
    if (!image2.data) {
        std::cout << "Could not load '" << image2path << "'." << std::endl;
        return 0;
    }

    auto start = std::clock();

    // Find corners in images
    auto corners1 = corners_detector<9>(image1);
    auto corners2 = corners_detector<9>(image2);
    auto detectionTime = std::clock();
    std::cout << "Corners detection: " << (detectionTime - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

    // Find pair of corners
    auto pairs = pair_corners<9>(corners1, corners2);
    auto pairingTime = std::clock();
    std::cout << "Corners pairing: " << (pairingTime - detectionTime) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

    // Find transformation
    std::vector<std::pair<cv::Point, cv::Point>> selectedPairs;
    auto transformMat = find_transform(pairs, selectedPairs);
    auto findTransformTime = std::clock();
    std::cout << "Finding transform: " << (findTransformTime - pairingTime) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

    auto p00 = transform(cv::Point(0, 0), transformMat);
    auto p10 = transform(cv::Point(image1.size().width - 1, 0), transformMat);
    auto p01 = transform(cv::Point(0, image1.size().height - 1), transformMat);
    auto p11 = transform(cv::Point(image1.size().width - 1, image1.size().height - 1), transformMat);

    auto minX = std::min<int>({ p00.x, p10.x, p01.x, p11.x, 0 });
    auto maxX = std::max<int>({ p00.x, p10.x, p01.x, p11.x, image2.size().width - 1 });
    auto minY = std::min<int>({ p00.y, p10.y, p01.y, p11.y, 0 });
    auto maxY = std::max<int>({ p00.y, p10.y, p01.y, p11.y, image2.size().height - 1 });
    
    cv::Mat inverseTransformMat;
    cv::invert(transformMat, inverseTransformMat);
    cv::Mat panoramaImage(cv::Size(maxX - minX + 1, maxY - minY + 1), CV_8UC1, UCHAR_MAX);
    image2.copyTo(panoramaImage(cv::Rect(-minX, -minY, image2.size().width, image2.size().height)));
    auto* pPanorama = panoramaImage.data;
    for (int i = 0; i < panoramaImage.size().height; i++) {
        for (int j = 0; j < panoramaImage.size().width; j++, pPanorama++) {
            cv::Point p(j + minX, i + minY);
            auto pp = transform(p, inverseTransformMat);
            if (pp.x >= 0 && pp.x < image1.size().width - 1 &&
                pp.y >= 0 && pp.y < image1.size().height - 1)
                *pPanorama = (uchar)(((int)image1.at<uchar>(pp) + (int)*pPanorama) / 2);
        }
    }
    
    // Draw rectangles around corners in image 1
    for (const auto& corner : corners1) {
        cv::rectangle(image1, cv::Rect(corner.position.x - 4, corner.position.y - 4, 9, 9), UCHAR_MAX);
    }
    
    // Draw rectangles around corners in image 2
    for (const auto& corner : corners2) {
        cv::rectangle(image2, cv::Rect(corner.position.x - 4, corner.position.y - 4, 9, 9), UCHAR_MAX);
    }

    // Create image containing both image 1 and 2
    cv::Mat images(
        std::max(image1.size().height, image2.size().height),
        image1.size().width + image2.size().width,
        CV_8UC1);
    auto roiImage1 = images(cv::Rect(0, 0, image1.size().width, image1.size().height));
    auto roiImage2 = images(cv::Rect(image1.size().width, 0, image2.size().width, image2.size().height));
    image1.copyTo(roiImage1);
    image2.copyTo(roiImage2);

    // Draws lines for pairs of corners
    for (auto pair : selectedPairs) {
        auto p1 = pair.first;
        auto p2 = pair.second;
        p2.x += image1.size().width;
        cv::line(images, p1, p2, UCHAR_MAX);
    }

    cv::imshow("corners", panoramaImage);
    cv::waitKey();

    return 0;
}
