#include <stdio.h>
#include <opencv2/opencv.hpp>

#ifndef STR_IMPLEMENTATION
#define STR_IMPLEMENTATION 1
#endif
#include "image_io.hpp"

static void help(const char* exe_name)
{
    printf("Usage: %s image_path\n", exe_name);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        help(argv[0]);
        return 1;
    }

    std::string filename = argv[1];

    cv::Mat image = imcmp::loadImage(filename);
    std::string win_name = filename;
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, cv::Size{600, 800});
    cv::imshow(win_name, image);
    //cv::imwrite("result.png", image);
    cv::waitKey(0);

    return 0;
}