#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace kantu {

// https://github.com/FFmpeg/FFmpeg/blob/master/libavutil/pixfmt.h
enum class PixelFormat
{
    NONE = -1,
    //AV_PIX_FMT_YUV420P, // i420

    // 3/2
    NV12,
    NV21,
    I420,
    YV12,

    // 2
    UYVY,
    YUYV,
    YVYU,

    // 3
    I444,
    BGR24,
    RGB24,
    
    // 4
    BGRA32,
    RGBA32,

    // 1
    GRAY8,
};

class FourccImage
{
public:
    PixelFormat format;
    std::vector<cv::Mat> planes;
    cv::Mat view1d;
    cv::Mat view2d;
    int height;
    int width;
};

} // namespace kantu