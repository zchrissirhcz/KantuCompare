#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace kantu {

// https://github.com/FFmpeg/FFmpeg/blob/master/libavutil/pixfmt.h
enum class PixelFormat
{
    PIX_FMT_NONE = -1,
    AV_PIX_FMT_YUV420P,
    PIX_FMT_GRAY8,
    PIX_FMT_BGR8,
    PIX_FMT_RGB8,
    PIX_FMT_NV12,
    PIX_FMT_NV21,
};

class Image
{
public:
    int format;
    std::vector<cv::Mat> planes;
};

} // namespace kantu