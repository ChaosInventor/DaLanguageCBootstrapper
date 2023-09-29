#!/bin/sh

BUILD_DIR=${BUILD_DIR:-build}

#Make sure detok test exists
if [ ! -x $BUILD_DIR/bin/Tests/UnparserTest ]; then
    echo 'UnparserTest not found, running `make`' >&2
    make $BUILD_DIR/bin/Tests/UnparserTest
    if [ ! -x $BUILD_DIR/bin/Tests/UnparserTest ]; then
        echo 'UnparserTest is still not executable, error' >&2
        exit -2
    fi
fi

failedFiles=0

#Run unparser test for each positional argument, diff the output with the original.
for file; do
    echo "Running test with file $file" >&2
    if ! ./$BUILD_DIR/bin/Tests/UnparserTest < $file | diff --text $file - 2> /dev/null; then
        echo Failed to test file $file >&2
        filedFiles=$((failedFiles+1))
    else
        echo Test succeeded for $file >&2
    fi
done

exit $failedFiles

