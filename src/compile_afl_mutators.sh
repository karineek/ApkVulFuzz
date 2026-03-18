mkdir -p ../build
cd custom_mutators/
clang -c -fpic -Wall -O3 -I$HOME/AFLplusplus/include/ $HOME/AFLplusplus/src/afl-performance.c -o afl-performance.o
clang -c -fpic -Wall -O3 -I$HOME/AFLplusplus/include/ -I . bitflip.c cm_ApkVulFuzz.c -o mutators_afl.o
clang -shared -o cm-ApkVulFuzz.so afl-performance.o mutators_afl.o
cp *.o ../../build
cp *.so ../../build
