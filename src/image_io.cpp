#include "image_io.hpp"
#include <filesystem>
#include <vector>

namespace {
using namespace imcmp;

class FileMetaInfo
{
public:
    FileMetaInfo()
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

std::vector<std::string> get_valid_image_file_ext()
{
    static std::vector<std::string> valid_ext = {
        "jpg",
        "jpeg",
        "png",
        "bmp",
        "nv21",
        "nv12",
        "rgb24",
        "bgr24",

        "gray",
        "i420",
        "rgba32"};
    return valid_ext;
}

cv::Mat read_image(const FileMetaInfo& meta_info)
{
    cv::Mat image;
    if (meta_info.ext == "bmp" || meta_info.ext == "jpg" || meta_info.ext == ".jpeg" || meta_info.ext == "png")
    {
        image = cv::imread(meta_info.filename, cv::IMREAD_UNCHANGED);
    }
    else if (meta_info.ext == "nv21" || meta_info.ext == "nv12")
    {
        int height = meta_info.height;
        int width = meta_info.width;
        FILE* fin = fopen(meta_info.filename.c_str(), "rb");
        int buf_size = height * width * 3 / 2;

        cv::Mat yuv420sp_mat(height * 3 / 2, width, CV_8UC1);
        uchar* yuv_buf = yuv420sp_mat.data;
        fread(yuv_buf, buf_size, 1, fin);
        fclose(fin);

        cv::Size size;
        size.height = height;
        size.width = width;
        image = cv::Mat(size, CV_8UC3);

        if (meta_info.ext == "nv21") // nv21 => bgr
        {
            cv::cvtColor(yuv420sp_mat, image, cv::COLOR_YUV2BGR_NV21);
        }
        else if (meta_info.ext == "nv12") // nv12 => bgr
        {
            cv::cvtColor(yuv420sp_mat, image, cv::COLOR_YUV2BGR_NV12);
        }
    }
    else if (meta_info.ext == "bgr24" || meta_info.ext == "rgb24")
    {
        int height = meta_info.height;
        int width = meta_info.width;
        FILE* fin = fopen(meta_info.filename.c_str(), "rb");
        int buf_size = height * width * 3;
        cv::Size size;
        size.height = height;
        size.width = width;
        image = cv::Mat(size, CV_8UC3);
        fread(image.data, buf_size, 1, fin);
        fclose(fin);

        if (meta_info.ext == "rgb24") // bgr => rgb, inplace
        {
            cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        }
    }
    return image;
}

FileMetaInfo get_meta_info(const std::string& filename)
{
    FileMetaInfo meta_info;
    meta_info.filename = filename;

    do {
        /// validate filename format
        int ext_pos = filename.find_last_of(".");
        // valid format:
        // [head].[ext]
        // len(head) > 0
        // len(ext) >= 3
        if (!(ext_pos > 0 && ext_pos <= filename.length() - 4))
        {
            meta_info.valid = false;
            meta_info.err_msg = "invalid image path format " + filename + " , required format is [head].[ext] , where len(head)>0 and len(ext)>=3";
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
        meta_info.head = head;
        meta_info.ext = ext;

        /// validate filename's ext
        bool found = false;
        std::vector<std::string> valid_ext = get_valid_image_file_ext();
        for (int i = 0; i < valid_ext.size(); i++)
        {
            if (valid_ext[i] == ext)
            {
                found = true;
            }
        }
        if (!found)
        {
            meta_info.valid = false;
            meta_info.err_msg = "invalid filenames extension! Currently only supports these: (case insensitive) ";
            for (int i = 0; i < valid_ext.size(); i++)
            {
                meta_info.err_msg = meta_info.err_msg + " " + valid_ext[i];
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
            meta_info.valid = false;
            meta_info.err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, `_` position invalid now";
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
            meta_info.valid = false;
            meta_info.err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, dim str wrong now";
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
        meta_info.height = height;
        meta_info.width = width;

        int actual_size = imcmp::get_file_size(filename);
        int expected_size = -1;
        if (ext == "nv21" || ext == "nv12")
        {
            expected_size = height * width * 3 / 2;
        }
        else if (ext == "rgb24" || ext == "bgr24")
        {
            expected_size = height * width * 3;
        }

        if (expected_size != actual_size)
        {
            meta_info.valid = false;
            meta_info.err_msg = "invalid file size, filename described different that actual";
            break;
        }

        if (ext == "nv21" || ext == "nv12")
        {
            if (height % 2 != 0 || width % 2 != 0)
            {
                meta_info.valid = false;
                meta_info.err_msg = "file dimension invalid. both height and width should be even number";
                break;
            }
        }
    } while (0);

    return meta_info;
}

} // namespace

int imcmp::get_file_size(const Str256& filepath)
{
    std::filesystem::path p{filepath.c_str()};
    return std::filesystem::file_size(p);
}

cv::Mat imcmp::loadImage(const std::string& image_path)
{
    /// check if file exist or not
    if (!imcmp::file_exist(image_path))
    {
        fprintf(stderr, "file %s does not exist\n", image_path.c_str());
        return cv::Mat(100, 100, CV_8UC3);
    }

    FileMetaInfo meta_info = get_meta_info(image_path);
    if (!meta_info.valid)
    {
        fprintf(stderr, "%s\n", meta_info.err_msg.c_str());
        return cv::Mat(100, 100, CV_8UC3);
    }
    else
    {
        printf("reading file %s\n", image_path.c_str());
        cv::Mat image = read_image(meta_info);
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