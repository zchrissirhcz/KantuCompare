#include "image_compare_core.hpp"


#include <sys/stat.h>

/// @brief get file size. Works on Linux.
// https://blog.csdn.net/yutianzuijin/article/details/27205121
int get_file_size(const char* filename)
{
    struct stat statbuf;
    stat(filename,&statbuf);
    int size=statbuf.st_size;

    return size;
}

int get_file_size(const std::string& filename)
{
    return get_file_size(filename.c_str());
}

void imk::getDiffImage(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& diff, int thresh, cv::Scalar below, cv::Scalar above)
{
    CV_Assert(src1.rows == src2.rows && src1.cols == src2.cols);
    CV_Assert(src1.channels() == src2.channels());
    CV_Assert(thresh >= 0);

    const int channels = src1.channels();
    
    cv::absdiff(src1, src2, diff);
    for (int i = 0; i < diff.rows; i++)
    {
        for (int j = 0; j < diff.cols; j++)
        {
            uchar* diff_pixel = diff.ptr(i, j);
            const uchar* src_pixel = src1.ptr(i, j);
            bool bigger = false;
            int diff_sum = 0;
            for (int k = 0; k < channels; k++)
            {
                diff_sum += diff_pixel[k];
                if (diff_pixel[k] > thresh)
                {
                    bigger = true;
                    break;
                }
            }
            if (diff_sum == 0)
            {
                float R2Y = 0.299;
                float G2Y = 0.587;
                float B2Y = 0.114;
                
                int B = src_pixel[0];
                int G = src_pixel[1];
                int R = src_pixel[2];

                int Gray = R2Y * R + G2Y * G + B2Y * B;

                diff_pixel[0] = Gray;
                diff_pixel[1] = Gray;
                diff_pixel[2] = Gray;
                diff_pixel[3] = 255;
            }
            else
            {
                if (bigger)
                {
                    diff_pixel[0] = above.val[0];
                    diff_pixel[1] = above.val[1];
                    diff_pixel[2] = above.val[2];
                    diff_pixel[3] = 255;
                }
                else
                {
                    diff_pixel[0] = below.val[0];
                    diff_pixel[1] = below.val[1];
                    diff_pixel[2] = below.val[2];
                    diff_pixel[3] = 255;
                }
            }
        }
    }
}

std::vector<std::string> get_valid_ext()
{
    static std::vector<std::string> valid_ext = {
        "jpg",
        "png",
        "bmp",
        "nv21",
        "nv12",
        "rgb24",
        "bgr24"
    };
    return valid_ext;
}

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

FileMetaInfo get_meat_info(const std::string& filename)
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
        if ( ! ( ext_pos > 0 && ext_pos <= filename.length()-4 ) )
        {
            meta_info.valid = false;
            meta_info.err_msg = "invalid image path format " + filename + " , required format is [head].[ext] , where len(head)>0 and len(ext)>=3";
            break;
        }

        /// split filename's head and ext
        std::string ext = filename.substr(ext_pos+1);
        int last_slash_pos = filename.find_last_of('/');
        std::string head = filename.substr(last_slash_pos+1, ext_pos - last_slash_pos - 1);
        fprintf(stderr, ">> head is %s\n", head.c_str());
        //std::cout << head << "." << ext << std::endl;
        for (int i=0; i<ext.size(); i++)
        {
            if (isalpha(ext[i])) {
                ext[i] = tolower(ext[i]);
            }
        }
        meta_info.head = head;
        meta_info.ext = ext;

        /// validate filename's ext
        bool found = false;
        std::vector<std::string> valid_ext = get_valid_ext();
        for (int i=0; i<valid_ext.size(); i++)
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
            for (int i=0; i<valid_ext.size(); i++)
            {
                meta_info.err_msg = meta_info.err_msg + " " + valid_ext[i];
            }
            break;
        }

        // validate filename's head
        if (ext=="jpg" || ext=="png" || ext=="bmp")
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
        if (underline_pos!=-1 && underline_pos > static_cast<int>(head.length())-4)
        {
            meta_info.valid = false;
            meta_info.err_msg = "filename's head invalid.  [prefix]_[width]x[height] required, `_` position invalid now";
            break;
        }

        std::string dim_str = head.substr(underline_pos+1);
        int non_digit_cnt = 0;
        int non_digit_pos = -1;
        for (int i=0; i<dim_str.length(); i++)
        {
            if (!isdigit(dim_str[i]))
            {
                non_digit_cnt ++;
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
        for (int i=0; i<non_digit_pos; i++)
        {
            width = width * 10 + (dim_str[i] - '0');
        }
        for (int i=non_digit_pos+1; i<dim_str.length(); i++)
        {
            height = height * 10 + (dim_str[i] - '0');
        }
        meta_info.height = height;
        meta_info.width = width;

        int actual_size = get_file_size(filename);
        int expected_size = -1;
        if (ext=="nv21" || ext=="nv12")
        {
            expected_size = height * width * 3 / 2;
        }
        else if (ext=="rgb24" || ext=="bgr24")
        {
            expected_size = height * width * 3;
        }

        if (expected_size != actual_size)
        {
            meta_info.valid = false;
            meta_info.err_msg = "invalid file size, filename described different that actual";
            break;
        }

        if (ext=="nv21" || ext=="nv12")
        {
            if (height%2!=0 || width%2!=0)
            {
                meta_info.valid = false;
                meta_info.err_msg = "file dimension invalid. both height and width should be even number";
                break;
            }
        }
    } while (0);

    return meta_info;
}

cv::Mat read_image(const FileMetaInfo& meta_info)
{
    cv::Mat image;
    if (meta_info.ext=="bmp" || meta_info.ext=="jpg" || meta_info.ext=="png")
    {
        image = cv::imread(meta_info.filename);
    }
    else if (meta_info.ext=="nv21" || meta_info.ext=="nv12")
    {
        int height = meta_info.height;
        int width = meta_info.width;
        FILE* fin = fopen(meta_info.filename.c_str(), "rb");
        int buf_size = height * width * 3 / 2;

        cv::Mat yuv420sp_mat(height*3/2, width, CV_8UC1);
        uchar* yuv_buf = yuv420sp_mat.data;
        fread(yuv_buf, buf_size, 1, fin);
        fclose(fin);

        cv::Size size;
        size.height = height;
        size.width = width;
        image = cv::Mat(size, CV_8UC3);


        if (meta_info.ext=="nv21") // nv21 => bgr
        {
            cv::cvtColor(yuv420sp_mat, image, cv::COLOR_YUV2BGR_NV21);
        }
        else if (meta_info.ext=="nv12") // nv12 => bgr
        {
            cv::cvtColor(yuv420sp_mat, image, cv::COLOR_YUV2BGR_NV12);
        }
    }
    else if (meta_info.ext=="bgr24" || meta_info.ext=="rgb24")
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

        if (meta_info.ext=="rgb24") // bgr => rgb, inplace
        {
            cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        }
    }
    return image;
}



cv::Mat imk::loadImage(const std::string& image_path)
{
    FileMetaInfo meta_info = get_meat_info(image_path);
    if (!meta_info.valid)
    {
        fprintf(stderr, "%s\n", meta_info.err_msg.c_str());
        return cv::Mat(100, 100, CV_8UC3);
    } else {
        cv::Mat image = read_image(meta_info);
        return image;
    }
}