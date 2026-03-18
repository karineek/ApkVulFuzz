## AFL_NO_AFFINITY=1 AFL_SHUFFLE_QUEUE=1 AFL_CUSTOM_MUTATOR_ONLY=1 AFL_DUMB_FORKSRV=1 AFL_FAST_CAL=1 AFL_IGNORE_PROBLEMS=1 AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 \
## AFL_MAP_SIZE=1200000 AFL_CUSTOM_MUTATOR_LIBRARY="$cm_path/cm-gem5.so" \
## $afl -m 50000 -t 99000 -i $input -o $output -- /home/ubuntu/gem5-ssbse-challenge-2023/build/X86/gem5.opt \
## /home/ubuntu/ASEGem5/hello-custom-binary-Ex.py --isa X86 --input @@
## python3 start.py -a ../F-Droid.apk -d emulator-5554 -policy bfs_greedy -count 1000 -interval 2 -timeout 1800 -o results/fdroid_run -is_emulator -grant_perm

AFL_SKIP_CPUFREQ=1 AFL_NO_AFFINITY=1 AFL_SHUFFLE_QUEUE=1 AFL_CUSTOM_MUTATOR_ONLY=1 AFL_DUMB_FORKSRV=1 AFL_FAST_CAL=1 AFL_IGNORE_PROBLEMS=1 AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 \
AFL_MAP_SIZE=1200000 \
AFL_CUSTOM_MUTATOR_LIBRARY=$HOME/ApkVulFuzz/build/cm-ApkVulFuzz.so \
$HOME/AFLplusplus/afl-fuzz -m 50000 -t 99000 \
-i $HOME/ApkVulFuzz/Evaluation-SSBSE-2026/corpus/ -o output -- \
python3 $HOME/droidbot/start.py -a @@ -d emulator-5554 \
-policy bfs_greedy -count 100 -interval 2 \
-timeout 50 -o results/fdroid_run \
-is_emulator -grant_perm
