#include <iostream>
#include <ctime>
#include <opencv2/opencv.hpp>
#include "arguments.hpp"
#include "corner_detection.hpp"

int main(int argc, char** argv)
{
    arguments_t args(argc, argv);
    if (args.parametersSize() < 1)
    {
        std::cout << "Usage: " << argv[0] << " <image>" << std::endl;
        return 0;
    }

    auto image = cv::imread(args[0], cv::IMREAD_GRAYSCALE);
    if (!image.data)
    {
        std::cout << "Could not load image '" << args[0] << "'" << std::endl;
        return 0;
    }

    int threshold = 50;
    
    auto start = std::clock();
    auto my_corners = find_corners<9>(image, threshold);
    std::cout << "My FAST: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
    cv::Mat my_fast = image.clone();
    for (auto corner : my_corners)
        cv::rectangle(my_fast, cv::Rect(corner.position.x - 4, corner.position.y - 4, 9, 9), 255);

    std::vector<cv::KeyPoint> cv_corners;
    start = std::clock();
    cv::FAST(image, cv_corners, threshold, true, cv::FastFeatureDetector::TYPE_7_12);
    std::cout << "OpenCV FAST: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
    cv::Mat cv_fast = image.clone();
    for (auto corner : cv_corners)
        cv::rectangle(cv_fast, cv::Rect(corner.pt.x - 4, corner.pt.y - 4, 9, 9), 255);

    cv::imwrite("my_fast.png", my_fast);
    cv::imwrite("opencv_fast.png", cv_fast);

    cv::imshow("my fast", my_fast);
    cv::imshow("cv fast", cv_fast);
    cv::waitKey();
}