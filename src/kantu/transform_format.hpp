#pragma once

#include "kantu/image.hpp"
#include <unordered_map>

namespace kantu {

enum class Transformer {
    RGB_to_GRAY,
    RGB_to_BGR
};

void transformFormat(const FourccImage& src, FourccImage& dst, const Transformer& transformer);

void chw_to_hwc(const cv::Mat& src, cv::Mat& dst);
void my_chw_to_hwc(cv::InputArray src, cv::OutputArray dst);
void i444_to_rgb(uint8_t* i444, uint8_t* bgr, int width, int height);

class PlaneInfo
{
public:
    PlaneInfo(int _height, int _width, int _channels):
        height(_height), width(_width), channels(_channels)
    {}

public:
    int height;
    int width;
    int channels;
};

} // namespace kantu