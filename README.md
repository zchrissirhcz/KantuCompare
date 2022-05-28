# Image Compare

Simple cross-platform (macOS, Linux, Windows) image comparison GUI tool.

## Usage
Click "Load Image" button for image loading and displaying.
Then do it again, to load another image.
Then click "Compare" button to display the diff image.

Currently it resize the two input image to 256x256, then do the absdiff.

## Dependencies
- [Dear ImGui](https://github.com/ocornut/imgui) with [glfw](https://github.com/glfw/glfw) and OpenGL backends
- [OpenCV](https://github.com/opencv/opencv)
- [CMake](https://cmake.org)

## References
- https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
- https://github.com/aang7/Pix