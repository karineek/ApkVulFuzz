sudo apt update
sudo apt install -y python3 python3-pip openjdk-17-jdk unzip wget
sudo apt install -y libxml2-utils
sudo apt install -y apksigner
python3 -m pip install --upgrade pip
python3 -m pip install --upgrade setuptools
python3 -m pip install --upgrade pip setuptools wheel

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

yes | sdkmanager --sdk_root="$ANDROID_SDK_ROOT" --licenses

sdkmanager --sdk_root="$ANDROID_SDK_ROOT" \
  "platform-tools" \
  "emulator" \
  "platforms;android-34" \
  "system-images;android-34;google_apis;x86_64"

sdkmanager "build-tools;34.0.0"

adb version
sdkmanager --list | head
avdmanager list target | head
apkanalyzer --help | head

avdmanager create avd -n test34 -k "system-images;android-34;google_apis;x86_64"

emulator -list-avds

emulator -avd test34 -no-window -no-audio -no-boot-anim -no-metrics -no-snapshot > /tmp/emulator.log 2>&1 &
adb wait-for-device
until [ "$(adb shell getprop sys.boot_completed | tr -d '\r')" = "1" ]; do sleep 2; done
adb devices -l

git clone https://github.com/honeynet/droidbot.git
cd droidbot

python3 -m pip uninstall -y androguard
python3 -m pip install "androguard>=3.4.0a1,<4"

python3 -m pip install -e .

echo ">> DONE."
