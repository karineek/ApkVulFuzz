sudo apt update
sudo apt install -y python3 python3-pip openjdk-17-jdk unzip wget
sudo apt install -y libxml2-utils
sudo apt install -y apksigner adb
python3 -m pip install --upgrade pip
python3 -m pip install --upgrade setuptools
python3 -m pip install --upgrade pip setuptools wheel networkx Pillow
python3 -m pip install --user "androguard==3.4.0a1"

## Fuzzing-related requirements:
sudo rm /usr/bin/cpp /usr/bin/gcc /usr/bin/g++  /usr/bin/gcov  /usr/bin/c++
sudo rm /usr/local/bin/cpp /usr/local/bin/gcc /usr/local/bin/g++ /usr/local/bin/gcov  /usr/local/bin/c++ /usr/bin/cc
sudo ln -s /usr/bin/cpp-11 /usr/bin/cpp
sudo ln -s /usr/bin/gcc-11 /usr/bin/gcc
sudo ln -s /usr/bin/gcc-11 /usr/bin/cc
sudo ln -s /usr/bin/g++-11 /usr/bin/g++
sudo ln -s /usr/bin/g++-11 /usr/bin/c++
sudo ln -s /usr/bin/gcov-11 /usr/bin/gcov

wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 13
sudo ln -s /usr/bin/llvm-config-13 /usr/bin/llvm-config
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-13 130 --slave /usr/bin/clang++ clang++ /usr/bin/clang++-13

cd ..
git clone https://github.com/AFLplusplus/AFLplusplus.git
cd AFLplusplus
RUN git checkout f596a297c4de6a5e1a6fb9fbb3b4e18124a24f58
cp ../ApkVulFuzz/src/afl-files/afl-fuzz-init.c src/afl-fuzz-init.c
make all
sudo make install

## APK related pacakges:
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
