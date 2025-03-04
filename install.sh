rm -rf build
find . -name '*.cpp' -o -name '*.h' | xargs clang-format -i
conan install . --output-folder=build --build=missing
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=build/instal
cmake --build build
cmake --install build