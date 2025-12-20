mkdir build
cd build
conan install .. --output-folder=. --build=missing
cd ..
cmake --preset conan-default
cmake --build --preset conan-release
