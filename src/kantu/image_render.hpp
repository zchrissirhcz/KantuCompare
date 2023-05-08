#pragma once

#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include "str/Str.h"

namespace kantu {

GLuint get_texture_from_image(const cv::Mat& image);

class RichImage
{
public:
    GLuint texture;
    cv::Mat mat;
    bool open;
    std::string name;
    int filesize;

public:
    RichImage()
        : texture(0), open(false)
    {
    }

    void load_from_file(const Str256& filepath);
    void reload();
    void load_mat(cv::Mat& frame);
    void update_mat(cv::Mat& frame, bool change_color_order = false);
    void clear(); // clear texture and release all memory associated with it
    GLuint get_texture() const;
    bool* get_open();
    void set_name(const Str256& _name);
    const char* get_name() const;
};

} // namespace kantu