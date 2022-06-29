#pragma once

#include <opencv2/opencv.hpp>

namespace imcmp {

void getDiffImage(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& diff, int thresh, cv::Scalar below, cv::Scalar above);
cv::Mat compare_two_mat(const cv::Mat& image_left, const cv::Mat& image_right, int toleranceThresh, bool& is_exactly_same);

} // namespace imcmp
