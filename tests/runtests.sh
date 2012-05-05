echo ""
echo "Unit Tests"

# Loop over compiled tests and run them.
for test_file in build/tests/*_tests
do
    # Only execute if result is a file.
    if test -f $test_file
    then
        # Log execution to file.
        if ./$test_file 2>&1 > /tmp/sky-test.log
        then
            rm -f /tmp/sky-test.log
        else
            # If error occurred then print off log.
            cat /tmp/sky-test.log
            exit 1
        fi
    fi
done

echo ""
