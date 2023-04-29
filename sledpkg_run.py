import sledpkg as sp
import shutil
import os

def prepare_imgui():
    pkg = sp.SledPackage('imgui')
    #pkg.clone_repo('https://github.com/ocornut/imgui', 'docking', mirror_url='https://gitee.com/mirrors/imgui')
    pkg.clone_repo('https://github.com/zchrissirhcz/imgui', 'docking-for-image-compare')
    # pkg.cmake_configure()
    # pkg.cmake_build()
    # pkg.cmake_install()

def preprare_portable_file_dialogs():
    pkg = sp.SledPackage('portable-file-dialogs')
    pkg.clone_repo('https://github.com/samhocevar/portable-file-dialogs', mirror_url='https://gitee.com/mirrors_samhocevar/portable-file-dialogs')
    # pkg.cmake_configure()
    # pkg.cmake_build()
    # pkg.cmake_install()

def prepare_glfw():
    pkg = sp.SledPackage('glfw')
    pkg.clone_repo('https://github.com/glfw/glfw', mirror_url='https://gitee.com/mirrors/glfw')
    # pkg.cmake_configure()
    # pkg.cmake_build()
    # pkg.cmake_install()

def prepare_str():
    pkg = sp.SledPackage('str')
    pkg.clone_repo('https://github.com/ocornut/str')

def prepare_gtest():
    pkg = sp.SledPackage('gtest')
    pkg.clone_repo('https://github.com/google/googletest', tag='release-1.11.0', mirror_url='https://gitee.com/mirrors/googletest')
    pkg.cmake_configure(
        [
            "-DCMAKE_BUILD_TYPE=Release",
            "-DBUILD_GMOCK=OFF",
            "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
        ]
    )
    pkg.cmake_build()
    pkg.cmake_install()

def prepare_opencv():
    pkg = sp.SledPackage('opencv')
    #pkg.clone_repo('https://github.com/opencv/opencv', tag='4.7.0', mirror_url='https://gitee.com/mirrors/opencv')
    pkg.clone_repo('https://github.com/opencv/opencv', mirror_url='https://gitee.com/mirrors/opencv')
    gapi_dir = pkg.src_dir + "/modules/gapi"
    if (os.path.exists(gapi_dir)):
        shutil.rmtree(gapi_dir)
    pkg.cmake_configure(
        [
            "-D BUILD_SHARED_LIBS=OFF",
            "-D BUILD_WITH_STATIC_CRT=OFF",
            "-D OPENCV_GENERATE_PKGCONFIG=ON",
            "-D BUILD_LIST=core,imgproc,imgcodecs,highgui",
            "-D BUILD_TESTS=OFF",
            "-D BUILD_PERF_TESTS=OFF",
            "-D WITH_CUDA=OFF",
            "-D WITH_VTK=OFF",
            "-D WITH_MATLAB=OFF",
            "-D BUILD_DOCS=OFF",
            "-D BUILD_opencv_python3=OFF",
            "-D BUILD_opencv_python2=OFF",
            "-D WITH_IPP=OFF",
            "-D WITH_PROTOBUF=OFF",
            "-D WITH_QUIRC=OFF",
            "-D WITH_EIGEN=OFF",
            "-D CV_DISABLE_OPTIMIZATION=OFF",
            "-D OPENCV_DOWNLOAD_MIRROR_ID=gitcode",
            "-D WITH_OPENCL=OFF",
        ]
    )
    if sp.is_windows():
        pkg.cmake_build('Debug')
        pkg.cmake_install('Debug')

        pkg.cmake_build('Release')
        pkg.cmake_install('Release')
    else:
        pkg.cmake_build()
        pkg.cmake_install()

if __name__ == '__main__':
    prepare_imgui()
    preprare_portable_file_dialogs()
    prepare_glfw()
    prepare_str()
    prepare_gtest()
    prepare_opencv()