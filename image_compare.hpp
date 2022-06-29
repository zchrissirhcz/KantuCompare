#pragma once

#include <opencv2/opencv.hpp>

namespace imcmp {

void getDiffImage(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& diff, int thresh, cv::Scalar below, cv::Scalar above);

} // namespace imcmp
