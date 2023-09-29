#!/bin/sh

BUILD_DIR=${BUILD_DIR:-build}

#Make sure detok test exists
if [ ! -x $BUILD_DIR/bin/Tests/DetokenizerTest ]; then
    echo 'DetokenizerTest not found, running `make`' >&2
    make $BUILD_DIR/bin/Tests/DetokenizerTest
    if [ ! -x $BUILD_DIR/bin/Tests/DetokenizerTest ]; then
        echo 'DetokenizerTest is still not executable, error' >&2
        exit -1
    fi
fi

failedFiles=0

#Run detok test for each positional argument, diff the output with the original.
for file; do
    echo "Running test with file $file" >&2
    if ! ./$BUILD_DIR/bin/Tests/DetokenizerTest < $file | diff --text $file - 2> /dev/null; then
        echo Test failed for $file >&2
        filedFiles=$((failedFiles+1))
    else
        echo Test succeeded for $file >&2
    fi
done

exit $failedFiles

