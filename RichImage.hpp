#pragma once

#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <GLFW/glfw3.h>
#include <string>

static GLuint getTextureFromImage(const cv::Mat& image)
{
    cv::Mat im0 = image;

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

    const int channels = image.channels();
    switch (channels)
    {
    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im0.size().width, im0.size().height, 0, GL_BGRA, GL_UNSIGNED_BYTE, im0.data);
        break;
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, im0.size().width, im0.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, im0.data);
        break;
    case 1:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, im0.size().width, im0.size().height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, im0.data);
        break;
    default:
        fprintf(stderr, "only support 1, 3, 4 channels\n");
    }

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

    void loadFromFile(const char* filepath)
    {
        cv::Mat mat = cv::imread(filepath, cv::IMREAD_UNCHANGED); 
        switch (mat.channels())
        {
        case 1:
            cv::cvtColor(mat, mat, cv::COLOR_GRAY2BGRA);
            break;
        case 3:
            cv::cvtColor(mat, mat, cv::COLOR_BGR2BGRA);
            break;
        case 4:
            // NOP
            break;
        default:
            printf("only support 1, 3, 4 channels\n"); // TODO: fix me. consider encapsulate cv::imread(), support 2 channels.
        }
        load_mat(mat);
        set_name(filepath);
    }
    
    void load_mat(cv::Mat& frame)
    {
        if (frame.empty())
            return;

        if (!texture)
        {
            open = true;
            mat = frame; // maybe i should copy that frame (clone it)
            texture = getTextureFromImage(mat);
        }
        else
        {
            //printf("We HAVE SOMETHING AT TEXTURE\n");
            clear();
            open = true;
            mat = frame;
            texture = getTextureFromImage(mat);
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
