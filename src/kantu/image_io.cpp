#include "kantu/image_io.hpp"
#include "kantu/image.hpp"
#include "kantu/transform_format.hpp"
#include "kantu/string.hpp"
#include <filesystem>
#include <opencv2/imgproc.hpp>
#include <unordered_map>
#include <vector>
#include "mlcc/fmt1.h"
#include "log.hpp"
#include <fstream>

namespace {
using namespace kantu;
using namespace Shadow;

PixelFormat get_pixel_format_from_file_ext(const std::string& ext)
{
    std::unordered_map<std::string, PixelFormat> mp = {
        { "gray", PixelFormat::GRAY8 },

        { "rgb24", PixelFormat::RGB24 },
        { "bgr24", PixelFormat::BGR24 },

        { "nv21", PixelFormat::NV21 },
        { "nv12", PixelFormat::NV12 },

        { "i420", PixelFormat::I420 },
        { "yv12", PixelFormat::YV12 },
        { "uyvy", PixelFormat::UYVY },
        { "yuyv", PixelFormat::YUYV },
        { "yvyu", PixelFormat::YVYU },
        
        { "i444", PixelFormat::I444 },
        { "bgra32", PixelFormat::BGRA32 },
        { "rgba32", PixelFormat::RGBA32 },
    };

    if (mp.count(ext))
    {
        return mp[ext];
    }
    LOG(ERROR) << fmt1::format("Un-supported image file extension {:s} for mapping\n", ext.c_str());
    return PixelFormat::NONE;
}

cv::Mat load_fourcc_and_convert_to_mat(const ImageFileInfo& file_info)
{
    Image tu;

    std::ifstream fs(file_info.filepath, std::ios::in | std::ios::binary);
    cv::Mat total(1, file_info.filesize, CV_8UC1);
    fs.read(reinterpret_cast<char*>(total.data), file_info.filesize);

    int height = file_info.height;
    int width = file_info.width;

    cv::Mat image;
    PixelFormat fmt = get_pixel_format_from_file_ext(file_info.mapped_ext);

    // 3/2
    if (fmt == PixelFormat::NV21)
    {
        cv::Mat nv21 = total.reshape(1, height * 3 / 2);
        cv::cvtColor(nv21, image, cv::COLOR_YUV2BGR_NV21);
    }
    else if (fmt == PixelFormat::NV12)
    {
        cv::Mat nv12 = total.reshape(1, height * 3 / 2);
        cv::cvtColor(nv12, image, cv::COLOR_YUV2BGR_NV12);
    }
    else if (fmt == PixelFormat::I420)
    {
        cv::Mat i420 = total.reshape(1, height * 3 / 2);
        cv::cvtColor(i420, image, cv::COLOR_YUV2BGR_I420);
    }
    else if (fmt == PixelFormat::YV12)
    {
        cv::Mat yv12 = total.reshape(1, height * 3 / 2);
        cv::cvtColor(yv12, image, cv::COLOR_YUV2BGR_YV12);
    }

    // 2
    else if (fmt == PixelFormat::UYVY)
    {
        cv::Mat uyvy = total.reshape(2, height);
        cv::cvtColor(uyvy, image, cv::COLOR_YUV2BGR_UYVY);
    }
    else if (fmt == PixelFormat::YUYV)
    {
        cv::Mat yuyv = total.reshape(2, height);
        cv::cvtColor(yuyv, image, cv::COLOR_YUV2BGR_YUYV);
    }
    else if (fmt == PixelFormat::YVYU)
    {
        cv::Mat yvyu = total.reshape(2, height);
        cv::cvtColor(yvyu, image, cv::COLOR_YUV2BGR_YVYU);
    }

    // 3
    else if (fmt == PixelFormat::I444)
    {
        cv::Mat i444 = total.reshape(3, height);
#if 1
        image.create(i444.size(), i444.type());
        i444_to_rgb(i444.data, image.data, width, height);
        cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
#else
        cv::cvtColor(yuv444_mat, image, cv::COLOR_YUV2BGR);
        // cv::Mat tmp;
        // chw_to_hwc(yuv444_mat, tmp);
#endif
    }

    else if (fmt == PixelFormat::BGR24)
    {
        image = total.reshape(3, height);
    }
    else if (fmt == PixelFormat::RGB24)
    {
        image = total.reshape(3, height);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
    }
    else if (fmt == PixelFormat::BGRA32)
    {
        image = total.reshape(4, height);
    }
    else if (fmt == PixelFormat::RGBA32)
    {
        image = total.reshape(4, height);
        cv::cvtColor(image, image, cv::COLOR_RGBA2BGRA);
    }
    else if (fmt == PixelFormat::GRAY8)
    {
        image = total.reshape(1, height);
    }
    else
    {
        LOG(ERROR) << fmt1::format("not supported format {:s}\n", file_info.mapped_ext.c_str());
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
        mapped_ext = to_lower(ext);

        // convert raw extension (lowercase) to identical extention
        bool do_opencv_identical_ext_mapping = true;
        if (do_opencv_identical_ext_mapping)
        {
            if (mapped_ext == "yuv" || mapped_ext == "iyuv")
            {
                mapped_ext = "i420";
            }
            else if (mapped_ext == "y422" || mapped_ext == "uynv")
            {
                mapped_ext = "uyvy";
            }
        }

        bool do_yuvviewer_identical_ext_mapping = true;
        if (do_yuvviewer_identical_ext_mapping)
        {
            if (mapped_ext == "grey")
            {
                mapped_ext = "gray";
            }
            else if (mapped_ext == "yv24")
            {
                mapped_ext = "i444";
            }
        }

        /// validate filename's ext
        bool found = false;
        std::vector<std::string> valid_ext = get_supported_image_file_exts();
        for (size_t i = 0; i < valid_ext.size(); i++)
        {
            if (valid_ext[i] == mapped_ext)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            err_msg = "invalid filename extension! Currently only supports these: (case insensitive) ";
            for (size_t i = 0; i < valid_ext.size(); i++)
            {
                err_msg = err_msg + " " + valid_ext[i];
            }
            break;
        }

        // validate filename's head
        if (mapped_ext == "jpg" || mapped_ext == "jpeg" || mapped_ext == "png" || mapped_ext == "bmp")
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
            err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, `_` position invalid now";
            break;
        }

        std::string dim_str = head.substr(underline_pos + 1);
        int non_digit_cnt = 0;
        int non_digit_pos = -1;
        for (size_t i = 0; i < dim_str.length(); i++)
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
            err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, dim str wrong now";
            break;
        }

        height = 0, width = 0;
        for (int i = 0; i < non_digit_pos; i++)
        {
            width = width * 10 + (dim_str[i] - '0');
        }
        for (size_t i = non_digit_pos + 1; i < dim_str.length(); i++)
        {
            height = height * 10 + (dim_str[i] - '0');
        }

        int actual_size = kantu::get_file_size(filepath);
        int expected_size = -1;
        filesize = actual_size;
        
        // std::unordered_map<PixelFormat, PlaneInfo[4]> mp;
        
        // // nv21
        // mp[PixelFormat::PIX_FMT_NV21][0] = PlaneInfo(height, width, 1);
        // mp[PixelFormat::PIX_FMT_NV21][1] = PlaneInfo(height / 2, width / 2, 2);

        // // nv12
        // mp[PixelFormat::PIX_FMT_NV12][0] = PlaneInfo(height, width, 1);
        // mp[PixelFormat::PIX_FMT_NV12][1] = PlaneInfo(height / 2, width / 2, 2);

        // // i420


        if (mapped_ext == "nv21" || mapped_ext == "nv12" || mapped_ext == "i420" || mapped_ext == "yv12")
        {
            expected_size = height * width * 3 / 2;
        }
        else if (mapped_ext == "rgb24" || mapped_ext == "bgr24")
        {
            expected_size = height * width * 3;
        }
        else if (mapped_ext == "rgba32" || mapped_ext == "bgra32")
        {
            expected_size = height * width * 4;
        }
        else if (mapped_ext == "gray")
        {
            expected_size = height * width;
        }
        else if (mapped_ext == "uyvy" || mapped_ext == "yuyv" || mapped_ext == "yvyu")
        {
            expected_size = height * width * 2;
        }
        else if (mapped_ext == "i444")
        {
            expected_size = height * width * 3;
        }
        LOG(INFO) << fmt1::format("lower_ext is {:s}\n", mapped_ext.c_str());

        if (expected_size != actual_size)
        {
            err_msg = "invalid file size, filename described different that actual";
            LOG(ERROR) << fmt1::format("expected_size: {:d}, actual_size: {:d}\n", expected_size, actual_size);
            break;
        }

        if (mapped_ext == "nv21" || mapped_ext == "nv12" || mapped_ext == "i420" || mapped_ext == "uyvy" || mapped_ext == "yuyv" || mapped_ext == "yv12" || mapped_ext == "yvyu")
        {
            if (height % 2 != 0 || width % 2 != 0)
            {
                err_msg = "file dimension invalid. both height and width should be even number";
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
        return cv::Mat::zeros(100, 100, CV_8UC3);
    }

    ImageFileInfo file_info(image_path);
    bool valid = file_info.valid;
    if (!valid)
    {
        LOG(ERROR) << fmt1::format("{:s}\n", file_info.err_msg.c_str());
        return cv::Mat::zeros(100, 100, CV_8UC3);
    }
    else
    {
        LOG(INFO) << fmt1::format("reading file {:s}\n", image_path.c_str());
        cv::Mat image;
        for (const std::string& encoded_ext : { "bmp", "jpg", "jpeg", "png" })
        {
            if (file_info.mapped_ext == encoded_ext)
            {
                return cv::imread(file_info.filepath, cv::IMREAD_UNCHANGED);
            }
        }

        return load_fourcc_and_convert_to_mat(file_info);
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
