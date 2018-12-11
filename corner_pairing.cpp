#include "arguments.hpp"
#include "corner_detection.hpp"
#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
    arguments_t args(argc, argv);
    if (args.parametersSize() < 2)
    {
        std::cout << "Usage: " << argv[0] << " <image1> <image2>" << std::endl;
        return 0;
    }

    auto image1 = cv::imread(args[0], cv::IMREAD_GRAYSCALE);
    if (!image1.data)
    {
        std::cout << "Could not load image '" << args[0] << "'" << std::endl;
        return 0;
    }

    auto image2 = cv::imread(args[1], cv::IMREAD_GRAYSCALE);
    if (!image2.data)
    {
        std::cout << "Could not load image '" << args[1] << "'" << std::endl;
        return 0;
    }

    int threshold = 50;
    const int cornerSize = 9;
    auto corners1 = find_corners<cornerSize>(image1, threshold);
    auto corners2 = find_corners<cornerSize>(image2, threshold);

    auto pairs = pair_corners(corners1, corners2);

    cv::Size size
    {
        image1.size().width + image2.size().width,
        std::max(image1.size().height, image2.size().height)
    };

    cv::Mat result(size, image1.type());

    image1.copyTo(result(cv::Rect(0, 0, image1.size().width, image1.size().height)));
    image2.copyTo(result(cv::Rect(image1.size().width, 0, image2.size().width, image2.size().height)));

    for (auto pair : pairs)
    {
        cv::Point p1(pair.first.x, pair.first.y);
        cv::Point p2(pair.second.x + image1.size().width, pair.second.y);

        cv::line(result, p1, p2, 255);
        cv::rectangle(result, cv::Rect(p1.x - cornerSize / 2, p1.y - cornerSize / 2, cornerSize, cornerSize), 255);
        cv::rectangle(result, cv::Rect(p2.x - cornerSize / 2, p2.y - cornerSize / 2, cornerSize, cornerSize), 255);
    }

    cv::imwrite("pairs.png", result);
    cv::imshow("result", result);
    cv::waitKey();

    return 0;
}