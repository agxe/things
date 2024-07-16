#!/usr/bin/env python3

# calcmismatch.py

"""
Description: Find the mismatch difference between reference and target file.
Author: Arun G
Date created: 2023/08/01
Version: 0.1.0
Python version: 3.11.0
"""

import sys

def usage():
    print("\nUsage:")
    print("  calcmismatch.py reference_file.txt target_file.txt width height")

def check_pyver():
    if sys.version_info[0] != 3:
        print('This script needs Python 3 or above version.')
        sys.exit(1)

def read_file(file_name):
    content = ""

    try:
        with open(file_name, "r") as fp:
            content = fp.readlines()
    except FileNotFoundError as e:
        print(f"{e}")
        sys.exit(1)

    return content

def find_difference(ref_buf, tar_buf, width, height):
    max_rv = 0.0
    max_tv = 0.0
    max_diff = 0.0
    total_mismatch_frac = 0.0

    max_idx = 0
    zero_cnt = 0

    for idx, (rc, tc) in enumerate(zip(ref_buf, tar_buf)):
        rc = rc.strip()
        tc = tc.strip()

        rv = float(rc)
        tv = float(tc)

        diff = abs(rv - tv)
        if diff > max_diff:
            max_diff = diff
            max_rv = rv
            max_tv = tv
            max_idx = idx + 1

        if rv == 0.0:
            zero_cnt += 1
        else:
            total_mismatch_frac += (diff / abs(rv));

    if max_diff > 0:
        y = max_idx // width;
        x = max_idx % width

        avg_mismatch_pct = (total_mismatch_frac / (width * height - 
                                                   zero_cnt)) * 100.0;

        print(f"Ref val = {max_rv:0.6f}, Tar val = {max_tv:0.6f}, "
              f"Max diff = {max_diff:0.6f}, X = {x:d}, Y = {y:d}, "
              f"Avg Mismatch = {avg_mismatch_pct:0.10f}%")
    else:
        print("No difference in files.")

def main(ref_file, tar_file, width, height):
    check_pyver()

    print(f"\nComparing {ref_file:s} vs {tar_file:s}...")

    ref_content = read_file(ref_file)
    tar_content = read_file(tar_file)

    num_elem = width * height
    if (num_elem != len(ref_content) or num_elem != len(tar_content)):
        print("size is different between reference and target file.")
        return

    find_difference(ref_content, tar_content, width, height)

if __name__ == '__main__':
    if 5 != len(sys.argv):
        usage()
        sys.exit(1)

    main(sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4]))
    sys.exit(0)
