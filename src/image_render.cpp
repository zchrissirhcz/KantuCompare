#include "image_render.hpp"
#include "image_io.hpp"

GLuint imcmp::getTextureFromImage(const cv::Mat& image)
{
    cv::Mat im0 = image;
#if _MSC_VER
    im0 = image.clone();
#endif

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if !_MSC_VER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
#endif
    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif

    const int channels = image.channels();
    GLint internalformat = 0;
    GLenum format = 0;
    bool valid_format = true;
    if (channels == 4)
    {
        internalformat = GL_RGBA;
#if _MSC_VER
        format = GL_RGBA;
        cvtColor(im0, im0, cv::COLOR_BGRA2RGBA);
#else
        format = GL_BGRA;
#endif
    }
    else if (channels == 3)
    {
        internalformat = GL_RGB;
#if _MSC_VER
        format = GL_RGB;
        cvtColor(im0, im0, cv::COLOR_BGR2RGB);
#else
        format = GL_BGR;
#endif
    }
    else if (channels == 1)
    {
        internalformat = GL_LUMINANCE;
        format = GL_LUMINANCE;
    }
    else
    {
        valid_format = false;
        fprintf(stderr, "only support 1, 3, 4 channels\n");
    }

    if (valid_format)
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, im0.size().width, im0.size().height, 0, format, GL_UNSIGNED_BYTE, im0.data);

    return image_texture;
}

namespace imcmp {

void RichImage::load_from_file(const Str256& filepath)
{
    //cv::Mat mat = cv::imread(filepath.c_str(), cv::IMREAD_UNCHANGED);
    std::string imagepath = filepath.c_str();
    cv::Mat mat = load_image(imagepath);
    if (mat.empty()) return;
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
    filesize = imcmp::get_file_size(filepath);
}

void RichImage::reload()
{
    if (name.length() > 0)
    {
        load_from_file(name.c_str());
    }
}

void RichImage::load_mat(cv::Mat& frame)
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

void RichImage::update_mat(cv::Mat& frame, bool change_color_order)
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
void RichImage::clear()
{
    glDeleteTextures(1, &texture);
    texture = 0;
}

GLuint RichImage::get_texture() const
{
    return texture;
}

bool* RichImage::get_open()
{
    return &open;
}

void RichImage::set_name(const Str256& _name)
{
    name = _name.c_str();
}

//then GetName
const char* RichImage::get_name()
{
    return name.c_str();
}

} // namespace imcmp