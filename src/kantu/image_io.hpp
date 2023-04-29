#pragma once

#include "kantu/image.hpp"
#include <string>
#include "Str.h"

namespace kantu {

class ImageFileInfo
{
public:
    ImageFileInfo() 
    {
    }
    ImageFileInfo(const std::string& filename);

public:
    std::string filename = "";
    std::string head = "";
    std::string ext = ""; // same as file
    std::string lower_ext; // converted to lowercase, then mapping to identical one
    int height = 0;
    int width = 0;
    bool valid = false;
    std::string err_msg = "";
};

// string utilities
std::string to_lower(const std::string& str);
std::string& replace_all(std::string& src, const std::string& old_value, const std::string& new_value);


Image loadImage(const std::string path);
void saveImage(const std::string path, Image& image);


int get_file_size(const Str256& filepath);
bool file_exist(const char* filename);
bool file_exist(const std::string& filename);

std::vector<std::string> get_supported_image_file_exts();
cv::Mat load_image(const std::string& image_path);

} // namespace kantu