#include <cstddef>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "app_design.hpp"
#include "tinyfiledialogs.h"
#include "RichImage.hpp"
#include "image_compare_core.hpp"

#include "my_widgets.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

class MyApp : public App<MyApp>
{
public:
    MyApp() = default;
    ~MyApp() = default;

    void myUpdateMouseWheel();

    void StartUp()
    {
        // Title
        glfwSetWindowTitle(window, u8"Simple Image Compare Tool");
        glfwSetWindowSize(window, 960, 640);

        // Style
        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 8.f;
        style.ChildRounding = 6.f;
        style.FrameRounding = 6.f;
        style.PopupRounding = 6.f;
        style.GrabRounding = 6.f;
        style.ChildRounding = 6.f;

        // Load Fonts only on specific OS for portability
        //std::string font_path = "/System/Library/Fonts/PingFang.ttc"; // system wide
        ImGuiIO& io = ImGui::GetIO();
#if __APPLE__
        // std::string font_path = "/Users/zz/Library/Fonts/SourceHanSansCN-Normal.otf"; // user installed
        // ImFont* font = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
        // io.Fonts->Build();
        // IM_ASSERT(font != NULL);
#endif

        //io.ConfigWindowsMoveFromTitleBarOnly = true;

#if IMGUI_WITH_DOCKING
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

        //io.FontAllowUserScaling = true;

    }

    void Update()
    {
        myUpdateMouseWheel();

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 display_size = io.DisplaySize;
        ImVec2 mvp_size = main_viewport->Size;
        int min_len = std::min(mvp_size.x, mvp_size.y) / 2;
        ImVec2 win_size(min_len, min_len);
        ImGuiCond_ cond = ImGuiCond_FirstUseEver;
        ImGuiWindowFlags flags = 0; //ImGuiWindowFlags_NoTitleBar;

        const char* image_path1 = NULL;

        ImGui::SetNextWindowPos(ImVec2(mvp_size.x/5, 0), cond);
        ImGui::SetNextWindowSize(win_size, cond);
        ImGui::Begin("Meta1", NULL, flags);
        {
            float x = ImGui::GetCursorPosX();
            ImGui::SameLine(ImGui::GetWindowWidth()-50); // align to the right
            if (ImGui::Button("Load1"))
            {
                LoadImage(imageLeft);
                compare_condition_updated = true;
            }
            if (!imageLeft.mat.empty())
            {
                std::string text = cv::format("%s\nW=%d,H=%d; %d bytes", imageLeft.get_name(), imageLeft.mat.size().width, imageLeft.mat.size().height, imageLeft.filesize);
                ImGui::SameLine();
                ImGui::SetCursorPosX(x); // align back to the left

                ImGui::Text("%s", text.c_str());
            }
        }
        ImGui::End();


        ImGui::Begin("Image1", NULL, flags);
        {
            if (!imageLeft.mat.empty())
            {
                std::string winname = std::string("Image1 - ") + imageLeft.get_name();
                ShowImage(winname.c_str(), imageLeft.get_open(), imageLeft);
            }
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(mvp_size.x/2, 0), cond);
        ImGui::SetNextWindowSize(win_size, cond);
        ImGui::Begin("Meta2", NULL, flags);
        {
            float x = ImGui::GetCursorPosX();
            ImGui::SameLine(ImGui::GetWindowWidth()-50); // align to the right
            if (ImGui::Button("Load"))
            {
                LoadImage(imageRight);
                compare_condition_updated = true;
            }
            if (!imageRight.mat.empty())
            {
                std::string text = cv::format("%s\nW=%d,H=%d; %d bytes", imageRight.get_name(), imageRight.mat.size().width, imageRight.mat.size().height, imageRight.filesize);
                ImGui::SameLine();
                ImGui::SetCursorPosX(x); // align back to the left

                ImGui::Text("%s", text.c_str());
            }
        }
        ImGui::End();


        ImGui::Begin("Image2", NULL, flags);
        {
            if (!imageRight.mat.empty())
            {
                std::string winname = std::string("Image2 - ") + imageRight.get_name();
                ShowImage(winname.c_str(), imageRight.get_open(), imageRight);
            }
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(mvp_size.x*4/5, mvp_size.y/2), cond);
        ImGui::SetNextWindowSize(win_size, cond);
        ImGui::Begin("Meta3", NULL, flags);
        {
            int old_diff_thresh = diff_thresh;
            ImGuiSliderFlags slider_flags = ImGuiSliderFlags_NoInput;
            // slider will always be 256 pixel wide
            ImGui::PushItemWidth(256);
            //ImGui::NewLine();
            ImGui::Text("Tolerance");
            ImGui::SliderInt("##Tolerance", &diff_thresh, 0, 255, "%d", slider_flags);
            //ImGui::VSliderInt("Tolerance", ImVec2(30, win_size.y*0.9), &diff_thresh, 0, 255);
            //scp::VSliderInt("tolerance", ImVec2(30, win_size.y*0.9), &diff_thresh, 0, 255);
            //ImGui::SameLine();
            if (diff_thresh != old_diff_thresh)
            {
                compare_condition_updated = true;
                old_diff_thresh = diff_thresh;
            }
            ComputeDiffImage();
        }
        ImGui::End();

        ImGui::Begin("DiffImage", NULL, flags);
        {
            if (show_diff_image)
            {
                ShowImage("Diff Image", &show_diff_image, diff_image);
            }
        }
        ImGui::End();
    }

private:
    void UI_ChooseImageFile();
    void LoadImage(RichImage& image);
    void ComputeDiffImage();
    void ShowImage(const char* windowName, bool *open, const RichImage& image);

private:
    bool window_open = true;
    bool Check = true;
    const char* filepath = NULL;
    RichImage imageLeft;
    RichImage imageRight;
    RichImage diff_image;
    bool compare_condition_updated = false;
    bool show_diff_image = false;
    int diff_thresh = 1;
};

static const float WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER    = 2.00f;    // Lock scrolled window (so it doesn't pick child windows that are scrolling through) for a certain time, unless mouse moved.

static void StartLockWheelingWindow(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    if (g.WheelingWindow == window)
        return;
    g.WheelingWindow = window;
    g.WheelingWindowRefMousePos = g.IO.MousePos;
    g.WheelingWindowTimer = WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER;
}

// When with mouse wheel moving (vertically), and current window name contains 'Image', resize current window's size
void MyApp::myUpdateMouseWheel()
{
    ImGuiContext& g = *GImGui;

    ImGuiWindow* cur_window = g.WheelingWindow;

    // Reset the locked window if we move the mouse or after the timer elapses
    if (cur_window != NULL)
    {
        std::string window_name = cur_window->Name;
        if (window_name.length() < 5 || window_name.find("Image") == std::string::npos)
        {
            return;
        }
        //printf("!! cur_window->Name: %s\n", cur_window->Name);
        
        g.WheelingWindowTimer -= g.IO.DeltaTime;
        if (ImGui::IsMousePosValid() && ImLengthSqr(g.IO.MousePos - g.WheelingWindowRefMousePos) > g.IO.MouseDragThreshold * g.IO.MouseDragThreshold)
            g.WheelingWindowTimer = 0.0f;
        if (g.WheelingWindowTimer <= 0.0f)
        {
            g.WheelingWindow = NULL;
            g.WheelingWindowTimer = 0.0f;
        }
    }

    float wheel_y = g.IO.MouseWheel;

    if ((g.ActiveId != 0 && g.ActiveIdUsingMouseWheel) || (g.HoveredIdPreviousFrame != 0 && g.HoveredIdPreviousFrameUsingMouseWheel))
        return;

    ImGuiWindow* window = g.WheelingWindow ? g.WheelingWindow : g.HoveredWindow;
    if (!window || window->Collapsed)
        return;

    if (wheel_y != 0.0f)
    {
        StartLockWheelingWindow(window);
        const float new_font_scale = ImClamp(window->FontWindowScale + g.IO.MouseWheel * 0.10f, 0.50f, 2.50f);
        const float scale = new_font_scale / window->FontWindowScale;
        window->FontWindowScale = new_font_scale;
        if (window == window->RootWindow)
        {
            const ImVec2 offset = window->Size * (1.0f - scale) * (g.IO.MousePos - window->Pos) / window->Size;
            ImGui::SetWindowPos(window, window->Pos + offset, 0);
            window->Size = ImFloor(window->Size * scale);
            window->SizeFull = ImFloor(window->SizeFull * scale);
        }
        // TODO: Zoom the image/texture in the current window
        return;
    }
}

void MyApp::ShowImage(const char* windowName, bool *open, const RichImage& image)
{
    if (*open)
    {
        GLuint texture = image.get_texture();
        //ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(INFINITY, INFINITY));

        ImVec2 p_min = ImGui::GetCursorScreenPos(); // actual position
        ImVec2 p_max = ImVec2(ImGui::GetContentRegionAvail().x + p_min.x, ImGui::GetContentRegionAvail().y  + p_min.y);

        //ImGui::BeginChild("Image1Content", ImVec2(0, 0), true);
        //ImGui::Begin("Image1Content", NULL);
        ImGui::GetWindowDrawList()->AddImage((void*)(uintptr_t)texture, p_min, p_max);
        //ImGui::EndChild();
        //ImGui::End();
        //
        //ImGui::Image((void*)(uintptr_t)texture, ImVec2(image.mat.size().width, image.mat.size().height));
        
    }
}

static cv::Mat compare_two_mat(const cv::Mat& image_left, const cv::Mat& image_right, int toleranceThresh)
{
    if (image_left.channels() != 4 || image_right.channels() != 4)
    {
        fprintf(stderr, "only support BGRA image for comparision\n");
        return cv::Mat();
    }
    const int channels = 4;

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

            // TODO: fix the transparency(alpha) for diff region in diff region(the intersection) and non-diff region(the union minus the intersection)
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

        if (sum == 0)
        {
            // if the left and right image is differnt size, but same in the overlaped region, we compute the gray image, but assign to RGB pixels
            cv::Size diff_size = diff_image_compare.size();
            for (int i = 0; i < diff_size.height; i++)
            {
                for (int j = 0; j < diff_size.width; j++)
                {
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
                    diff_image_compare.ptr(i, j)[3] = 255;
                }
            }
        }
        else
        {
            cv::Scalar above_color(0, 0, 255-50);
            cv::Scalar below_color(255-50, 0, 0);
            imk::getDiffImage(diff_image_left, diff_image_right, diff_image_compare, toleranceThresh, below_color, above_color);
        }

        diff = image_compare.clone();
        printf("Compare get pixel diff: (%d, %d, %d) with thresh %d\n",
            (int)pixel_diff.val[0],
            (int)pixel_diff.val[1],
            (int)pixel_diff.val[2],
            toleranceThresh
        );
    }

    return diff;
}

void MyApp::ComputeDiffImage()
{
    if ((!imageLeft.mat.empty() && !imageRight.mat.empty() && compare_condition_updated))
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
        compare_condition_updated = false;
        show_diff_image = true;
    }
}

void MyApp::UI_ChooseImageFile()
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
        printf("file choosed: %s\n", lTheOpenFileName);
        filepath = lTheOpenFileName;
    }
}

void MyApp::LoadImage(RichImage& image)
{
    UI_ChooseImageFile();
    if (filepath)
    {
        image.loadFromFile(filepath);
    }
    filepath = NULL;
}

int main( int argc, char** argv )
{
    MyApp app;
    app.Run();

    return 0;
}
