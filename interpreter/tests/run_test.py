#!/usr/bin/env python3

import sys
import os
import subprocess

with open(sys.argv[2], "r") as f:
    expect_output = f.read()
    result = subprocess.run(["./interpreter", sys.argv[1]], stdout=subprocess.PIPE)
    output = result.stdout.decode("utf-8")
    if expect_output != output:
        sys.exit(1)

