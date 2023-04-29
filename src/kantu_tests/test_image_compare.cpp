#include "gtest/gtest.h"

#ifndef STR_IMPLEMENTATION
#define STR_IMPLEMENTATION 1
#endif
#include "kantu/image_io.hpp"
#include "kantu/compare.hpp"
#include "kantu/string.hpp"

TEST(FilePath, t1)
{
    const std::string path = "C:\\Users\\zz\\data\\lena.png";
    kantu::FilePath filepath(path);
    EXPECT_EQ(filepath.path(), path);
    EXPECT_EQ(filepath.directory(), "C:\\Users\\zz\\data");
    EXPECT_EQ(filepath.filename(), "lena.png");
    EXPECT_EQ(filepath.basename(), "lena");
    EXPECT_EQ(filepath.ext(), "png");
}

TEST(FilePath, t2)
{
    const std::string path = "C:\\Users\\zz\\data\\lena";
    kantu::FilePath filepath(path);
    EXPECT_EQ(filepath.path(), path);
    EXPECT_EQ(filepath.directory(), "C:\\Users\\zz\\data");
    EXPECT_EQ(filepath.filename(), "lena");
    EXPECT_EQ(filepath.basename(), "lena");
    EXPECT_EQ(filepath.ext(), "");
}

TEST(FilePath, t3)
{
    const std::string path = "/home/zz/data/lena.png";
    kantu::FilePath filepath(path);
    EXPECT_EQ(filepath.path(), path);
    EXPECT_EQ(filepath.directory(), "/home/zz/data");
    EXPECT_EQ(filepath.filename(), "lena.png");
    EXPECT_EQ(filepath.basename(), "lena");
    EXPECT_EQ(filepath.ext(), "png");
}

TEST(FilePath, t4)
{
    const std::string path = "C:\\Users\\zz\\data/lena.png";
    kantu::FilePath filepath(path);
    EXPECT_EQ(filepath.path(), path);
    EXPECT_EQ(filepath.directory(), "C:\\Users\\zz\\data");
    EXPECT_EQ(filepath.filename(), "lena.png");
    EXPECT_EQ(filepath.basename(), "lena");
    EXPECT_EQ(filepath.ext(), "png");
}

TEST(ImageFileName, non_raw)
{
    std::string head = "temp";
    std::vector<std::string> image_exts = {
        "jpg", "png", "bmp", "jpeg", "jpg",
        "JPG", "PNG", "BMP", "JPEG", "JPG"
    };
    for (const std::string& ext : image_exts)
    {
        std::string lower_ext = kantu::to_lower(ext);
        std::string image_path = head + "." + ext;
        kantu::ImageFileInfo fileinfo(image_path);

        EXPECT_EQ(fileinfo.valid, true);
        EXPECT_EQ(fileinfo.filepath, image_path);
        EXPECT_EQ(fileinfo.head, head);
        EXPECT_EQ(fileinfo.ext, ext);
        EXPECT_EQ(fileinfo.lower_ext, lower_ext);
        EXPECT_EQ(fileinfo.height, 0);
        EXPECT_EQ(fileinfo.width, 0);
    }
}

// TEST(ImageFileName, raw)
// {
//     std::string head = "temp";
//     const int height = 1280;
//     const int width = 720;
//     std::vector<std::string> image_exts = {
//         "nv21", "nv12", "rgb", "bgr", "i420", "i444",
//         "NV21", "NV12", "RGB", "BGR", "I420", "I444",
//     };
//     for (const std::string& ext : image_exts)
//     {
//         std::string lower_ext = kantu::to_lower(ext);
//         std::string image_path = head + "_" + std::to_string(width) + "x" + std::to_string(height) + "." + ext;
//         kantu::ImageFileInfo fileinfo(image_path);

//         EXPECT_EQ(fileinfo.valid, true);
//         EXPECT_EQ(fileinfo.filename, image_path);
//         EXPECT_EQ(fileinfo.head, head);
//         EXPECT_EQ(fileinfo.ext, ext);
//         EXPECT_EQ(fileinfo.lower_ext, lower_ext);
//         EXPECT_EQ(fileinfo.height, height);
//         EXPECT_EQ(fileinfo.width, width);
//     }
// }

TEST(ImageFileName, head)
{
#if _MSC_VER
    std::string image_path = "C:/Users/zz/data/1920x1080.NV21";
    kantu::ImageFileInfo fileinfo(image_path);
    EXPECT_EQ(fileinfo.head, "1920X1080");
#endif
}

// 1. 文件名字 ImageFileName
// 2. 文件内容 ImageFile
// 3. 格式转换 TransformFormat
// 4. 图像比对 CompareImage
// 5. 渲染结果 RenderResult