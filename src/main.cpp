#include <iostream>
#include <opencv2/opencv.hpp>

#include "arguments.hpp"
#include "fast_detector.hpp"
#include "distance.hpp"
#include "local_maxima.hpp"
#include "skeletonization.hpp"

int main(int argc, char** argv) {
    arguments_t args(argc, argv);
    if (args.parametersSize() == 0) {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 0;
    }

    auto filename = args[0];
    auto image = cv::imread(filename, cv::IMREAD_GRAYSCALE);
    if (!image.data) {
        std::cout << "Could not load '" << filename << "'." << std::endl;
        return 0;
    }

    cv::Mat result;
    cv::equalizeHist(image, result);
    result = fast_detector(image, 50);
    cv::normalize(result, result, 0, 255, cv::NORM_MINMAX);
    result = local_maxima(result, 4);
    cv::threshold(result, result, 255. * 0.8, 255, cv::THRESH_BINARY);
    
    auto* pMaxima = result.data;
    for (int i = 0; i < result.size().height; i++) {
        for (int j = 0; j < result.size().width; j++, pMaxima++) {
            if (*pMaxima == UCHAR_MAX)
                cv::rectangle(image, cv::Rect(j - 4, i - 4, 8, 8), UCHAR_MAX);
        }
    }

    cv::imwrite("out.png", image);
    cv::imshow("image", image);
    cv::waitKey();

    return 0;
}
