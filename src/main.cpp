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

    std::cout << "Corners pairing: " << (std::clock() - detectionTime) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
    
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
    for (auto pair : pairs) {
        auto p1 = pair.first.position;
        auto p2 = pair.second.position;
        p2.x += image1.size().width;
        cv::line(images, p1, p2, UCHAR_MAX);
    }

    cv::imshow("corners", images);
    cv::waitKey();

    return 0;
}
