#!/bin/bash

for f in `ls $TEST_DIR/*.js`; do
    script_name=`basename $f`
    expect="$TEST_DIR/expect/$script_name.out"
    $TEST_DIR/run_test.py $f $expect
    if [[ "$?" != "0" ]]; then
        exit 1
    fi
done

