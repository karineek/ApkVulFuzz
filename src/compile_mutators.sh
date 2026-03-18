mkdir -p ../build
cd custom_mutators/
clang -Wall -Wextra -g -fsanitize=address,undefined -I. -o standalone_mutator bitflip.c cm_ApkVulFuzz.c
cp standalone_mutator ../../build/
echo ">> Done."
ls -l ../../build/
