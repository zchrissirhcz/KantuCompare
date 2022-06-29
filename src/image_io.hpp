#pragma once

#include "Str.h"
#include <opencv2/opencv.hpp>

namespace imcmp {

int get_file_size(const Str256& filepath);
bool file_exist(const char* filename);
bool file_exist(const std::string& filename);

std::vector<std::string> get_supported_image_file_exts();
cv::Mat load_image(const std::string& image_path);


} // namespace imcmp