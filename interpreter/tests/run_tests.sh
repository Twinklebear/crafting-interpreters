#!/bin/bash

failed=0
for f in `ls $TEST_DIR/*.js`; do
    script_name=`basename $f`
    printf "Run test '$script_name': "
    expect="$TEST_DIR/expect/$script_name.expect"
    $TEST_DIR/run_test.py $f $expect
    if [[ "$?" != "0" ]]; then
        echo "Failed"
        failed=1
    else
        echo "Passed"
    fi
done

if [[ $failed -eq 1 ]]; then
    echo "Some tests did not pass"
    exit 1
fi
echo "All tests passed"

