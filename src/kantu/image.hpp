#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace kantu {

// https://github.com/FFmpeg/FFmpeg/blob/master/libavutil/pixfmt.h
enum class PixelFormat
{
    NONE = -1,
    //AV_PIX_FMT_YUV420P, // i420
    GRAY8,
    BGR24,
    RGB24,
    NV12,
    NV21,

    // not sure now:
    I420,
    YV12,
    UYVY,
    YUYV,
    YVYU,
    I444,
    BGRA32,
    RGBA32,
};

class Image
{
public:
    PixelFormat format;
    std::vector<cv::Mat> planes;
};

} // namespace kantu