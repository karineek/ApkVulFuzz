sudo apt update
sudo apt install -y python3 python3-pip openjdk-17-jdk unzip wget
sudo apt install -y libxml2-utils
sudo apt install -y apksigner adb
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
EOF

source ~/.bashrc
## End of optional part

sudo groupadd -r kvm
sudo gpasswd -a $USER kvm
sudo usermod -aG kvm $USER
groups
echo ">> Restart the session. Part 1 is ended."
