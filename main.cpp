#include <stdio.h>
#include <stddef.h>

//#include <string>
#define STR_IMPLEMENTATION
#include "Str.h"
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "app_design.hpp"
#include "tinyfiledialogs.h"
#include "RichImage.hpp"
#include "image_compare_core.hpp"
#include "imgInspect.h"

class MyApp : public App<MyApp>
{
public:
    MyApp() = default;
    ~MyApp() = default;

    void myUpdateMouseWheel();

    void StartUp()
    {
        // Title
        glfwSetWindowTitle(window, u8"Image Compare");
        glfwSetWindowSize(window, 960, 640);

        // Style
        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        // style.WindowRounding = 8.f;
        // style.ChildRounding = 6.f;
        // style.FrameRounding = 6.f;
        // style.PopupRounding = 6.f;
        // style.GrabRounding = 6.f;
        // style.ChildRounding = 6.f;

        // Load Fonts only on specific OS for portability
        //std::string font_path = "/System/Library/Fonts/PingFang.ttc"; // system wide
        ImGuiIO& io = ImGui::GetIO();
#if __APPLE__ && __ARM_NEON
        std::string font_path = "/Users/zz/Library/Fonts/SourceHanSansCN-Normal.otf"; // user installed
        ImFont* font = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
        io.Fonts->Build();
        IM_ASSERT(font != NULL);
#endif

        //io.ConfigWindowsMoveFromTitleBarOnly = true;

#if IMGUI_WITH_DOCKING
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

        //io.FontAllowUserScaling = true;

    }

    void showText(const char* text, const char* inputId)
    {
        char input[256];
        strcpy(input, text);
        ImGui::PushID(inputId);
        ImGui::PushItemWidth(ImGui::GetWindowSize().x);
        ImGui::InputText("", input, 256, ImGuiInputTextFlags_ReadOnly);
        ImGui::PopItemWidth();
        ImGui::PopID();
    }

    void Update()
    {
        //ImGui::ShowDemoWindow();

        myUpdateMouseWheel(); // not working now.

        static bool use_work_area = true;
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse;

        // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
        // Based on your use case you may want one of the other.
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
        ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

        // ImGuiIO& io = ImGui::GetIO();
        // ImVec2 display_size(io.DisplaySize.x, io.DisplaySize.y);
        // ImGui::SetNextWindowSize(display_size);

        ImGui::Begin("Testing menu", NULL, flags);
        
        ImGui::BeginChild("##PathRegion", ImVec2(ImGui::GetWindowWidth() - 50, ImGui::GetWindowHeight() * 1 / 10), true, ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::BeginChild("##leftpath", ImVec2(ImGui::GetWindowWidth() / 2 - 10, ImGui::GetWindowHeight()), false);
            if (ImGui::Button("Load##1"))
            {
                LoadImage(imageLeft);
                compare_condition_updated = true;
            }
            if (!imageLeft.mat.empty())
            {
                
                ImGui::SameLine();
                //ImGui::SetCursorPosX(x); // align back to the left

                //ImGui::Text("%s", text.c_str());
                showText(imageLeft.get_name(), "1");
                Str256 meta_info;
                meta_info.setf("W=%d,H=%d; %d bytes", imageLeft.mat.size().width, imageLeft.mat.size().height, imageLeft.filesize);
                showText(meta_info.c_str(), "2");
            }
            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
            ImGui::SameLine();

            ImGui::BeginChild("##rightpath", ImVec2(ImGui::GetWindowWidth() / 2 - 10, ImGui::GetWindowHeight()), false);
            if (ImGui::Button("Load##2"))
            {
                LoadImage(imageRight);
                compare_condition_updated = true;
            }
            if (!imageRight.mat.empty())
            {
                Str256 text;
                text.setf("%s\nW=%d,H=%d; %d bytes", imageRight.get_name(), imageRight.mat.size().width, imageRight.mat.size().height, imageRight.filesize);
                ImGui::SameLine();
                //ImGui::SetCursorPosX(x); // align back to the left

                //ImGui::Text("%s", text.c_str());
                showText(imageRight.get_name(), "3");
                Str256 meta_info;
                meta_info.setf("W=%d,H=%d; %d bytes", imageRight.mat.size().width, imageRight.mat.size().height, imageRight.filesize);
                showText(meta_info.c_str(), "4");
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::BeginChild("##InputImagesRegion", ImVec2(ImGui::GetWindowWidth()-50, ImGui::GetWindowHeight() * 4 / 10), true);
        {
            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            ImVec2 vMax = ImGui::GetWindowContentRegionMax();

            // vMin.x += ImGui::GetWindowPos().x;
            // vMin.y += ImGui::GetWindowPos().y;
            // vMax.x += ImGui::GetWindowPos().x;
            // vMax.y += ImGui::GetWindowPos().y;
            float window_content_height = vMax.y - vMin.y;
            ImGui::BeginChild("Image1", ImVec2(ImGui::GetWindowWidth() / 2 - 10, window_content_height), false);
            if (!imageLeft.mat.empty())
            {
                //std::string winname = std::string("Image1 - ") + imageLeft.get_name();
                Str256 winname;
                winname.setf("Image1 - %s", imageLeft.get_name());
                ShowImage(winname.c_str(), imageLeft.get_open(), imageLeft, 1.0f);
            }
            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
            ImGui::SameLine();

            ImGui::BeginChild("Image2", ImVec2(ImGui::GetWindowWidth() / 2 - 10, window_content_height), false);
            if (!imageRight.mat.empty())
            {
                //std::string winname = std::string("Image2 - ") + imageRight.get_name();
                Str256 winname;
                winname.setf("Image2 - %s", imageLeft.get_name());
                ShowImage(winname.c_str(), imageRight.get_open(), imageRight, 0.0f);
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        //ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
        //ImGui::SameLine();

        ImGui::BeginChild("##CompareResultRegion", ImVec2(ImGui::GetWindowWidth()-50, ImGui::GetWindowHeight() * 5 / 10), false);
        {
            ImGui::BeginChild("###ConfigRegion", ImVec2(ImGui::GetWindowWidth()/4, ImGui::GetWindowHeight()), false);
            {
                // zoom
                {
                    int old_zoom_percent = zoom_percent;
                    ImGui::PushItemWidth(200);
                    char text[20] = {0};
                    sprintf(text, "Zoom: %d%%", zoom_percent);
                    ImGui::Text("%s", text);
                    ImGuiSliderFlags zoom_slider_flags = ImGuiSliderFlags_NoInput;
                    ImGui::SliderInt("##Zoom", &zoom_percent, zoom_percent_min, zoom_percent_max, "", zoom_slider_flags);
                    if (zoom_percent != old_zoom_percent)
                    {
                        old_zoom_percent = zoom_percent;
                    }
                }
                // tolerance
                {
                    int old_diff_thresh = diff_thresh;
                    ImGui::PushItemWidth(256);
                    char text[20] = {0};
                    sprintf(text, "Tolerance: %d", diff_thresh);
                    ImGui::Text("%s", text);
                    ImGuiSliderFlags tolerance_slider_flags = ImGuiSliderFlags_NoInput;
                    ImGui::SliderInt("##Tolerance", &diff_thresh, 0, 255, "", tolerance_slider_flags);
                    if (diff_thresh != old_diff_thresh)
                    {
                        compare_condition_updated = true;
                        old_diff_thresh = diff_thresh;
                    }
                }
                {
                    ImGui::Checkbox("Inspect Pixels", &inspect_pixels);
                }
                {
                    if (ImGui::Button("Reload Input Images"))
                    {
                        if (!imageLeft.mat.empty())
                        {
                            imageLeft.reload();
                            compare_condition_updated = true;
                        }
                        if (!imageRight.mat.empty())
                        {
                            imageRight.reload();
                            compare_condition_updated = true;
                        }
                    }
                }
            }
            ImGui::EndChild();

            ComputeDiffImage();

            ImGui::SameLine();
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
            ImGui::SameLine();

            ImGui::BeginChild("###RightImage", ImVec2(ImGui::GetWindowWidth() * 4 / 5 - 50, ImGui::GetWindowHeight()), false);
            if (show_diff_image)
            {
                ShowImage("Diff Image", &show_diff_image, diff_image, 0.3f);
                ImGuiWindow* window = ImGui::GetCurrentWindow();
                if (window->ScrollbarX || window->ScrollbarY)
                {
                    ImGui::PushClipRect(window->OuterRectClipped.Min, window->OuterRectClipped.Max, false);
                    if (window->ScrollbarX)
                    {
                        ImRect r = ImGui::GetWindowScrollbarRect(window, ImGuiAxis_X);
                        window->DrawList->AddRect(r.Min, r.Max, IM_COL32(255, 0, 0, 255));
                    }
                    if (window->ScrollbarY)
                    {
                        ImRect r = ImGui::GetWindowScrollbarRect(window, ImGuiAxis_Y);
                        window->DrawList->AddRect(r.Min, r.Max, IM_COL32(255, 0, 0, 255));
                    }
                    ImGui::PopClipRect();
                }

                {
                    ImGuiIO& io = ImGui::GetIO();

                    ImRect rc = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                    ImVec2 mouseUVCoord = (io.MousePos - rc.Min) / rc.GetSize();
                    mouseUVCoord.y = 1.f - mouseUVCoord.y;
                    if (inspect_pixels && mouseUVCoord.x >= 0.f && mouseUVCoord.y >= 0.f)
                    {
                        int width = diff_image.mat.size().width;
                        int height = diff_image.mat.size().height;

                        //imageInspect(width, height, pickerImage.GetBits(), mouseUVCoord, displayedTextureSize);
                        ImVec2 displayedTextureSize(8, 8);
                        ImageInspect::inspect(width, height, diff_image.mat.data, mouseUVCoord, displayedTextureSize);
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        //ImGui::SameLine();
        //ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
        //ImGui::SameLine();

        // ImGui::BeginChild("##StatusBarRegion", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() / 10), false);
        // ImGui::Text("Important status(TODO)");
        // ImGui::EndChild();
        //StatusbarUI();

        ImGui::End();
    }

private:
    void UI_ChooseImageFile();
    void LoadImage(RichImage& image);
    void ComputeDiffImage();
    void ShowImage(const char* windowName, bool *open, const RichImage& image, float align_to_right_ratio = 0.f);

    void StatusbarUI();

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
    int zoom_percent = 46;
    int zoom_percent_min = 10;
    int zoom_percent_max = 1000;
    bool inspect_pixels = false;

    const float statusbarSize = 50;
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
        // std::string window_name = cur_window->Name;
        // if (window_name.length() < 5 || window_name.find("Image") == std::string::npos)
        // {
        //     return;
        // }
        // printf("!! cur_window->Name: %s\n", cur_window->Name);
        
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
        
        zoom_percent = zoom_percent + g.IO.MouseWheel * 5;
        if (zoom_percent > zoom_percent_max)
        {
            zoom_percent = zoom_percent_max;
        }
        else if (zoom_percent < zoom_percent_min)
        {
            zoom_percent = zoom_percent_min;
        }

        return;
    }
}

void MyApp::StatusbarUI()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - statusbarSize));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, statusbarSize));
    //ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 0
        //| ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoSavedSettings
        ;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ImGui::Begin("STATUSBAR", NULL, window_flags);
    ImGui::PopStyleVar();
    ImGui::Text("Status bar message.");
    ImGui::End();
    ImGui::PopStyleColor();
}

void MyApp::ShowImage(const char* windowName, bool *open, const RichImage& image, float align_to_right_ratio)
{
    if (*open)
    {
        GLuint texture = image.get_texture();
        //ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(INFINITY, INFINITY));

        // ImVec2 p_min = ImGui::GetCursorScreenPos(); // actual position
        // ImVec2 p_max = ImVec2(ImGui::GetContentRegionAvail().x + p_min.x, ImGui::GetContentRegionAvail().y  + p_min.y);

        //ImGui::BeginChild("Image1Content", ImVec2(0, 0), true);
        //ImGui::Begin("Image1Content", NULL);
        //ImGui::GetWindowDrawList()->AddImage((void*)(uintptr_t)texture, p_min, p_max);
        //ImGui::EndChild();
        //ImGui::End();
        //
        ImVec2 actual_image_size(image.mat.size().width, image.mat.size().height);
        ImVec2 rendered_texture_size = actual_image_size * (zoom_percent * 1.0 / 100);

        bool clamped_x_by_window = false;
        bool clamped_y_by_window = false;

        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 image_window_size = rendered_texture_size;
        if (rendered_texture_size.x > window_size.x)
        {
            //rendered_texture_size.x = window_size.x;
            clamped_x_by_window = true;
        }
        if (rendered_texture_size.y > window_size.y)
        {
            //rendered_texture_size.y = window_size.y;
            clamped_y_by_window = true;
        }
        bool clamped_by_window = clamped_x_by_window | clamped_y_by_window;
        if (clamped_by_window)
        {
            image_window_size = window_size;
        }

        if (align_to_right_ratio >= 0 && align_to_right_ratio <= 1)
        {
            ImVec2 win_size = ImGui::GetCurrentWindow()->Size;
            ImVec2 offset;
            offset.y = 0;
            if (clamped_x_by_window)
                offset.x = 0;
            else
                offset.x = (win_size.x - rendered_texture_size.x) * align_to_right_ratio;
            ImVec2 p_min = ImGui::GetCursorScreenPos() + offset;
            ImVec2 p_max = p_min + rendered_texture_size;
            //ImGui::GetWindowDrawList()->AddImage((void*)(uintptr_t)texture, p_min, p_max);
            ImGui::SetNextWindowPos(p_min);
        }
        else
        {
            //ImGui::Image((void*)(uintptr_t)texture, rendered_texture_size);
            ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        }

        //std::string label = cv::format("image##%d", texture);
        Str256 label;
        label.setf("image##%d", texture);
        ImGui::BeginChild(label.c_str(), image_window_size, clamped_by_window, ImGuiWindowFlags_HorizontalScrollbar);
        {
            ImGui::Image((void*)(uintptr_t)texture, rendered_texture_size);
        }
        ImGui::EndChild();
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
