# Image Compare

Simple cross-platform (macOS, Linux, Windows) image comparison GUI tool.

## Usage
Click "Load" buttons to load images. Once both two input images loaded, the diff image is computed and displayed.

- Change `Tolerance` slider to get different compare result.
- Change `Zoom` slider or use mouse wheel to scale current image.

![](snapshots/snapshot_2022-06-12.png)

## How to build
**Required dependencies**
- [Dear ImGui](https://github.com/ocornut/imgui) with [glfw](https://github.com/glfw/glfw) and OpenGL backends
- [OpenCV](https://github.com/opencv/opencv)
- [CMake](https://cmake.org)

**Get sources and configure dependencies**
```bash
# switch to working directory
cd ~/work/github

# get imgui latest source, docking branch
git clone https://github.com/ocornut/imgui -b docking imgui-docking
# or get it from mirror:
# https://gitee.com/mirrors/imgui

# get imgui latest source
git clone https://github.com/glfw/glfw
# or get it from mirror:
# git clone https://gitee.com/mirrors/glfw

# get opencv
sudo apt install libopencv-dev # ubuntu
brew install opencv # mac
# or, just build from source

# get this repo
git clone https://github.com/zchrissirhcz/small-compare

# open the project with VSCode
cd small-compare
code .
# edit CMakeLists.txt if you using different paths of dependencies
```

**configure && build && run**
```bash
cd build
./linux-x64.sh    # for Linux / macOS
#.\vs2022-x64.cmd # for Windows with Visual Studio 2022
./ImageCompare    # start the application
```

## Remarks
The initial implementation is based on Qt, and is not updated anymore, for the sake of license and law (for example, on the company's computer, a lawyer's letter is received when the QtCreator development environment is installed).

You can switch to the [Qt branch](https://github.com/zchrissirhcz/small-compare/tree/qt) to view the source code.

## References
- https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
- https://github.com/aang7/Pix
- https://github.com/shangchiwu/advanced-image-processor
- https://www.youtube.com/watch?v=OYQp0GuoByM
- https://github.com/Smorodov/imgui_image_viewer
