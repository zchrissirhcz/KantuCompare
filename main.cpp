#include <opencv2/core/hal/interface.h>
#ifdef __MACH__
#define GL_SILENCE_DEPRECATION
#endif

#include <stdio.h>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <opencv2/opencv.hpp>
#include "tinyfiledialogs.h"
#include "imageutils.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static const char* decide_gl_glsl_versions()
{
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    return glsl_version;
}

const char* filepath = NULL;

void* load_image(void *)
{
    char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
    const char* lTheOpenFileName = tinyfd_openFileDialog(
                        "let us read the password back",
                        "",
                        2,
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
        return NULL;
        
    }
    else {
        std::cout << "file choosed: " << lTheOpenFileName << std::endl;
        filepath = lTheOpenFileName;
    }
    
    return NULL;
}


static void showImage(const char* windowName, bool *open, const RichImage& image)
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

static void ShowImageWindow(bool* p_open = NULL)
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
            load_image(NULL);
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
            showImage("Diff Image", &show_diff_image, diff_image);
        }

        for(int i = 0; i < data.size(); i++)
        {
            ImGui::PushID(i);
            showImage(data[i].get_name(), data[i].get_open(), data[i]);
            ImGui::PopID();
        }
    }
    ImGui::End();
}


int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = decide_gl_glsl_versions();

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    //std::string font_path = "/System/Library/Fonts/PingFang.ttc"; // system wide
    std::string font_path = "/Users/zz/Library/Fonts/SourceHanSansCN-Normal.otf"; // user installed
    ImFont* font = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    io.Fonts->Build();
    IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = false;
    bool show_image_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        if (show_image_window)
            ShowImageWindow(&show_image_window);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
