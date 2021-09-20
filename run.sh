set -e
clang++ -g -O3 main.cpp `llvm-config --cxxflags --ldflags --system-libs --libs all` -o test
./test > test.ll
clang++ test.ll
./a.out
