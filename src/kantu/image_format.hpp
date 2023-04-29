#pragma once

#include "kantu/image.hpp"

namespace kantu {

enum class Transformer {
    RGB_to_GRAY,
    RGB_to_BGR
};

void transformFormat(const Image& src, Image& dst, const Transformer& transformer);

void chw_to_hwc(const cv::Mat& src, cv::Mat& dst);
void my_chw_to_hwc(cv::InputArray src, cv::OutputArray dst);
void i444_to_rgb(uint8_t* i444, uint8_t* bgr, int width, int height);

} // namespace kantu