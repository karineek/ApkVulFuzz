# ApkVulFuzz
Fuzzing APK as binaries to hit vulnerabilities


## Installing a Simulator of APK

1. Install pre-requirements
```
sudo apt update
sudo apt install -y python3 python3-pip openjdk-17-jdk unzip wget
```
  
2.  Setup Android SDK root environment
```

mkdir -p "$HOME/android-sdk/cmdline-tools"
export ANDROID_SDK_ROOT="$HOME/android-sdk"
export ANDROID_HOME="$ANDROID_SDK_ROOT"

cd /tmp
wget https://dl.google.com/android/repository/commandlinetools-linux-13114758_latest.zip -O cmdline-tools.zip
unzip -q cmdline-tools.zip -d /tmp/cmdline-tools-unpacked

mkdir -p "$ANDROID_SDK_ROOT/cmdline-tools/latest"
mv /tmp/cmdline-tools-unpacked/cmdline-tools/* "$ANDROID_SDK_ROOT/cmdline-tools/latest/"

export PATH="$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$ANDROID_SDK_ROOT/platform-tools:$ANDROID_SDK_ROOT/emulator:$PATH"

cat >> ~/.bashrc <<'EOF'
export ANDROID_SDK_ROOT="$HOME/android-sdk"
export ANDROID_HOME="$ANDROID_SDK_ROOT"
export PATH="$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$ANDROID_SDK_ROOT/platform-tools:$ANDROID_SDK_ROOT/emulator:$PATH"
EOF
source ~/.bashrc

sdkmanager --sdk_root="$ANDROID_SDK_ROOT" --licenses
```

The install SDK for X86:
```
sdkmanager --sdk_root="$ANDROID_SDK_ROOT" \
  "platform-tools" \
  "emulator" \
  "platforms;android-34" \
  "system-images;android-34;google_apis;x86_64"
```
Or for ARM:

```
sdkmanager --sdk_root="$ANDROID_SDK_ROOT" \
  "platform-tools" \
  "emulator" \
  "platforms;android-34" \
  "system-images;android-34;google_apis;arm64-v8a"
```


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
