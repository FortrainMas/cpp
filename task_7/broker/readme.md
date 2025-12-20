<!-- mkdir build
cd build

conan install .. --output-folder=. --build=missing

cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build . --config Release -->



mkdir build
cd build
conan install .. --output-folder=. --build=missing
cd ..
cmake --preset conan-default
cmake --build --preset conan-release
