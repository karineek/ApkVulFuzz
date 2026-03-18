mkdir -p ../build
cd custom_mutators/ || exit 1

# Compile AFL helper
clang -c -fPIC -Wall -O3 \
  -I"$HOME/AFLplusplus/include" \
  "$HOME/AFLplusplus/src/afl-performance.c" \
  -o afl-performance.o

# Compile sources (SEPARATELY)
clang -c -fPIC -Wall -O3 \
  -I"$HOME/AFLplusplus/include" -I. \
  -D AFL_CM \
  bitflip.c -o bitflip.o

clang -c -fPIC -Wall -O3 \
  -I"$HOME/AFLplusplus/include" -I. \
  -D AFL_CM \
  cm_ApkVulFuzz.c -o cm_ApkVulFuzz.o

# Link into shared object
clang -shared -o cm-ApkVulFuzz.so \
  afl-performance.o bitflip.o cm_ApkVulFuzz.o

# Copy outputs
cp *.o ../../build
cp *.so ../../build
