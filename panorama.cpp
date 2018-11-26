#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include "arguments.hpp"
#include "corner_detection.hpp"
#include "transformation.hpp"

int main(int argc, char** argv) {
    arguments_t args(argc, argv);
    if (args.parametersSize() < 3) {
        std::cout << "Usage: " << argv[0] << " <image1> <image2> <output>" << std::endl;
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
    std::cout << "Corners detection... \t";
    auto corners1 = find_corners<9>(image1, 100);
    auto corners2 = find_corners<9>(image2, 100);
    auto detectionTime = std::clock();
    std::cout << (detectionTime - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

    // Find pair of corners
    std::cout << "Corners pairing... \t";
    auto pairs = pair_corners<9>(corners1, corners2);
    auto pairingTime = std::clock();
    std::cout << (pairingTime - detectionTime) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

    // Find transformation
    std::cout << "Finding transform... \t";
    std::vector<std::pair<cv::Point, cv::Point>> selectedPairs;
    auto transformMat = find_transform(pairs, selectedPairs);
    auto findTransformTime = std::clock();
    std::cout << (findTransformTime - pairingTime) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

    // Transforming image
    std::cout << "Transforming image... \t";
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
                *pPanorama = image1.at<uchar>(pp);
        }
    }
    
    auto transformTime = std::clock();
    std::cout << (transformTime - findTransformTime) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

    cv::imwrite(args[2], panoramaImage);
    cv::imshow("corners", panoramaImage);
    cv::waitKey();

    return 0;
}
