#pragma once

#include "Str.h"
#include <opencv2/opencv.hpp>

namespace imcmp {

int get_file_size(const Str256& filepath);
bool file_exist(const char* filename);
bool file_exist(const std::string& filename);

// support .jpg/.png/.bmp,  .nv21/.nv12/.rgb24/.bgr24
cv::Mat loadImage(const std::string& image_path);

} // namespace imcmp