#include <stdio.h>
#include <stddef.h>

//#include <string>
#include <vector>

#include "image_io.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "app_design.hpp"
#include "portable-file-dialogs.h"

#include "image_compare.hpp"
#include "image_render.hpp"
#include "imgInspect.h"

#define STR_IMPLEMENTATION
#include "Str.h"
using namespace imcmp;

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
        ImGuiStyle& style = ImGui::GetStyle();
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

        InitFileFilters();
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

        ImGui::BeginChild("##InputImagesRegion", ImVec2(ImGui::GetWindowWidth() - 50, ImGui::GetWindowHeight() * 4 / 10), true);
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

            ImGui::BeginChild("Image2", ImVec2(0, window_content_height), false);
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

        ImGui::BeginChild("##CompareResultRegion", ImVec2(ImGui::GetWindowWidth() - 50, ImGui::GetWindowHeight() * 5 / 10), false);
        {
            ImGui::BeginChild("###ConfigRegion", ImVec2(ImGui::GetWindowWidth() / 4, ImGui::GetWindowHeight() - 20), false);
            {
                // zoom
                {
                    ImGui::PushItemWidth(200);
                    char text[20] = {0};
                    sprintf(text, "Zoom: %d%%", zoom_percent);
                    ImGui::Text("%s", text);
                    ImGuiSliderFlags zoom_slider_flags = ImGuiSliderFlags_NoInput;
                    ImGui::SliderInt("##Zoom", &zoom_percent, zoom_percent_min, zoom_percent_max, "", zoom_slider_flags);
                }
                // tolerance
                {
                    ImGui::PushItemWidth(256);
                    char text[20] = {0};
                    sprintf(text, "Tolerance: %d", diff_thresh);
                    ImGui::Text("%s", text);
                    ImGuiSliderFlags tolerance_slider_flags = ImGuiSliderFlags_NoInput;
                    compare_condition_updated |= ImGui::SliderInt("##Tolerance", &diff_thresh, 0, 255, "", tolerance_slider_flags);
                }
                {
                    if (is_exactly_same)
                        ImGui::Text("Exactly Same: Yes");
                    else
                        ImGui::Text("Exactly Same: No");
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

            ImGui::BeginChild("###RightImage", ImVec2(0, ImGui::GetWindowHeight() - 20), false);
            if (show_diff_image)
            {
                ShowImage("Diff Image", &show_diff_image, diff_image, 0.3f);
                ImGuiWindow* win = ImGui::GetCurrentWindow();
                if (win->ScrollbarX || win->ScrollbarY)
                {
                    ImGui::PushClipRect(win->OuterRectClipped.Min, win->OuterRectClipped.Max, false);
                    if (win->ScrollbarX)
                    {
                        ImRect r = ImGui::GetWindowScrollbarRect(win, ImGuiAxis_X);
                        win->DrawList->AddRect(r.Min, r.Max, IM_COL32(255, 0, 0, 255));
                    }
                    if (win->ScrollbarY)
                    {
                        ImRect r = ImGui::GetWindowScrollbarRect(win, ImGuiAxis_Y);
                        win->DrawList->AddRect(r.Min, r.Max, IM_COL32(255, 0, 0, 255));
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
    int UI_ChooseImageFile();
    void LoadImage(RichImage& image);
    void ComputeDiffImage();
    void ShowImage(const char* windowName, bool* open, const RichImage& image, float align_to_right_ratio = 0.f);

    void StatusbarUI();
    void InitFileFilters();

private:
    bool window_open = true;
    bool Check = true;
    Str256 filepath;
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
    bool is_exactly_same = false;

    const float statusbarSize = 50;

    std::string filter_msg1 = "Image Files (";
    std::string filter_msg2 = "";
};

static const float WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER = 2.00f; // Lock scrolled window (so it doesn't pick child windows that are scrolling through) for a certain time, unless mouse moved.

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
                                    | ImGuiWindowFlags_NoSavedSettings;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ImGui::Begin("STATUSBAR", NULL, window_flags);
    ImGui::PopStyleVar();
    ImGui::Text("Status bar message.");
    ImGui::End();
    ImGui::PopStyleColor();
}

void MyApp::InitFileFilters()
{
    filter_msg1 = "Image Files (";
    filter_msg2 = "";
    std::vector<std::string> exts = imcmp::get_supported_image_file_exts();
    for (int i = 0; i < exts.size(); i++)
    {
        if (i > 0)
        {
            filter_msg1 += " ." + exts[i];
            filter_msg2 += " *." + exts[i];
        }
        else
        {
            filter_msg1 += "." + exts[i];
            filter_msg2 += "*." + exts[i];
        }
    }
    filter_msg1 += ")";
}

void MyApp::ShowImage(const char* windowName, bool* open, const RichImage& image, float align_to_right_ratio)
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
            //ImVec2 p_max = p_min + rendered_texture_size;
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

void MyApp::ComputeDiffImage()
{
    if ((!imageLeft.mat.empty() && !imageRight.mat.empty() && compare_condition_updated))
    {
        cv::Mat diff_mat;
        if (!imageLeft.mat.empty() && !imageRight.mat.empty())
        {
            diff_mat = compare_two_mat(imageLeft.mat, imageRight.mat, diff_thresh, is_exactly_same);
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

int MyApp::UI_ChooseImageFile()
{
    // Check that a backend is available
    if (!pfd::settings::available())
    {
        std::cout << "Portable File Dialogs are not available on this platform.\n";
        return 1;
    }

    // Set verbosity to true
    pfd::settings::verbose(true);

#if __APPLE__
    std::string default_image_directory = "/Users/zz/data";
#elif __linux__
    std::string default_image_directory = "/home/zz/data2";
#elif _MSC_VER
    std::string default_image_directory = "D:/data";
#endif

    // NOTE: file extension filter not working on macOSX
    auto f = pfd::open_file("Choose image file", 
                            //pfd::path::home(),
                            default_image_directory,
                            
                            //{"Image Files (.jpg .png .jpeg .bmp .nv21 .nv12 .rgb24 .bgr24)", "*.jpg *.png *.jpeg *.bmp *.nv21 *.nv12 *.rgb24 *.bgr24",
                            {filter_msg1, filter_msg2, "All Files", "*"}
                            //pfd::opt::multiselect
    );
    if (f.result().size() > 0)
    {
        std::cout << "Selected files:";
        std::cout << f.result()[0] << std::endl;

        filepath.setf("%s", f.result()[0].c_str());
    }
    return 0;
}

void MyApp::LoadImage(RichImage& image)
{
    UI_ChooseImageFile();
    if (filepath.c_str())
    {
        image.load_from_file(filepath);
    }
    filepath = NULL;
}

int main(int argc, char** argv)
{
    MyApp app;
    app.Run();

    return 0;
}
