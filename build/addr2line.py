#!/usr/bin/env python

import subprocess

NDK_DIR="/home/zz/soft/android-ndk-r21e"
NDK_ADDR2LINE="{:s}/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/bin/aarch64-linux-android-addr2line".format(NDK_DIR)

raw_lines = """
    #0 0x4abac8 in calloc (/home/zz/work/image-compare/build/linux-x64/ImageCompare+0x4abac8)
    #1 0x7f64ec68bbdf in _XimOpenIM (/usr/lib/x86_64-linux-gnu/libX11.so.6+0x65bdf)
""".strip().split("\n")

exe_file = 'linux-x64/ImageCompare'
exe_file_name = exe_file.split('/')[-1]
addr2line = 'addr2line'
#addr2line = NDK_ADDR2LINE


for raw_line in raw_lines:
    pos = raw_line.find("+0x")
    addr_str = raw_line[pos+1:-1]
    #$NDK_ADDR2LINE 0x5f5a559d90 -C -f -e ./android-arm64/testbed
    #print(remain)

    cmd = "{:s} {:s} -C -f -e {:s}".format(addr2line, addr_str, exe_file)
    #cmd = "$NDK_ADDR2LINE {:s} -C -f -e {:s}".format(addr_str, exe_file)

    process = subprocess.Popen(cmd,
                                shell=True,
                                stdout=subprocess.PIPE,
                            )
    output = process.communicate()[0].decode('utf-8').strip()
    outlines = output.split('\n')

    # ignore no address info
    if (outlines[0] == "??" or outlines[1] == "??:0"):
        continue

    # ignore LLVM compiler(STL) stuffs
    if ('llvm' in outlines[1]):
        continue
    # ignore google test stuffs
    if ('buildbot' in outlines[1]):
        continue

    #print("[command] \n" + cmd)
    print(cmd)
    print(output)
    print("")

    