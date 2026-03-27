env \
  LD_LIBRARY_PATH=$HOME/adb-fuzz/instrumented_binaries/usr/lib/x86_64-linux-gnu/android \
  LD_PRELOAD=$HOME/adb-fuzz/instrumented_binaries/usr/lib/x86_64-linux-gnu/android/libadb.so.0 \
  AFL_SKIP_CPUFREQ=1 \
  AFL_SKIP_BIN_CHECK=1 \
  AFL_CUSTOM_MUTATOR_ONLY=1 \
  AFL_SHUFFLE_QUEUE=1 \
  AFL_CUSTOM_MUTATOR_LIBRARY=$HOME/ApkVulFuzz/build/cm-ApkVulFuzz.so \
  AFL_NO_AFFINITY=1 \
  $HOME/AFLplusplus/afl-fuzz -n -m none -t 99000 \
        -i input \
        -o output -- \
        python3 $HOME/droidbot/start.py -aa @@ -d emulator-5554 \
        -policy bfs_greedy -count 100 -interval 2 \
        -timeout 50 -o results/fdroid_run \
        -is_emulator -grant_perm

env \
  LD_LIBRARY_PATH=$HOME/adb-fuzz/instrumented_binaries/usr/lib/x86_64-linux-gnu/android \
  LD_PRELOAD=$HOME/adb-fuzz/instrumented_binaries/usr/lib/x86_64-linux-gnu/android/libadb.so.0 \
  AFL_PRELOAD=$HOME/adb-fuzz/instrumented_binaries/usr/lib/x86_64-linux-gnu/android/libadb.so.0 \
  AFL_SKIP_CPUFREQ=1 \
  AFL_SKIP_BIN_CHECK=1 \
  AFL_CUSTOM_MUTATOR_ONLY=1 \
  AFL_SHUFFLE_QUEUE=1 \
  AFL_CUSTOM_MUTATOR_LIBRARY=$HOME/ApkVulFuzz/build/cm-ApkVulFuzz.so \
  AFL_NO_AFFINITY=1 \
  $HOME/AFLplusplus/afl-fuzz -m none -t 99000 \
    -i input \
    -o output -- \
    python3 $HOME/droidbot/start.py -aa @@ -d emulator-5554 \
    -policy bfs_greedy -count 100 -interval 2 \
    -timeout 50 -o results/fdroid_run \
    -is_emulator -grant_perm

env \
  LD_LIBRARY_PATH=~/adb-fuzz/instrumented_binaries/usr/lib/x86_64-linux-gnu/android \
  AFL_SKIP_CPUFREQ=1 \
  AFL_CUSTOM_MUTATOR_ONLY=1 \
  AFL_SHUFFLE_QUEUE=1 \
  AFL_CUSTOM_MUTATOR_LIBRARY=$HOME/ApkVulFuzz/build/cm-ApkVulFuzz.so \
  AFL_NO_AFFINITY=1 \
  $HOME/AFLplusplus/afl-fuzz -m none -t 99000 \
    -i input \
    -o output -- \
    python3 $HOME/droidbot/start.py -aa @@ -d emulator-5554 \
    -policy bfs_greedy -count 100 -interval 2 \
    -timeout 50 -o results/fdroid_run \
    -is_emulator -grant_perm

## python3 /users/kevenmen/droidbot/start.py -aa /users/kevenmen/AFLplusplus/output/.cur_input -d emulator-5554 -policy bfs_greedy -count 100 -interval 2 -timeout 50 -o results/fdroid_run -is_emulator -grant_perm
## python3                          start.py -a ../F-Droid.apk                                 -d emulator-5554 -policy bfs_greedy -count 1000 -interval 2 -timeout 1800 -o results/fdroid_run -is_emulator -grant_perm

AFL_DEBUG=1 \
AFL_SKIP_BIN_CHECK=1 \
AFL_CUSTOM_MUTATOR_ONLY=1 \
AFL_SHUFFLE_QUEUE=1 \
AFL_SKIP_CPUFREQ=1 \
AFL_NO_AFFINITY=1 \
AFL_CUSTOM_MUTATOR_LIBRARY=$HOME/ApkVulFuzz/build/cm-ApkVulFuzz.so \
py-afl-fuzz -m none -t 99000 \
          -i $HOME/ApkVulFuzz/Evaluation-SSBSE-2026/input \
          -o output -- \
      python3 $HOME/droidbot/start.py -aa @@ -d emulator-5554 \
      -policy bfs_greedy -count 100 -interval 2 \
      -timeout 30 -o results/fdroid_run \
      -is_emulator -grant_perm

### py-afl-fuzz -i /fuzzer_input_corpus -o /OUTPUTS -t 4000 -- /harnesses/differential_harness.py


env \
  AFL_DEBUG=1 \
  AFL_SKIP_CPUFREQ=1 \
  AFL_SKIP_BIN_CHECK=1 \
  AFL_CUSTOM_MUTATOR_ONLY=1 \
  AFL_SHUFFLE_QUEUE=1 \
  AFL_CUSTOM_MUTATOR_LIBRARY="$HOME/ApkVulFuzz/build/cm-ApkVulFuzz.so" \
  py-afl-fuzz -m none -t 99000 \
    -i input \
    -o output -- \
    python3 "$HOME/droidbot/start.py" -aa @@ -d emulator-5554 \
      -policy bfs_greedy -count 100 -interval 2 \
      -timeout 30 -o results/fdroid_run \
      -is_emulator -grant_perm


env \
  AFL_SKIP_CPUFREQ=1 \
  AFL_SKIP_BIN_CHECK=1 \
  AFL_CUSTOM_MUTATOR_ONLY=1 \
  AFL_SHUFFLE_QUEUE=1 \
  AFL_CUSTOM_MUTATOR_LIBRARY="$HOME/ApkVulFuzz/build/cm-ApkVulFuzz.so" \
  py-afl-fuzz -m none -t 99000 \
    -i input \
    -o output -- \
    python3 "$HOME/droidbot/start.py" -aa @@ -d emulator-5554 \
      -policy bfs_greedy -count 100 -interval 2 \
      -timeout 30 -o results/fdroid_run \
      -is_emulator -grant_perm
