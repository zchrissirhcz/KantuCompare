#include <opencv2/core/types.hpp>
#include <string>
#include <opencv2/opencv.hpp>
#include "tinyfiledialogs.h"
#include "imageutils.h"

#include "app_design.hpp"
#include "image_compare_core.hpp"

class MyApp : public App<MyApp>
{
public:
    void Update()
    {
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        if (show_image_window)
            ShowImageWindow(&show_image_window);
    }
    
    void StartUp()
    {
        glfwSetWindowTitle(window, u8"Simple Image Compare - 简易图像对比工具");

        // Load Fonts
        //std::string font_path = "/System/Library/Fonts/PingFang.ttc"; // system wide
        std::string font_path = "/Users/zz/Library/Fonts/SourceHanSansCN-Normal.otf"; // user installed
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
        io.Fonts->Build();
        IM_ASSERT(font != NULL);

#if IMGUI_WITH_DOCKING
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif
    }

private:
    void ShowImageWindow(bool* p_open = NULL);
    void ShowImage(const char* windowName, bool *open, const RichImage& image);
    void LoadImage();

private:
    bool show_demo_window = false;
    bool show_image_window = true;
    const char* filepath = NULL;
};

void MyApp::LoadImage()
{
    char const* lFilterPatterns[3] = { "*.jpg", "*.png", "*.jpeg" };
    const char* lTheOpenFileName = tinyfd_openFileDialog(
                        "let us read the password back",
                        "",
                        3,
                        lFilterPatterns,
                        NULL,
                        0);
    if (!lTheOpenFileName)
    {
        tinyfd_messageBox(
                "Error",
                "Open file name is NULL",
                "ok",
                "error",
                1);
    }
    else
    {
        std::cout << "file choosed: " << lTheOpenFileName << std::endl;
        filepath = lTheOpenFileName;
    }
}

void MyApp::ShowImage(const char* windowName, bool *open, const RichImage& image)
{
    if (*open)
    {
        ImGui::SetNextWindowBgAlpha(0.4f); // Transparent background

        GLuint texture = image.get_texture();
        ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(INFINITY, INFINITY));

        if (ImGui::Begin(windowName, open))
        {
            ImVec2 p_min = ImGui::GetCursorScreenPos(); // actual position
            ImVec2 p_max = ImVec2(ImGui::GetContentRegionAvail().x + p_min.x, ImGui::GetContentRegionAvail().y  + p_min.y);
            ImGui::GetWindowDrawList()->AddImage((void*)(uintptr_t)texture, p_min, p_max);
        }
        ImGui::End();
    }
}

static cv::Mat compare_two_mat(const cv::Mat& image_left, const cv::Mat& image_right, int toleranceThresh)
{
    // TODO: support 1, 2, 4 channel image comparison
    cv::Mat diff;
    if (image_left.empty() && image_right.empty())
    {
        diff.create(256, 256, CV_8UC3);
        diff = cv::Scalar(128, 128, 128);
    }
    else if (image_left.empty())
    {
        diff = image_right.clone();
    }
    else if (image_left.empty())
    {
        diff = image_left.clone();
    }
    else
    {
        cv::Scalar pixel_diff;
        cv::Mat diff_image_left;
        cv::Mat diff_image_right;
        cv::Mat diff_image_compare;
        const int channels = image_left.channels();

        cv::Mat image_compare;
        if (image_left.size() != image_right.size())
        {
            cv::Size big_size;
            big_size.height = std::max(image_left.size().height, image_right.size().height);
            big_size.width = std::max(image_left.size().width, image_right.size().width);

            cv::Mat image_left_big(big_size, image_left.type(), cv::Scalar(0));
            cv::Mat image_right_big(big_size, image_right.type(), cv::Scalar(0));
            for (int i = 0; i < image_left.rows; i++)
            {
                for (int j = 0; j < image_left.cols; j++)
                {
                    for (int k = 0; k < channels; k++)
                    {
                        image_left_big.ptr(i, j)[k] = image_left.ptr(i, j)[k];
                    }
                }
            }

            for (int i = 0; i < image_right.rows; i++)
            {
                for (int j = 0; j < image_right.cols; j++)
                {
                    for (int k = 0; k < channels; k++)
                    {
                        image_right_big.ptr(i, j)[k] = image_right.ptr(i, j)[k];
                    }
                }
            }

            cv::addWeighted(image_left_big, 1.0, image_right_big, 1.0, 0.0, image_compare);

            const int roi_width = std::min(image_left.size().width, image_right.size().width);
            const int roi_height = std::min(image_left.size().height, image_right.size().height);
            cv::Rect rect(0, 0, roi_width, roi_height);

            diff_image_left = image_left(rect);
            diff_image_right = image_right(rect);
            diff_image_compare = image_compare(rect);
        }
        else  // size equal
        {
            diff_image_left = image_left;
            diff_image_right = image_right;
            image_compare.create(image_left.size(), image_left.type());
            diff_image_compare = image_compare;
        }

        cv::absdiff(diff_image_left, diff_image_right, diff_image_compare);
        pixel_diff = cv::sum(diff_image_compare);
        int sum = pixel_diff.val[0] + pixel_diff.val[1] + pixel_diff.val[2] + pixel_diff.val[3];
        //cv::setNumThreads(1);
        
        if (sum == 0) {
            // if the left and right image is differnt size, but same in the overlaped region, we compute the gray image, but assign to RGB pixels
            cv::Size diff_size = diff_image_compare.size();
            for (int i = 0; i < diff_size.height; i++)
            {
                for (int j = 0; j < diff_size.width; j++)
                {
                    // TODO: what if the channels is not equal to 3? e.g. 4 channels, or 1 channel?
                    float R2Y = 0.299;
                    float G2Y = 0.587;
                    float B2Y = 0.114;
                    int B = diff_image_left.ptr(i, j)[0];
                    int G = diff_image_left.ptr(i, j)[1];
                    int R = diff_image_left.ptr(i, j)[2];
                    int gray = cv::saturate_cast<uchar>(R2Y * R + G2Y * G + B2Y * B);

                    diff_image_compare.ptr(i, j)[0] = gray;
                    diff_image_compare.ptr(i, j)[1] = gray;
                    diff_image_compare.ptr(i, j)[2] = gray;
                }
            }
        }
        else
        {
            cv::Scalar above_color(255-50, 0, 0);
            cv::Scalar below_color(0, 0, 255-50);
            cv::Mat diff;
            imk::getDiffImage(diff_image_left, diff_image_right, diff, toleranceThresh, below_color, above_color);

            cv::Mat blend;
            cv::addWeighted(diff_image_left, 0.5, diff_image_right, 0.5, 0.0, blend);

            addWeighted(diff, 0.7, blend, 0.3, 0.0, diff_image_compare);
        }

        diff = diff_image_compare.clone();
        printf("compare get pixel diff: (%d, %d, %d)\n", 
            (int)pixel_diff.val[0],
            (int)pixel_diff.val[1],
            (int)pixel_diff.val[2]
        );
    }

    return diff;
}

void MyApp::ShowImageWindow(bool* p_open)
{
    //static std::vector<RichImage> data;
    static RichImage imageLeft;
    static RichImage imageRight;
    static bool window2 = false;
    static int  selectedItem = -1;
    static int actualitem = -1;
    static bool show_diff_image = false;
    static bool input_image_updated = false;

    ImGui::ShowMetricsWindow(&window2);
    ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
    if (ImGui::Begin("name", p_open))
    {
        // Load Image
        if (ImGui::Button("Load Image1"))
        {
            LoadImage();
            if (filepath != NULL)
            {
                cv::Mat mat = cv::imread(filepath); 
                imageLeft.load_mat(mat);
                imageLeft.set_name(filepath);
                filepath = NULL;
            }
            input_image_updated = true;
        }
        if (ImGui::Button("Load Image2"))
        {
            LoadImage();
            if (filepath != NULL)
            {
                cv::Mat mat = cv::imread(filepath); 
                imageRight.load_mat(mat);
                imageRight.set_name(filepath);
                filepath = NULL;
            }
            input_image_updated = true;
        }
        
        // Display test image
        static RichImage diff_image;
        static int diff_thresh = 1;
        int old_diff_thresh = diff_thresh;
        ImGui::SliderInt("diff thresh", &diff_thresh, 0, 255);
        if (diff_thresh != old_diff_thresh)
        {
            input_image_updated = true;
            old_diff_thresh = diff_thresh;
        }

        if ((!imageLeft.mat.empty() && !imageRight.mat.empty() && input_image_updated))
        {
            cv::Mat diff_mat;
            if (!imageLeft.mat.empty() && !imageRight.mat.empty())
            {
                diff_mat = compare_two_mat(imageLeft.mat, imageRight.mat, diff_thresh);
            }
            else
            {
                diff_mat.create(255, 255, CV_8UC3);
                diff_mat = cv::Scalar(128, 128, 128);
            }

            if (diff_image.mat.empty())
            {
                diff_image.clear(); // free texture memory
            }
            diff_image.load_mat(diff_mat);
            show_diff_image = true;
            input_image_updated = false;
        }

        if (show_diff_image)
        {
            ShowImage("Diff Image", &show_diff_image, diff_image);
        }

        if (!imageLeft.mat.empty())
        {
            std::string winname = std::string("Image1 - ") + imageLeft.get_name();
            ShowImage(winname.c_str(), imageLeft.get_open(), imageLeft);
        }
        if (!imageRight.mat.empty())
        {
            std::string winname = std::string("Image2 - ") + imageRight.get_name();
            ShowImage(winname.c_str(), imageRight.get_open(), imageRight);
        }
    }
    ImGui::End();
}


int main(int, char**)
{
    MyApp app;
    app.Run();

    return 0;
}
