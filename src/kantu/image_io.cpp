#include "kantu/image_io.hpp"
#include "kantu/image.hpp"
#include "kantu/transform_format.hpp"
#include "kantu/string.hpp"
#include <filesystem>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <unordered_map>
#include <vector>
#include "mlcc/fmt1.h"
#include "log.hpp"
#include <fstream>

using namespace Shadow;

/// @return BGR, BGRA or GRAY
cv::Mat kantu::convert_fourcc_to_mat(const FourccImage& fourcc)
{
    cv::Mat total = fourcc.view1d;

    int height = fourcc.height;
    int width = fourcc.width;

    cv::Mat mat;
    PixelFormat fmt = fourcc.format;

    switch (fmt)
    {
    case PixelFormat::NV21:
        cv::cvtColor(fourcc.view2d, mat, cv::COLOR_YUV2BGR_NV21);
        break;

    case PixelFormat::NV12:
        cv::cvtColor(fourcc.view2d, mat, cv::COLOR_YUV2BGR_NV12);
        break;

    case PixelFormat::I420:
        cv::cvtColor(fourcc.view2d, mat, cv::COLOR_YUV2BGR_I420);
        break;

    case PixelFormat::YV12:
        cv::cvtColor(fourcc.view2d, mat, cv::COLOR_YUV2BGR_YV12);
        break;

    case PixelFormat::UYVY:
        cv::cvtColor(fourcc.view2d, mat, cv::COLOR_YUV2BGR_UYVY);
        break;

    case PixelFormat::YUYV:
        cv::cvtColor(fourcc.view2d, mat, cv::COLOR_YUV2BGR_YUYV);
        break;

    case PixelFormat::YVYU:
        cv::cvtColor(fourcc.view2d, mat, cv::COLOR_YUV2BGR_YVYU);
        break;

    case PixelFormat::I444:
    {
        cv::Mat i444 = fourcc.view2d;
#if 1
        mat.create(i444.size(), i444.type());
        i444_to_rgb(i444.data, mat.data, width, height);
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
#else
        cv::cvtColor(yuv444_mat, image, cv::COLOR_YUV2BGR);
        // cv::Mat tmp;
        // chw_to_hwc(yuv444_mat, tmp);
#endif
        break;
    }

    case PixelFormat::BGR24:
        mat = fourcc.view2d;
        break;

    case PixelFormat::RGB24:
        cv::cvtColor(fourcc.view2d, mat, cv::COLOR_RGB2BGR);
        break;

    case PixelFormat::BGRA32:
        mat = fourcc.view2d;
        break;

    case PixelFormat::RGBA32:
        cv::cvtColor(fourcc.view2d, mat, cv::COLOR_RGBA2BGRA);
        break;

    case PixelFormat::GRAY8:
        mat = fourcc.view2d;
        break;
    
    default:
        LOG(ERROR) << fmt1::format("not supported format {:d}\n", fmt);
    }

    return mat;
}

namespace {
using namespace kantu;
int get_expected_fourcc_file_size(std::string mapped_ext, int height, int width)
{
    int expected_size = -1;
    PixelFormat fmt = get_pixel_format_from_file_ext(mapped_ext);
    switch (fmt)
    {
    case PixelFormat::NV21:
    case PixelFormat::NV12:
    case PixelFormat::I420:
    case PixelFormat::YV12:
        expected_size = height * width * 3 / 2;
        break;
    
    case PixelFormat::RGB24:
    case PixelFormat::BGR24:
        expected_size = height * width * 3;
        break;

    case PixelFormat::RGBA32:
    case PixelFormat::BGRA32:
        expected_size = height * width * 4;
        break;

    case PixelFormat::GRAY8:
        expected_size = height * width;
        break;

    case PixelFormat::UYVY:
    case PixelFormat::YUYV:
    case PixelFormat::YVYU:
        expected_size = height * width * 2;
        break;

    case PixelFormat::I444:
        expected_size = height * width * 3;
        break;
    
    default:
        LOG(INFO) << fmt1::format("un handled lower_ext: {:s}\n", mapped_ext.c_str());
    }

    return expected_size;
}

} // namespace

namespace kantu {

FourccFileInfo::FourccFileInfo(const FilePath& path)
{
    ext = path.ext();
    if (ext.length() < 3)
    {
        valid = false;
        err_msg = "no valid ext found in filepath";
        return;
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
    std::vector<std::string> valid_ext = get_supported_image_file_exts();
    int pos = kantu::find(valid_ext, mapped_ext);
    if (pos == -1)
    {
        err_msg = "invalid filename extension! Currently only supports these: (case insensitive) ";
        for (size_t i = 0; i < valid_ext.size(); i++)
        {
            err_msg = err_msg + " " + valid_ext[i];
        }
        return;
    }

    // (ext=="nv21" || ext=="nv12" || ext=="bgr24" || ext=="rgb24")
    // valid format for head:
    // [prefix]_[width]x[height]
    // len(prefix)>0
    // len(width)>0
    // len(height)>0
    // find and validate `_`
    int underline_pos = head.find_last_of('_');
    // LOG(INFO) << fmt1::format("underline_pos = {:d}\n", underline_pos);
    if (underline_pos != -1 && underline_pos > static_cast<int>(head.length()) - 4)
    {
        err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, `_` position invalid now";
        return;
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
    // LOG(INFO) << (fmt1::format("non_digit_cnt is {:d}\n", non_digit_cnt));
    if ((non_digit_cnt != 1) || (dim_str[non_digit_pos] != 'x'))
    {
        err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, dim str wrong now";
        return;
    }

    std::string width_str = dim_str.substr(0, non_digit_pos);
    std::string height_str = dim_str.substr(non_digit_pos + 1);
    width = std::stoi(width_str);
    height = std::stoi(height_str);

    int actual_size = kantu::get_file_size(filepath);
    int expected_size = get_expected_fourcc_file_size(mapped_ext, height, width);
    filesize = actual_size;

    if (expected_size != actual_size)
    {
        err_msg = "invalid file size, filename described different that actual";
        LOG(ERROR) << fmt1::format("expected_size: {:d}, actual_size: {:d}\n", expected_size, actual_size);
        return;
    }

    if (mapped_ext == "nv21" || mapped_ext == "nv12" || mapped_ext == "i420" || mapped_ext == "uyvy" || mapped_ext == "yuyv" || mapped_ext == "yv12" || mapped_ext == "yvyu")
    {
        if (height % 2 != 0 || width % 2 != 0)
        {
            err_msg = "file dimension invalid. both height and width should be even number";
            return;
        }
    }
    valid = true;
}

} // namespace kantu

int kantu::get_file_size(const Str256& filepath)
{
    std::filesystem::path p{filepath.c_str()};
    return std::filesystem::file_size(p);
}

static cv::Mat convert_mat_to_bgra(const cv::Mat& src)
{
    cv::Mat dst = src;
    switch (src.channels())
    {
    case 1:
        cv::cvtColor(src, dst, cv::COLOR_GRAY2BGRA);
        break;
    case 3:
        cv::cvtColor(src, dst, cv::COLOR_BGR2BGRA);
        break;
    case 4:
        dst = src;
    default:
        LOG(ERROR) << "only 1, 3, 4 channel image supported\n";
    }
    return dst;
}

/// displayable means `bgra`
cv::Mat kantu::load_as_displayable_image(const std::string& image_path)
{
    cv::Mat empty_image;

    if (!kantu::file_exist(image_path))
    {
        LOG(ERROR) << fmt1::format("file {:s} does not exist\n", image_path.c_str());
        return empty_image;
    }

    FilePath path(image_path);
    std::string lower_ext = to_lower(path.ext());
    int pos = kantu::find({ "jpg", "jpeg", "png", "bmp" }, lower_ext);
    if (pos != -1)
    {
        cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);
        return convert_mat_to_bgra(image);
    }

    FourccFileInfo file_info(path);
    bool valid = file_info.valid;
    if (!valid)
    {
        LOG(ERROR) << fmt1::format("{:s}\n", file_info.err_msg.c_str());
        return empty_image;
    }
    else
    {
        LOG(INFO) << fmt1::format("reading file {:s}\n", image_path.c_str());
        FourccImage fourcc = load_fourcc(file_info);
        cv::Mat image = convert_fourcc_to_mat(fourcc);
        return convert_mat_to_bgra(image);
    }
}

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



FourccImage kantu::load_fourcc(const FourccFileInfo& file_info)
{
    FourccImage fourcc;

    std::ifstream fs(file_info.filepath, std::ios::in | std::ios::binary);
    cv::Mat view1d(1, file_info.filesize, CV_8UC1);
    fs.read(reinterpret_cast<char*>(view1d.data), file_info.filesize);

    int height = file_info.height;
    int width = file_info.width;
    fourcc.view1d = view1d;
    fourcc.height = height;
    fourcc.width = width;

    PixelFormat fmt = get_pixel_format_from_file_ext(file_info.mapped_ext);
    fourcc.format = fmt;
    switch (fmt)
    {
    case PixelFormat::NV21:
    case PixelFormat::NV12:
    {
        fourcc.view2d = view1d.reshape(1, height * 3 / 2);

        cv::Range y_range(0, height * width);
        cv::Range uv_range(y_range.end, view1d.total());
        cv::Mat Y = view1d.colRange(y_range).reshape(1, height);
        cv::Mat UV = view1d.colRange(uv_range).reshape(2, height / 2);
        fourcc.planes.emplace_back(Y);
        fourcc.planes.emplace_back(UV);
        break;
    }

    case PixelFormat::I420:
    {
        fourcc.view2d = view1d.reshape(1, height * 3 / 2);

        cv::Range y_range(0, height * width);
        cv::Range u_range(y_range.end, y_range.end + (height / 2) * (width / 2));
        cv::Range v_range(u_range.end, view1d.total());
        cv::Mat Y = view1d.colRange(y_range).reshape(1, height);
        cv::Mat U = view1d.colRange(u_range).reshape(2, height / 2);
        cv::Mat V = view1d.colRange(v_range).reshape(2, height / 2);
        fourcc.planes.emplace_back(Y);
        fourcc.planes.emplace_back(U);
        fourcc.planes.emplace_back(V);
        break;
    }
    case PixelFormat::YV12:
    {
        fourcc.view2d = view1d.reshape(1, height * 3 / 2);

        cv::Range y_range(0, height * width);
        cv::Range v_range(y_range.end, y_range.end + (height / 2) * (width / 2));
        cv::Range u_range(v_range.end, view1d.total());
        cv::Mat Y = view1d.colRange(y_range).reshape(1, height);
        cv::Mat V = view1d.colRange(v_range).reshape(2, height / 2);
        cv::Mat U = view1d.colRange(u_range).reshape(2, height / 2);
        fourcc.planes.emplace_back(Y);
        fourcc.planes.emplace_back(V);
        fourcc.planes.emplace_back(U);
        break;
    }
    
    case PixelFormat::UYVY:
    case PixelFormat::YUYV:
    case PixelFormat::YVYU:
    {
        fourcc.view2d = view1d.reshape(2, height);
        fourcc.planes.emplace_back(fourcc.view2d);
        break;
    }

    case PixelFormat::I444:
    {
        fourcc.view2d = view1d.reshape(3, height);
        fourcc.planes.emplace_back(fourcc.view2d);
        break;
    }
    
    case PixelFormat::BGR24:
    case PixelFormat::RGB24:
    {
        fourcc.view2d = view1d.reshape(3, height);
        fourcc.planes.emplace_back(fourcc.view2d);
        break;
    }

    case PixelFormat::BGRA32:
    case PixelFormat::RGBA32:
    {
        fourcc.view2d = view1d.reshape(4, height);
        fourcc.planes.emplace_back(fourcc.view2d);
        break;
    }

    case PixelFormat::GRAY8:
    {
        fourcc.view2d = view1d.reshape(1, height);
        fourcc.planes.emplace_back(fourcc.view2d);
        break;
    }

    default:
        LOG(ERROR) << "not supported or not handled format\n";

    }

    return fourcc;
}

PixelFormat kantu::get_pixel_format_from_file_ext(const std::string& ext)
{
    std::unordered_map<std::string, PixelFormat> mp = {
        { "nv21", PixelFormat::NV21 },
        { "nv12", PixelFormat::NV12 },
        { "i420", PixelFormat::I420 },
        { "yv12", PixelFormat::YV12 },

        { "uyvy", PixelFormat::UYVY },
        { "yuyv", PixelFormat::YUYV },
        { "yvyu", PixelFormat::YVYU },

        { "i444", PixelFormat::I444 },

        { "rgb24", PixelFormat::RGB24 },
        { "bgr24", PixelFormat::BGR24 },

        { "bgra32", PixelFormat::BGRA32 },
        { "rgba32", PixelFormat::RGBA32 },

        { "gray", PixelFormat::GRAY8 },
    };

    if (mp.count(ext))
    {
        return mp[ext];
    }
    LOG(ERROR) << fmt1::format("Un-supported image file extension {:s} for mapping\n", ext.c_str());
    return PixelFormat::NONE;
}