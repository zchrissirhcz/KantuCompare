# KantuCompare

<img alt="GitHub" src="https://img.shields.io/github/license/zchrissirhcz/KantuCompare"> [![linux-x64](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/linux-x64.yml/badge.svg)](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/linux-x64.yml) [![windows-x64](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/windows-x64.yml/badge.svg)](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/windows-x64.yml) [![mac-x64](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/mac-x64.yml/badge.svg)](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/mac-x64.yml)

A GUI for image difference visualization.

![](images/snapshots/snapshot_2022-06-12.png)

## Features
- Support various image formats: encoded and Fourcc images (`[prefix_]WIDTHxHEIGHT.ext`):
    - `.jpg`, `.png`, `.bmp`, `.jpeg`
    - `test_1280x720.NV21`, `test_1280x720.NV12`, `hello_7680x4320.nv21`, `yes_640x480.i420`
    - `lena_1280x720.rgb24`, `lena_1280x720.bgr24`, `lena_1280x720.gray`
- Automatically compare and display difference image, just like Beyond Compare

## Usage
Click "Load" buttons to load images. Once both two input images loaded, the diff image is computed and displayed.

- Change `Tolerance` slider to get different compare result.
- Change `Zoom` slider or use mouse wheel to scale images.

See [images](https://github.com/zchrissirhcz/KantuCompare/tree/main/images) directory for testing images.

## Installation

### Download Prebuilt

There are prebuilt executable packages in the [Releases page](https://github.com/zchrissirhcz/KantuCompare/releases), support Windows-x64, Linux-x64, MacOSX-x64.

### Build from source
```bash
# clone repo
https://github.com/zchrissirhcz/KantuCompare
cd KantuCompare

# clone, build and install dependencies by provided script
pip install -r requirements.txt
python sledpkg_run.py
# alternatively, go to cmake/deps.cmake and modify it

# build the project itself
cd build
./vs2022-x64.cmd # for Windows
./linux-x64.sh   # for Linux/MacOSX

# start the app
cd linux-x64
./KantuCompareApp
```

## Remarks
The [initial trial](https://github.com/zchrissirhcz/KantuCompare/releases/tag/v0) was based on Qt.

## References
- https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
- https://github.com/aang7/Pix
- https://github.com/shangchiwu/advanced-image-processor
- https://www.youtube.com/watch?v=OYQp0GuoByM
- https://github.com/Smorodov/imgui_image_viewer