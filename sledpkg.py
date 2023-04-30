# sledpkg:  Semi-precise package manager
# Author:   Zhuo Zhang <imzhuo#foxmail.com>
# Created:  2023.04.28 00:00:00
# Modified: 2023.04.30 16:15:00

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


def ninja_available():
    out = subprocess.getstatusoutput('ninja --version')
    if (out[0] == 0):
        return True
    return False


class SledPackage(object):
    def __init__(self, name):
        self.name = name
        self.src_dir = None

    def clone_repo(self, git_url, branch=None, tag=None, commit_id=None, mirror_url=None, shallow=True, save_dir='deps'):
        if (mirror_url is not None):
            git_url = mirror_url
        repo_name = git_url.split('/')[-1]
        to_path = '{:s}/{:s}'.format(save_dir, repo_name)
        self.src_dir = to_path
        self.build_dir = to_path + "/build"
        self.install_dir = to_path + "/install" # the default one. you may override this
        self.build_type = "Release"

        if (commit_id is not None):
            shallow = False

        kwargs = dict()

        print("[git clone] {:s}".format(git_url))
        print("  - to_path:", to_path)
        print("  - branch:", str(branch))
        print("  - tag:", str(tag))
        print("  - commit_id:", commit_id)
        if (shallow):
            print("  - shallow: True (depth=1)")
            kwargs['depth'] = 1
        else:
            print("  - shallow: False (full clone)")

        if ((branch is not None) and (tag is not None)):
            print("  Error: confusion. you can't specify both branch and tag.")
            return

        to_path_dot_git = to_path + "/.git"
        if (os.path.exists(to_path_dot_git)):
            print("  Warning: directory {:s} already exist, skip git clone".format(to_path_dot_git))
            return
        
        if (branch is not None):
            kwargs['branch'] = branch
        elif (tag is not None):
            kwargs['branch'] = tag
        
        repo = git.Repo.clone_from(git_url, to_path = to_path, progress=CloneProgress(), **kwargs)

        if (commit_id is not None):
            commit = repo.commit(commit_id)
            repo.head.reference = commit
            repo.head.reset(index=True, working_tree=True)

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
        if (not found_build_type):
            cmd += " -DCMAKE_BUILD_TYPE={:s}".format(self.build_type)
        if is_windows():
            cmd += ' -G "Visual Studio 17 2022" -A x64'
        elif ninja_available():
            cmd += ' -G Ninja'

        print("  cmake configure command is: {:s}".format(cmd))
        CommandRunner.run(cmd)

    def cmake_build(self, build_type = None):
        print("[build] {:s}".format(self.name))
        cmd = "cmake --build {:s} -j{:d}".format(self.build_dir, os.cpu_count())
        if (build_type is not None):
            cmd += " --config {:s}".format(build_type)
        print("  cmake install cmd: ", cmd)
        # if (os.path.exists(self.build_dir)):
        #     print("  build dir {:s} already exist, skip build".format(self.build_dir))
        #     return
        CommandRunner.run(cmd)

    def cmake_install(self, build_type = None):
        print("[install] {:s}".format(self.name))
        cmd = "cmake --install {:s}".format(self.build_dir)
        if (build_type is not None):
            cmd += " --config {:s}".format(build_type)
        print("  cmake install cmd: ", cmd)
        # if (os.path.exists(self.install_dir)):
        #     print("  install dir {:s} already exist, skip install".format(self.install_dir))
        #     return
        CommandRunner.run(cmd)
