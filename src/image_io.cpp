#include "image_io.hpp"
#include <filesystem>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace {
using namespace imcmp;

class FileInfo
{
public:
    FileInfo()
    {
        height = -1;
        width = -1;
        head = "";
        ext = "";
        valid = true;
        err_msg = "";
    }
    std::string filename;
    std::string head;
    std::string ext;
    int height;
    int width;
    bool valid;
    std::string err_msg;
};

cv::Mat load_fourcc_and_convert_to_mat(const FileInfo& file_info)
{
    cv::Mat image;
    if (file_info.ext == "nv21" || file_info.ext == "nv12" || file_info.ext == "i420" || file_info.ext == "uyvy" || file_info.ext == "yuyv")
    {
        int height = file_info.height;
        int width = file_info.width;
        FILE* fin = fopen(file_info.filename.c_str(), "rb");
        
        cv::Size size;
        size.height = height;
        size.width = width;
        image = cv::Mat(size, CV_8UC3);

        if (file_info.ext == "nv21") // nv21 => bgr
        {
            int buf_size = height * width * 3 / 2;
            cv::Mat yuv420sp_mat(height * 3 / 2, width, CV_8UC1);
            uchar* yuv_buf = yuv420sp_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv420sp_mat, image, cv::COLOR_YUV2BGR_NV21);
        }
        else if (file_info.ext == "nv12") // nv12 => bgr
        {
            int buf_size = height * width * 3 / 2;
            cv::Mat yuv420sp_mat(height * 3 / 2, width, CV_8UC1);
            uchar* yuv_buf = yuv420sp_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv420sp_mat, image, cv::COLOR_YUV2BGR_NV12);
        }
        else if (file_info.ext == "i420")
        {
            int buf_size = height * width * 3 / 2;
            cv::Mat yuv420sp_mat(height * 3 / 2, width, CV_8UC1);
            uchar* yuv_buf = yuv420sp_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv420sp_mat, image, cv::COLOR_YUV2BGR_I420);
        }
        else if (file_info.ext == "uyvy")
        {
            int buf_size = height * width * 2;
            cv::Mat yuv422_mat(height, width, CV_8UC2);
            uchar* yuv_buf = yuv422_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv422_mat, image, cv::COLOR_YUV2BGR_UYVY);
        }
        else if (file_info.ext == "yuyv")
        {
            int buf_size = height * width * 2;
            cv::Mat yuv422_mat(height, width, CV_8UC2);
            uchar* yuv_buf = yuv422_mat.data;
            fread(yuv_buf, buf_size, 1, fin);
            fclose(fin);
            cv::cvtColor(yuv422_mat, image, cv::COLOR_YUV2BGR_YUYV);
        }
    }
    else if (file_info.ext == "bgr24" || file_info.ext == "rgb24" || file_info.ext == "rgba32" || file_info.ext == "bgra32" || file_info.ext == "gray")
    {
        int channels = 1; // "gray"
        if (file_info.ext == "bgr24" || file_info.ext == "rgb24")
        {
            channels = 3;
        }
        else if (file_info.ext == "rgba32" || file_info.ext == "bgra32")
        {
            channels = 4;
        }

        int height = file_info.height;
        int width = file_info.width;
        FILE* fin = fopen(file_info.filename.c_str(), "rb");
        int buf_size = height * width * channels;
        cv::Size size;
        size.height = height;
        size.width = width;
        image = cv::Mat(size, CV_8UC(channels));
        fread(image.data, buf_size, 1, fin);
        fclose(fin);

        if (file_info.ext == "rgb24") // bgr => rgb, inplace
        {
            cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        }
        else if (file_info.ext == "rgba32")
        {
            cv::cvtColor(image, image, cv::COLOR_RGBA2BGRA);
        }
    }
    else
    {
        fprintf(stderr, "not supported format %s\n", file_info.ext.c_str());
    }
    return image;
}

cv::Mat read_image(const FileInfo& file_info)
{
    cv::Mat image;
    if (file_info.ext == "bmp" || file_info.ext == "jpg" || file_info.ext == ".jpeg" || file_info.ext == "png")
    {
        image = cv::imread(file_info.filename, cv::IMREAD_UNCHANGED);
    }
    else
    {
        image = load_fourcc_and_convert_to_mat(file_info);
    }
    return image;
}

FileInfo get_meta_info(const std::string& filename)
{
    FileInfo file_info;
    file_info.filename = filename;

    do {
        /// validate filename format
        int ext_pos = filename.find_last_of(".");
        // valid format:
        // [head].[ext]
        // len(head) > 0
        // len(ext) >= 3
        if (!(ext_pos > 0 && ext_pos <= filename.length() - 4))
        {
            file_info.valid = false;
            file_info.err_msg = "invalid image path format " + filename + " , required format is [head].[ext] , where len(head)>0 and len(ext)>=3";
            break;
        }

        /// split filename's head and ext
        std::string ext = filename.substr(ext_pos + 1);
        int last_slash_pos = filename.find_last_of('/');
        std::string head = filename.substr(last_slash_pos + 1, ext_pos - last_slash_pos - 1);
        //std::cout << head << "." << ext << std::endl;
        for (int i = 0; i < ext.size(); i++)
        {
            if (isalpha(ext[i]))
            {
                ext[i] = tolower(ext[i]);
            }
        }
        file_info.head = head;
        file_info.ext = ext;

        /// validate filename's ext
        bool found = false;
        std::vector<std::string> valid_ext = get_supported_image_file_exts();
        for (int i = 0; i < valid_ext.size(); i++)
        {
            if (valid_ext[i] == ext)
            {
                found = true;
            }
        }
        if (!found)
        {
            file_info.valid = false;
            file_info.err_msg = "invalid filename extension! Currently only supports these: (case insensitive) ";
            for (int i = 0; i < valid_ext.size(); i++)
            {
                file_info.err_msg = file_info.err_msg + " " + valid_ext[i];
            }
            break;
        }

        // validate filename's head
        if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp")
        {
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
        fprintf(stderr, "underline_pos = %d\n", underline_pos);
        if (underline_pos != -1 && underline_pos > static_cast<int>(head.length()) - 4)
        {
            file_info.valid = false;
            file_info.err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, `_` position invalid now";
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
        fprintf(stderr, "non_digit_cnt is %d\n", non_digit_cnt);
        if (non_digit_cnt != 1)
        {
            file_info.valid = false;
            file_info.err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, dim str wrong now";
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
        file_info.height = height;
        file_info.width = width;

        int actual_size = imcmp::get_file_size(filename);
        int expected_size = -1;
        if (ext == "nv21" || ext == "nv12" || ext == "i420")
        {
            expected_size = height * width * 3 / 2;
        }
        else if (ext == "rgb24" || ext == "bgr24")
        {
            expected_size = height * width * 3;
        }
        else if (ext == "rgba32" || ext == "bgra32")
        {
            expected_size = height * width * 4;
        }
        else if (ext == "gray")
        {
            expected_size = height * width;
        }
        else if (ext == "uyvy" || ext == "yuyv")
        {
            expected_size = height * width * 2;
        }

        if (expected_size != actual_size)
        {
            file_info.valid = false;
            file_info.err_msg = "invalid file size, filename described different that actual";
            break;
        }

        if (ext == "nv21" || ext == "nv12" || ext == "i420" || ext == "uyvy" || ext == "yuyv")
        {
            if (height % 2 != 0 || width % 2 != 0)
            {
                file_info.valid = false;
                file_info.err_msg = "file dimension invalid. both height and width should be even number";
                break;
            }
        }
    } while (0);

    return file_info;
}

} // namespace

int imcmp::get_file_size(const Str256& filepath)
{
    std::filesystem::path p{filepath.c_str()};
    return std::filesystem::file_size(p);
}

cv::Mat imcmp::load_image(const std::string& image_path)
{
    /// check if file exist or not
    if (!imcmp::file_exist(image_path))
    {
        fprintf(stderr, "file %s does not exist\n", image_path.c_str());
        return cv::Mat(100, 100, CV_8UC3);
    }

    FileInfo file_info = get_meta_info(image_path);
    if (!file_info.valid)
    {
        fprintf(stderr, "%s\n", file_info.err_msg.c_str());
        return cv::Mat(100, 100, CV_8UC3);
    }
    else
    {
        printf("reading file %s\n", image_path.c_str());
        cv::Mat image = read_image(file_info);
        return image;
    }
}

/// @brief check if file exist
/// @retval true file exist
/// @retval false file not exist
bool imcmp::file_exist(const char* filename)
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

bool imcmp::file_exist(const std::string& filename)
{
    return file_exist(filename.c_str());
}

std::vector<std::string> imcmp::get_supported_image_file_exts()
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
        "gray",

        "nv21", // yuv420sp
        "nv12",
        "i420", // iyuv
        "uyvy", // y422, uynv

        // to be supported
        "yuyv",  // yunv, yuy2
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
        "yuv",
        "yvu",
        "uvy",
        "vuy",
        "grey"
    };

    // YUVviewer supported:
    // I444, YV24, NV12, NV21, I420, YV12, UYVY, UYVY2, VYUY, VYUY2, YUYV, YUYV2, YVYU, YVYU2, I422H, YV16H, I422V, YV16V, LPI422H, YUV, YVU, UVY, VUY, Gray, Grey

    return exts;
}