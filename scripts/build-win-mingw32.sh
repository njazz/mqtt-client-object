#run from repository root

mkdir builds
cd builds
mkdir win-mingw
cd win-mingw
cmake ../.. -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.mingw.cmake
make

