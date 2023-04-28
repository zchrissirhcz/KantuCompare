#!/bin/bash

import git
from git import RemoteProgress
import os
import subprocess
import platform

def is_wsl():
    return 'microsoft-standard' in platform.uname().release

def is_windows():
    return platform.system().lower() == "windows"

def is_linux():
    return platform.system().lower() == "linux"


class CommandRunner(object):
    @staticmethod
    def run(cmd, verbose = True):
        if (verbose):
            print('Executing cmd:', cmd)
        process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
        if is_windows():
            encoding = 'ISO-8859-1'
        else:
            encoding = 'utf-8'
        output = process.communicate()[0].decode(encoding)
        if (verbose):
            print('Execution output:\n', output)
        return output


class CloneProgress(RemoteProgress):
    def update(self, op_code, cur_count, max_count=None, message=''):
        if message:
            print(message)

class SledPackage(object):
    def __init__(self, name):
        self.name = name
        self.src_dir = None

    def clone_repo(self, git_url, branch=None, tag=None, mirror_url=None, depth=1, save_dir='deps'):
        if (mirror_url is not None):
            git_url = mirror_url
        repo_name = git_url.split('/')[-1]
        to_path = '{:s}/{:s}'.format(save_dir, repo_name)
        self.src_dir = to_path
        self.build_dir = to_path + "/build"
        self.install_dir = to_path + "/install" # the default one. you may override this
        self.build_type = "Debug"

        print("[git clone] {:s}".format(git_url))
        print("  - to_path:", to_path)
        print("  - branch:", str(branch))
        print("  - tag:", str(tag))
        print("  - depth:", depth)
        to_path_dot_git = to_path + "/.git"
        if (os.path.exists(to_path_dot_git)):
            print("  Warning: directory {:s} already exist, skip git clone".format(to_path_dot_git))
            return
        if ((branch is None) and (tag is None)):
            git.Repo.clone_from(git_url, to_path = to_path, depth = depth, progress=CloneProgress())
        elif ((branch is not None) and (tag is not None)):
            print("Error: confusion. you can't specify both branch and tag.")
            return
        else:
            if (tag is not None):
                branch = tag
            git.Repo.clone_from(git_url, to_path = to_path, branch=branch, depth = depth, progress=CloneProgress())

    def cmake_configure(self, cmake_configure_args=[]):
        print("[configure] {:s}".format(self.name))
        found_install_prefix = False
        found_build_type = False
        for item in cmake_configure_args:
            if ('CMAKE_INSTALL_PREFIX' in item):
                found_install_prefix = True
                self.install_dir = item.split('=')[-1]
            if ('CMAKE_BUILD_TYPE' in item):
                found_build_type = True
                self.build_type = item.split('=')[-1]
        
        cmd = "cmake -S {:s} -B {:s}".format(self.src_dir, self.build_dir)
        for item in cmake_configure_args:
            cmd += " " + item
        if (not found_install_prefix):
            cmd += " -DCMAKE_INSTALL_PREFIX={:s}".format(self.install_dir)
        if is_windows():
            cmd += '-G "Visual Studio 17 2022" -A x64'

        print("  cmake configure command is: {:s}".format(cmd))
        CommandRunner.run(cmd)

    def cmake_build(self):
        print("[build] {:s}".format(self.name))
        # if (os.path.exists(self.build_dir)):
        #     print("  build dir {:s} already exist, skip build".format(self.build_dir))
        #     return
        cmd = "cmake --build {:s} -j4 --config {:s}".format(self.build_dir, self.build_type)
        CommandRunner.run(cmd)

    def cmake_install(self):
        print("[install] {:s}".format(self.name))
        if (os.path.exists(self.install_dir)):
            print("  install dir {:s} already exist, skip install".format(self.install_dir))
            return
        cmd = "cmake --install {:s} --config {:s}".format(self.build_dir, self.build_type)
        CommandRunner.run(cmd)


def prepare_imgui():
    pkg = SledPackage('imgui')
    #pkg.clone_repo('https://github.com/ocornut/imgui', 'docking', mirror_url='https://gitee.com/mirrors/imgui')
    pkg.clone_repo('https://github.com/zchrissirhcz/imgui', 'docking-for-image-compare')
    # pkg.cmake_configure()
    # pkg.cmake_build()
    # pkg.cmake_install()

def preprare_portable_file_dialogs():
    pkg = SledPackage('portable-file-dialogs')
    pkg.clone_repo('https://github.com/samhocevar/portable-file-dialogs')
    # pkg.cmake_configure()
    # pkg.cmake_build()
    # pkg.cmake_install()

def prepare_glfw():
    pkg = SledPackage('glfw')
    pkg.clone_repo('https://github.com/glfw/glfw', mirror_url='https://gitee.com/mirrors/glfw')
    # pkg.cmake_configure()
    # pkg.cmake_build()
    # pkg.cmake_install()

def prepare_gtest():
    pkg = SledPackage('gtest')
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
    pkg = SledPackage('opencv')
    pkg.clone_repo('https://github.com/opencv/opencv', tag='4.7.0', mirror_url='https://gitee.com/mirrors/opencv')
    pkg.cmake_configure(
        [
            "-D BUILD_SHARED_LIBS=OFF",
            "-D OPENCV_GENERATE_PKGCONFIG=ON",
            "-D BUILD_LIST=core,imgproc,highgui,gapi",
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
    pkg.cmake_build()
    pkg.cmake_install()

if __name__ == '__main__':
    prepare_imgui()
    preprare_portable_file_dialogs()
    prepare_glfw()
    prepare_gtest()
    prepare_opencv()