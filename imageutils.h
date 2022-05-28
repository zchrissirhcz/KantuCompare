#pragma once

#include <opencv2/opencv.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <sstream>
#include <iostream>

static GLuint getTextureFromImage(const cv::Mat& image)
{
    cv::Mat im0 = image;
    // if (image.channels() == 3)
    // {
    //     cv::cvtColor(image, im0, cv::COLOR_BGR2BGRA);
    // }

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.size().width, image.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, im0.data);

    return image_texture;
}

class RichImage
{
public:
    GLuint texture;
    cv::Mat mat;
    bool open;
    std::string name;
    
public:
    RichImage(): texture(0), open(false) {}
    
    void load_mat(cv::Mat& frame)
    {
        if (frame.empty())
            return;

        if (!texture)
        {
            //open = true;
            mat = frame; // maybe i should copy that frame (clone it)
            texture = getTextureFromImage(mat);
        }
        else
        {
            std::cout << "We HAVE SOMETHING AT TEXTURE" << "\n";
        }
    }

    void update_mat(cv::Mat& frame, bool change_color_order = false)
    {
        // if does not have the same size then i need to recreate the texture from scratch
        //could be something like
        if (!(frame.size() == mat.size()))
        {
            clear();
            mat.release(); // maybe this is redundant
            load_mat(frame);
        } 
        
        if (change_color_order)
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        // image must have same size
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_RGB,
                GL_UNSIGNED_BYTE, frame.ptr());
    }

    // clear texture and realease all memory associated with it
    void clear()
    {
        glDeleteTextures(1, &texture);
        texture = 0;
    }

    GLuint get_texture() const
    {
        return texture;
    }

    bool* get_open()
    {
        return &open;
    }

    void set_name(const char* _name)
    {
        name = _name;
    }

    //then GetName
    const char* get_name()
    {
        return name.c_str();
    }
};
