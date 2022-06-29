#pragma once

#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include "Str.h"

namespace imcmp {

GLuint getTextureFromImage(const cv::Mat& image);

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

    void loadFromFile(const Str256& filepath);
    void reload();
    void load_mat(cv::Mat& frame);
    void update_mat(cv::Mat& frame, bool change_color_order = false);
    // clear texture and realease all memory associated with it
    void clear();
    GLuint get_texture() const;
    bool* get_open();
    void set_name(const Str256& _name);
    //then GetName
    const char* get_name();
};

} // namespace imcmp