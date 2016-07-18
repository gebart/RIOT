#!/usr/bin/env python3

import codecs
import os
import sys

FILE_TYPE = "static const unsigned char"

def mkconstfs(root_path, mount_point, constfs_name):
    print("#include \"fs/constfs.h\"")
    print("")

    for dirname, subdir_list, file_list in os.walk(root_path):
        target_dirname = os.path.join("/", dirname[len(root_path):])
        for fname in file_list:
            local_fname = os.path.join(dirname, fname)
            target_fname = os.path.join(target_dirname, fname)
            print_file_data(local_fname, target_fname)

    print("\nstatic const constfs_file_t _files[] = {")

    for mangled_name, target_name, size in files:
        print("    {")
        print("    .path = \"%s\"," % target_name)
        print("    .data = %s," % mangled_name)
        print("    .size = %s" % size)
        print("    },")
    print("};")

    print("""
static const constfs_t _fs_data = {
    .files = _files,
    .nfiles = sizeof(_files) / sizeof(_files[0]),
};

vfs_mount_t %s = {
    .fs = &constfs_file_system,
    .mount_point = \"%s\",
    .private_data = (void*)&_fs_data,
};
    """ % (constfs_name, mount_point))

def mangle_name(fname):
    fname = fname.replace("/", "__")
    fname = fname.replace(".", "__")

    return fname

def print_file_data(local_fname, target_fname):
    mangled_name = mangle_name(target_fname)
    print(FILE_TYPE, mangled_name, "[] = {", end="")

    line_length = 8
    nread = 0
    with open(local_fname, 'rb') as f:
        byte = f.read(1)
        while byte:
            if nread == 0:
                print("\n    ", end="")
            elif nread % line_length == 0:
                print(",\n    ", end="")
            else:
                print(", ", end="")
            nread += 1
            print ("0x" + codecs.encode(byte, 'hex').decode('ascii'), end="")
            # Do stuff with byte.
            byte = f.read(1)

    print("\n};")

    files.append((mangled_name, target_fname, nread))

files = []

if __name__=="__main__":
    mountpoint = "/"
    constfs_name = "_constfs"

    if len(sys.argv) < 2:
        print("usage: mkconstfs.py <path> [mountpoint] [constfs_name]")
        exit(1)

    path = sys.argv[1]
    if len(sys.argv) > 2:
        mountpoint = sys.argv[2]

    if len(sys.argv) > 3:
        constfs_name = sys.argv[3]

    mkconstfs(path, mountpoint, constfs_name)
