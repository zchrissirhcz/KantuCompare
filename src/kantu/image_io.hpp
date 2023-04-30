#pragma once

#include "kantu/image.hpp"
#include <string>
#include "str/Str.h"
#include "kantu/string.hpp"

namespace kantu {

class FourccFileInfo
{
public:
    FourccFileInfo() 
    {
    }
    FourccFileInfo(const FilePath& path);

public:
    std::string filepath = "";
    std::string head = "";
    std::string ext = ""; // same as file
    std::string mapped_ext; // converted to lowercase, then mapping to identical one
    int height = 0;
    int width = 0;
    bool valid = false;
    std::string err_msg = "";
    int filesize = 0;
};

cv::Mat convert_fourcc_to_mat(const FourccImage& tu);

FourccImage loadImage(const std::string path);
void saveImage(const std::string path, FourccImage& image);


int get_file_size(const Str256& filepath);
bool file_exist(const char* filename);
bool file_exist(const std::string& filename);

std::vector<std::string> get_supported_image_file_exts();
cv::Mat load_as_displayable_image(const std::string& image_path);

} // namespace kantu