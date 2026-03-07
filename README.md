# ApkVulFuzz
Fuzzing APK as binaries to hit vulnerabilities


## Installing a Simulator of APK

**1.** Install pre-requirements
```
sudo apt update
sudo apt install -y python3 python3-pip openjdk-17-jdk unzip wget
python3 -m pip install --upgrade pip
python3 -m pip install --upgrade setuptools
python3 -m pip install --upgrade pip setuptools wheel
```
  
**2.** Setup Android SDK root environment
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
export PATH="$ANDROID_SDK_ROOT/emulator:$ANDROID_SDK_ROOT/platform-tools:$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$PATH"

## Optional if you want to install it for good
cat >> ~/.bashrc <<'EOF'
export ANDROID_SDK_ROOT="$HOME/android-sdk"
export ANDROID_HOME="$ANDROID_SDK_ROOT"
export PATH="$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$ANDROID_SDK_ROOT/platform-tools:$ANDROID_SDK_ROOT/emulator:$PATH"
export PATH="$ANDROID_SDK_ROOT/emulator:$ANDROID_SDK_ROOT/platform-tools:$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$PATH"
EOF
source ~/.bashrc
## End of optional part

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

and then
```
sdkmanager "build-tools;34.0.0"
```

**3.** Check installation:

```
adb version
sdkmanager --list | head
avdmanager list target | head
apkanalyzer --help | head
```

Which then returns something like this:
```
Android Debug Bridge version 1.0.41
Version 37.0.0-14910828
Installed as /users/kevenmen/android-sdk/platform-tools/adb
Running on Linux 5.15.0-168-generic (x86_64)
[=======================================] 100% Computing updates...             
Installed packages:
  Path                                        | Version | Description                                | Location                                   
  -------                                     | ------- | -------                                    | -------                                    
  build-tools;34.0.0                          | 34.0.0  | Android SDK Build-Tools 34                 | build-tools/34.0.0                         
  emulator                                    | 36.4.9  | Android Emulator                           | emulator                                   
  platform-tools                              | 37.0.0  | Android SDK Platform-Tools                 | platform-tools                             
  platforms;android-34                        | 3       | Android SDK Platform 34                    | platforms/android-34                       
  system-images;android-34;google_apis;x86_64 | 14      | Google APIs Intel x86_64 Atom System Image | system-images/android-34/google_apis/x86_64

[=======================================] 100% Fetch remote repository...       
Available Android targets:
----------
id: 1 or "android-34"
     Name: Android API 34, extension level 7
     Type: Platform
     API level: 34
     Revision: 3
Verb must be one of: 

Usage:
apkanalyzer [global options] <subject> <verb> [options] <apk> [<apk2>]

Option            Description                         
------            -----------                         
--human-readable  Print sizes in human readable format
```

**4.** Install the emulator + start:

Run this:
```
avdmanager create avd -n test34 -k "system-images;android-34;google_apis;x86_64"
```

Check if it is created:
```
emulator -list-avds
```

And start the emulator:
```
emulator -avd test34 -no-window -no-audio -no-boot-anim -no-metrics -no-snapshot > /tmp/emulator.log 2>&1 &
adb wait-for-device
until [ "$(adb shell getprop sys.boot_completed | tr -d '\r')" = "1" ]; do sleep 2; done
adb devices -l
```

**5.** Install the simulator:

```
git clone https://github.com/honeynet/droidbot.git
cd droidbot
pip install -e .
```
You can read further on how to use this emulator: [https://github.com/honeynet/droidbot]   

## Blackbox fuzzing

Run:
```
./scripts/search-blackbox.sh > all.log 2>&1 
```

## Greybox fuzzing
TODO

## Test Corrupted APK

First, try this command:
```
apkanalyzer apk summary F-Droid.apk 
```

Run this command to test if the apk is behaving oddly:
```
user@nmscmo008175 droidbot % python3 start.py -a ../F-Droid.apk -d emulator-5554 -policy bfs_greedy -count 100 -interval 2 -timeout 20 -o results/fdroid_run -is_emulator -grant_perm
```
