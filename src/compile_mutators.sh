mkdir -p ../build
cd custom_mutators/
clang -I. -o standalone_mutator.o bitflip.c cm_ApkVulFuzz.c
cp *.o ../../build
