# Image Compare

Simple cross-platform (macOS, Linux, Windows) image comparison GUI tool.

## Usage
Click "Load" buttons to load images. Once both two input images loaded, the diff image is computed and displayed.

- Change `Tolerance` slider to get different compare result.
- Change `Zoom` slider or use mouse wheel to scale current image.

![](images/snapshots/snapshot_2022-06-12.png)

Example input images and snapshots can be found in [images](https://github.com/zchrissirhcz/image-compare/tree/main/images) directory.

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
# edit CMakeLists.txt if you are using different paths of dependencies
```

**configure && build && run**
```bash
cd build

# for Linux / macOS
./linux-x64.sh
cd linux-x64
./ImageCompare
cd ..

# for Windows
.\vs2022-build.cmd
cd vs2022-build
.\Debug\ImageCompare
cd ..
```

## Remarks
The initial implementation was based on Qt, and is not updated anymore, for the sake of license and law (for example, on the company's computer, a lawyer's letter is received when the QtCreator development environment is installed).

You can switch to the [Qt branch](https://github.com/zchrissirhcz/small-compare/tree/qt) to view the source code.

## References
- https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
- https://github.com/aang7/Pix
- https://github.com/shangchiwu/advanced-image-processor
- https://www.youtube.com/watch?v=OYQp0GuoByM
- https://github.com/Smorodov/imgui_image_viewer
- https://github.com/CedricGuillemet/imgInspect