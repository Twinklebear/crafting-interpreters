#!/bin/bash

for f in `ls $TEST_DIR/*.js`; do
    script_name=`basename $f`
    echo "Run test '$script_name'"
    expect="$TEST_DIR/expect/$script_name.expect"
    $TEST_DIR/run_test.py $f $expect
    if [[ "$?" != "0" ]]; then
        echo "Failed"
        exit 1
    fi
    echo "Passed"
done

