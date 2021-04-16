#!/bin/env python3

import os

check_dir = "sflib"
tmp_file = "__check_tmp.cpp"
tmp_obj = "__check_tmp.o"

cmd = "include-what-you-use -std=c++2a -pipe -Wall -Werror -c -I%s %s -o %s" % (check_dir, tmp_file, tmp_obj)

def check_self_satisfaction(filename):
    print("check self satisfaction:", file, "...")
    with open(tmp_file, "w") as fp:
        fp.write('''#include "%s"\n#include "%s"\n''' % (filename, filename))
    os.system(cmd)

if __name__ == "__main__":
    print("cmd:", cmd)
    files = os.listdir(check_dir)
    for f in files:
        if not f.endswith('hpp'):
            continue
        file = os.path.join(check_dir, f)
        check_self_satisfaction(file)
    os.remove(tmp_file)
    os.remove(tmp_obj)

