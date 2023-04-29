#include "kantu/image_io.hpp"
#include "kantu/transform_format.hpp"
#include "kantu/string.hpp"
#include <filesystem>
#include <opencv2/imgproc.hpp>
#include <vector>
#include "mlcc/fmt1.h"
#include "log.hpp"

namespace {
using namespace kantu;
using namespace Shadow;

cv::Mat load_fourcc_and_convert_to_mat(const ImageFileInfo& file_info)
{
    cv::Mat image;
    const std::string& ext = file_info.lower_ext;
    if (ext == "nv21" || ext == "nv12" || ext == "i420" || ext == "yv12" // 3/2
        || ext == "uyvy" || ext == "yuyv" || ext == "yvyu" // 2
        || ext == "i444" // 3
        )
    {
        int height = file_info.height;
        int width = file_info.width;
        FILE* fin = fopen(file_info.filepath.c_str(), "rb");
        
        cv::Size size;
        size.height = height;
        size.width = width;
        image = cv::Mat(size, CV_8UC3);

        if (ext == "nv21") // nv21 => bgr
        {
            int buf_size = height * width * 3 / 2;
            cv::Mat yuv420sp_mat(height * 3 / 2, width, CV_8UC1);
            uchar* yuv_buf = yuv420sp_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv420sp_mat, image, cv::COLOR_YUV2BGR_NV21);
        }
        else if (ext == "nv12") // nv12 => bgr
        {
            int buf_size = height * width * 3 / 2;
            cv::Mat yuv420sp_mat(height * 3 / 2, width, CV_8UC1);
            uchar* yuv_buf = yuv420sp_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv420sp_mat, image, cv::COLOR_YUV2BGR_NV12);
        }
        else if (ext == "i420")
        {
            int buf_size = height * width * 3 / 2;
            cv::Mat mat(height * 3 / 2, width, CV_8UC1);
            uchar* yuv_buf = mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(mat, image, cv::COLOR_YUV2BGR_I420);
        }
        else if (ext == "yv12")
        {
            int buf_size = height * width * 3 / 2;
            cv::Mat yuv420p_mat(height * 3 / 2, width, CV_8UC1);
            uchar* yuv_buf = yuv420p_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv420p_mat, image, cv::COLOR_YUV2BGR_YV12);
        }
        else if (ext == "uyvy")
        {
            int buf_size = height * width * 2;
            cv::Mat yuv422_mat(height, width, CV_8UC2);
            uchar* yuv_buf = yuv422_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv422_mat, image, cv::COLOR_YUV2BGR_UYVY);
        }
        else if (ext == "yuyv")
        {
            int buf_size = height * width * 2;
            cv::Mat yuv422_mat(height, width, CV_8UC2);
            uchar* yuv_buf = yuv422_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv422_mat, image, cv::COLOR_YUV2BGR_YUYV);
        }
        else if (ext == "yvyu")
        {
            int buf_size = height * width * 2;
            cv::Mat yuv422_mat(height, width, CV_8UC2);
            uchar* yuv_buf = yuv422_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv422_mat, image, cv::COLOR_YUV2BGR_YVYU);
        }
        else if (ext == "i444")
        {
            int buf_size = height * width * 3;
            cv::Mat yuv444_mat(height, width, CV_8UC3);
            uchar* yuv_buf = yuv444_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
#if 1
            image.create(yuv444_mat.size(), yuv444_mat.type());
            i444_to_rgb(yuv444_mat.data, image.data, width, height);
            cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
#else
            cv::cvtColor(yuv444_mat, image, cv::COLOR_YUV2BGR);
            // cv::Mat tmp;
            // chw_to_hwc(yuv444_mat, tmp);
            
#endif
        }
    }
    else if (ext == "bgr24" || ext == "rgb24" || ext == "rgba32" || ext == "bgra32" || ext == "gray")
    {
        int channels = 1; // "gray"
        if (file_info.lower_ext == "bgr24" || file_info.lower_ext == "rgb24")
        {
            channels = 3;
        }
        else if (file_info.lower_ext == "rgba32" || file_info.lower_ext == "bgra32")
        {
            channels = 4;
        }

        int height = file_info.height;
        int width = file_info.width;
        FILE* fin = fopen(file_info.filepath.c_str(), "rb");
        int buf_size = height * width * channels;
        cv::Size size;
        size.height = height;
        size.width = width;
        image = cv::Mat(size, CV_8UC(channels));
        fread(image.data, buf_size, 1, fin);
        fclose(fin);

        if (file_info.lower_ext == "rgb24") // bgr => rgb, inplace
        {
            cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        }
        else if (file_info.lower_ext == "rgba32")
        {
            cv::cvtColor(image, image, cv::COLOR_RGBA2BGRA);
        }
    }
    else
    {
        LOG(ERROR) << fmt1::format("not supported format {:s}\n", file_info.lower_ext.c_str());
    }
    return image;
}

cv::Mat read_image(const ImageFileInfo& file_info)
{
    cv::Mat image;
    if (file_info.lower_ext == "bmp" || file_info.lower_ext == "jpg" || file_info.lower_ext == ".jpeg" || file_info.lower_ext == "png")
    {
        image = cv::imread(file_info.filepath, cv::IMREAD_UNCHANGED);
    }
    else
    {
        image = load_fourcc_and_convert_to_mat(file_info);
    }
    return image;
}

} // namespace

namespace kantu {

ImageFileInfo::ImageFileInfo(const std::string& _filename)
{
    do {
        FilePath path(_filename);
        ext = path.ext();
        if (ext.length() < 3)
        {
            valid = false;
            err_msg = "no valid ext found in filepath";
            break;
        }

        filepath = path.path();
        head = path.basename();
        lower_ext = to_lower(ext);

        // convert raw extension (lowercase) to identical extention
        bool do_opencv_identical_ext_mapping = true;
        if (do_opencv_identical_ext_mapping)
        {
            if (lower_ext == "yuv" || lower_ext == "iyuv")
            {
                lower_ext = "i420";
            }
            else if (lower_ext == "y422" || lower_ext == "uynv")
            {
                lower_ext = "uyvy";
            }
        }

        bool do_yuvviewer_identical_ext_mapping = true;
        if (do_yuvviewer_identical_ext_mapping)
        {
            if (lower_ext == "grey")
            {
                lower_ext = "gray";
            }
            else if (lower_ext == "yv24")
            {
                lower_ext = "i444";
            }
        }

        /// validate filename's ext
        bool found = false;
        std::vector<std::string> valid_ext = get_supported_image_file_exts();
        for (int i = 0; i < valid_ext.size(); i++)
        {
            if (valid_ext[i] == lower_ext)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            this->err_msg = "invalid filename extension! Currently only supports these: (case insensitive) ";
            for (int i = 0; i < valid_ext.size(); i++)
            {
                this->err_msg = this->err_msg + " " + valid_ext[i];
            }
            break;
        }

        // validate filename's head
        if (lower_ext == "jpg" || lower_ext == "jpeg" || lower_ext == "png" || lower_ext == "bmp")
        {
            valid = true;
            break;
        }

        // (ext=="nv21" || ext=="nv12" || ext=="bgr24" || ext=="rgb24")
        // valid format for head:
        // [prefix]_[width]x[height]
        // len(prefix)>0
        // len(width)>0
        // len(height)>0
        // find and validate `_`
        int underline_pos = head.find_last_of('_');
        LOG(INFO) << fmt1::format("underline_pos = {:d}\n", underline_pos);
        if (underline_pos != -1 && underline_pos > static_cast<int>(head.length()) - 4)
        {
            this->err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, `_` position invalid now";
            break;
        }

        std::string dim_str = head.substr(underline_pos + 1);
        int non_digit_cnt = 0;
        int non_digit_pos = -1;
        for (int i = 0; i < dim_str.length(); i++)
        {
            if (!isdigit(dim_str[i]))
            {
                non_digit_cnt++;
                non_digit_pos = i;
            }
        }
        LOG(INFO) << (fmt1::format("non_digit_cnt is {:d}\n", non_digit_cnt));
        if (non_digit_cnt != 1)
        {
            this->err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, dim str wrong now";
            break;
        }

        int height = 0, width = 0;
        for (int i = 0; i < non_digit_pos; i++)
        {
            width = width * 10 + (dim_str[i] - '0');
        }
        for (int i = non_digit_pos + 1; i < dim_str.length(); i++)
        {
            height = height * 10 + (dim_str[i] - '0');
        }
        this->height = height;
        this->width = width;

        int actual_size = kantu::get_file_size(filepath);
        int expected_size = -1;
        
        // std::unordered_map<PixelFormat, PlaneInfo[4]> mp;
        
        // // nv21
        // mp[PixelFormat::PIX_FMT_NV21][0] = PlaneInfo(height, width, 1);
        // mp[PixelFormat::PIX_FMT_NV21][1] = PlaneInfo(height / 2, width / 2, 2);

        // // nv12
        // mp[PixelFormat::PIX_FMT_NV12][0] = PlaneInfo(height, width, 1);
        // mp[PixelFormat::PIX_FMT_NV12][1] = PlaneInfo(height / 2, width / 2, 2);

        // // i420


        if (lower_ext == "nv21" || lower_ext == "nv12" || lower_ext == "i420" || lower_ext == "yv12")
        {
            expected_size = height * width * 3 / 2;
        }
        else if (lower_ext == "rgb24" || lower_ext == "bgr24")
        {
            expected_size = height * width * 3;
        }
        else if (lower_ext == "rgba32" || lower_ext == "bgra32")
        {
            expected_size = height * width * 4;
        }
        else if (lower_ext == "gray")
        {
            expected_size = height * width;
        }
        else if (lower_ext == "uyvy" || lower_ext == "yuyv" || lower_ext == "yvyu")
        {
            expected_size = height * width * 2;
        }
        else if (lower_ext == "i444")
        {
            expected_size = height * width * 3;
        }
        LOG(INFO) << fmt1::format("ext is {:s}\n", lower_ext.c_str());

        if (expected_size != actual_size)
        {
            this->err_msg = "invalid file size, filename described different that actual";
            LOG(ERROR) << fmt1::format("expected_size: {:d}, actual_size: {:d}\n", expected_size, actual_size);
            break;
        }

        if (lower_ext == "nv21" || lower_ext == "nv12" || lower_ext == "i420" || lower_ext == "uyvy" || lower_ext == "yuyv" || lower_ext == "yv12" || lower_ext == "yvyu")
        {
            if (height % 2 != 0 || width % 2 != 0)
            {
                this->err_msg = "file dimension invalid. both height and width should be even number";
                break;
            }
        }
        valid = true;
    } while (0);
}

} // namespace kantu

int kantu::get_file_size(const Str256& filepath)
{
    std::filesystem::path p{filepath.c_str()};
    return std::filesystem::file_size(p);
}

cv::Mat kantu::load_image(const std::string& image_path)
{
    /// check if file exist or not
    if (!kantu::file_exist(image_path))
    {
        LOG(ERROR) << fmt1::format("file {:s} does not exist\n", image_path.c_str());
        cv::Mat image(100, 100, CV_8UC3, cv::Scalar(0, 0, 0));
        return image;
    }

    ImageFileInfo file_info(image_path);
    bool valid = file_info.valid;
    if (!valid)
    {
        LOG(ERROR) << fmt1::format("{:s}\n", file_info.err_msg.c_str());
        return cv::Mat(100, 100, CV_8UC3);
    }
    else
    {
        LOG(INFO) << fmt1::format("reading file {:s}\n", image_path.c_str());
        cv::Mat image = read_image(file_info);
        return image;
    }
}

/// @brief check if file exist
/// @retval true file exist
/// @retval false file not exist
bool kantu::file_exist(const char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return false;
    }
    else
    {
        fclose(fp);
        return true;
    }
}

bool kantu::file_exist(const std::string& filename)
{
    return file_exist(filename.c_str());
}

std::vector<std::string> kantu::get_supported_image_file_exts()
{
    static std::vector<std::string> exts = {
        "jpg",
        "jpeg",
        "png",
        "bmp",

        "rgb24",
        "bgr24",
        "rgba32",
        "bgra32",
        "gray", "grey",

        "nv21", // yuv420sp
        "nv12",
        "i420", 
            "iyuv", // opencv
            "yuv",  // yuvviewer

        "uyvy",
            "y422", // opencv
            "uynv", // opencv

        "yuyv",
            "yunv", // opencv
            "yuy2", // opencv

        "yv12",  // yuv420p
        "yvyu",

        // not supported yet
        "i444",
            "yv24",

        "uyvy2",
        "vyuy",
        "vyuy2",
        "yuyv2",
        "i422h",
        "yv16h",
        "i422v",
        "yv16v",
        "lpi422h",
        "yvu",
        "uvy",
        "vuy",
    };

    // YUVviewer supported:
    // I444, YV24, NV12, NV21, I420, YV12, UYVY, UYVY2, VYUY, VYUY2, YUYV, YUYV2, YVYU, YVYU2, I422H, YV16H, I422V, YV16V, LPI422H, YUV, YVU, UVY, VUY, Gray, Grey

    return exts;
}

