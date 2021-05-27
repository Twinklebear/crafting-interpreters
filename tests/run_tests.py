#!/usr/bin/env python3

import sys
import os
import subprocess
import glob

ANSI_RED = "\033[91m"
ANSI_GREEN = "\033[92m"
ANSI_END = "\033[0m"

test_dir = os.getenv("TEST_DIR")
failed_tests = 0
ran_tests = 0
for test_input in glob.glob("{}/*.lox".format(test_dir)):
    script_name = os.path.basename(test_input)
    expect_out_file = "{}/expect/{}.expect".format(test_dir, script_name)
    print("Running test '{}':".format(script_name), end=" ")
    with open(expect_out_file, "r") as expect_file:
        ran_tests += 1
        expect_output = expect_file.read()
        result = subprocess.run(["./interpreter", test_input], stdout=subprocess.PIPE)
        output = result.stdout.decode("utf-8")
        if expect_output != output:
            failed_tests += 1
            print(ANSI_RED + "Failed" + ANSI_END)
        else:
            print(ANSI_GREEN + "Passed" + ANSI_END)

print("Ran {} tests".format(ran_tests))

if failed_tests != 0:
    print("Some tests did not pass")
    sys.exit(1)
else:
    print("All tests passed")

