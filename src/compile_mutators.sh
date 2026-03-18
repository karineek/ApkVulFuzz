mkdir -p ../build
cd custom_mutators/
clang -I. -o standalone_mutator bitflip.c cm_ApkVulFuzz.c
cp *.o ../../build
