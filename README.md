# KantuCompare

<img alt="GitHub" src="https://img.shields.io/github/license/zchrissirhcz/KantuCompare"> [![linux-x64](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/linux-x64.yml/badge.svg)](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/linux-x64.yml) [![windows-x64](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/windows-x64.yml/badge.svg)](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/windows-x64.yml) [![mac-x64](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/mac-x64.yml/badge.svg)](https://github.com/zchrissirhcz/KantuCompare/actions/workflows/mac-x64.yml)

A GUI for image difference visualization.

![](images/snapshots/snapshot_2022-06-12.png)

## Usage
Click "Load" buttons to load images. Once both two input images loaded, the diff image is computed and displayed.

- Supported image extensions:
    - `.jpg`, `.jpeg`, `.bmp`, `.png`
    - `.nv21`, `.nv12`, `.i420`, `.gray`, `.rgb24`, `.bgr24`, `.rgba32`, `.bgra32`
- Change `Tolerance` slider to get different compare result.
- Change `Zoom` slider or use mouse wheel to scale images.

See [images](https://github.com/zchrissirhcz/KantuCompare/tree/main/images) directory for testing images.

## Build
```bash
# clone repo
https://github.com/zchrissirhcz/KantuCompare
cd KantuCompare

# clone, build and install dependencies
# you may use the provided python script for dependencies preparation
pip install -r requirements.txt
python sledpkg_run.py
# alternatively, go to cmake/deps.cmake and modify it

# build KantuCompare
cd build
./vs2022-x64.cmd # for Windows
./linux-x64.sh   # for Linux/MacOSX

# start the app
cd linux-x64
./KantuCompareApp
```

## Remarks
The [initial trial](https://github.com/zchrissirhcz/KantuCompare/tree/qt) was based on Qt.

## References
- https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
- https://github.com/aang7/Pix
- https://github.com/shangchiwu/advanced-image-processor
- https://www.youtube.com/watch?v=OYQp0GuoByM
- https://github.com/Smorodov/imgui_image_viewer