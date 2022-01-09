#!/bin/bash

# Run from b-tests directory

# Copy & backup files
cp test_B* ../test/
mkdir ../src/protocol/bckup
mv ../src/protocol/*Test* ../src/protocol/bckup
cp LCSF_* ../src/protocol/
cp Test_Main* ../src/protocol/

#Execute tests
cd ..
ceedling clean
ceedling test:pattern[test_B_*]
cd b-tests

# Reset files
rm ../test/test_B*
rm  ../src/protocol/*Test*
mv ../src/protocol/bckup/*Test* ../src/protocol/
rmdir ../src/protocol/bckup
cd ..
ceedling clean