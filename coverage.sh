# rmdir build-cov
mkdir build-cov

set -e
cd build-cov
cmake ..
make
cd tests
for file in test*
do
  "./$file"
done
cd ../..
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info # filter system-files
genhtml coverage.info -o coverage-report
rm coverage.info
open coverage-report/index.html
