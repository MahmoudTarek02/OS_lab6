#!/bin/bash

# Check for Makefile
if [ ! -f Makefile ] && [ ! -f makefile ]; then
    echo "Test failed: no makefile to build your C++ file"
    exit 2
fi

# Build the project
if ! make; then
    echo
    echo "Test failed: your makefile failed to build"
    exit 3
fi

# make sure binary directory and name are correct
if [ ! -f ./lab6 ]; then
    echo "Test failed: Your makefile should create an executeable named lab6 in the same directory"
    exit 4
fi

# Check if testcases directory exists
if [ ! -d "./testcases" ]; then
    echo "This script assumes all testcases are inside a folder named testcases"
    echo "But the folder was not found"
    exit 5
fi

failures=0
successes=0
skipped=0
test_failed=0

# Use process substitution to avoid subshell
while IFS= read -r in_file; do
    out_file="${in_file/-input.txt/-output.txt}"
    echo "Testing input file: $in_file for the output from $out_file"

    # check if output file is missing
    if [ ! -f "$out_file" ]; then
        echo "Error: $out_file was not found"
        ((skipped++))
        continue
    fi

    if ! ./lab6 < "$in_file" | diff --color=always --minimal "$out_file" -; then
        echo "Test failed: there is a difference in output"
        ((test_failed=1))
        ((failures++))
    else
        echo "Test was successful"
        ((successes++))
    fi

    echo
done < <(find ./testcases/ -maxdepth 1 -type f -regex '.*-input\.txt' | sort)

echo
echo "All tests finished"
echo "$successes successes, $failures failures"

if [ $skipped -ne 0 ]; then
    echo "$skipped output file(s) are missing"
fi

if (( successes + failures + skipped < 36 )); then
    remaining=$(( 36 - successes - failures - skipped ))
    echo "$remaining input file(s) are missing"
fi

exit "$test_failed"
