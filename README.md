# ApkVulFuzz
Fuzzing APK as binaries to hit vulnerabilities


## Installing a Simulator of APK


## Blackbox fuzzing

Run:
```
./scripts/search-blackbox.sh > all.log 2>&1 
```

## Greybox fuzzing
TODO

## Test Corrupted APK

Run this command to test if the apk is behaving oddly:
```
user@nmscmo008175 droidbot % python3 start.py -a ../F-Droid.apk -d emulator-5554 -policy bfs_greedy -count 100 -interval 2 -timeout 20 -o results/fdroid_run -is_emulator -grant_perm
```
