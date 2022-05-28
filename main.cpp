#include <string>
#include <opencv2/opencv.hpp>
#include "tinyfiledialogs.h"
#include "imageutils.h"

#include "app_design.hpp"

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
        // Load Fonts
        //std::string font_path = "/System/Library/Fonts/PingFang.ttc"; // system wide
        std::string font_path = "/Users/zz/Library/Fonts/SourceHanSansCN-Normal.otf"; // user installed
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
        io.Fonts->Build();
        IM_ASSERT(font != NULL);
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
        ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(INFINITY, INFINITY));

        if (ImGui::Begin(windowName, open))
        {
            ImVec2 p_min = ImGui::GetCursorScreenPos(); // actual position
            ImVec2 p_max = ImVec2(ImGui::GetContentRegionAvail().x + p_min.x, ImGui::GetContentRegionAvail().y  + p_min.y);
            ImGui::GetWindowDrawList()->AddImage((void*)(uintptr_t)texture, p_min, p_max);
        }
        ImGui::End();
    }
}

void MyApp::ShowImageWindow(bool* p_open)
{
    static std::vector<RichImage> data;
    static bool window2 = false;
    static int  selectedItem = -1;
    static int actualitem = -1;
    static bool show_diff_image = false;

    ImGui::ShowMetricsWindow(&window2);
    ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
    if (ImGui::Begin("name", p_open))
    {
        // Load Image
        if (ImGui::Button("Load Image"))
        {
            LoadImage();
            if (filepath != NULL)
            {
                data.push_back(RichImage());
                cv::Mat mat = cv::imread(filepath); 
                data.back().load_mat(mat);
                data[data.size() - 1].set_name(filepath);
                filepath = NULL;
            }
        }
        for(int i = 0; i < data.size(); i++)
        {
            ImGui::PushID(i);
            
            if (ImGui::Selectable(data[i].get_name(), data[i].get_open()))
            {
                selectedItem = i;
            }
            
            ImGui::PopID();
            ImGui::Text("value: %s", *(data[i].get_open()) ? "true" : "false");
        }
        
        // Display test image
        static RichImage diff_image;
        static int diff_thresh = 1;
        ImGui::SliderInt("diff thresh", &diff_thresh, 0, 255);

        if (ImGui::Button("Compare"))
        {
            // TODO: get diff image with respect to size equal or not, and use diff_thresh
            cv::Mat diff_mat(255, 255, CV_8UC3);
            if (data.size()==2)
            {
                cv::Mat& mat1 = data[0].mat;
                cv::Mat& mat2 = data[1].mat;
                
                cv::Size dsize(256, 256);

                cv::Mat tmp1;
                cv::resize(mat1, tmp1, dsize);
                
                cv::Mat tmp2;
                cv::resize(mat2, tmp2, dsize);

                cv::absdiff(tmp1, tmp2, diff_mat);
            }
            else
            {
                diff_mat = cv::Scalar(0, 0, 0);
            }

            if (diff_image.mat.empty())
            {
                diff_image.clear(); // free texture memory
            }
            diff_image.load_mat(diff_mat);
            bool show_compare_image = true;
            show_diff_image = true;
        }

        if (show_diff_image)
        {
            ShowImage("Diff Image", &show_diff_image, diff_image);
        }

        for(int i = 0; i < data.size(); i++)
        {
            ImGui::PushID(i);
            ShowImage(data[i].get_name(), data[i].get_open(), data[i]);
            ImGui::PopID();
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
