#!/usr/bin/env bash
set -e

sudo sed -i -e 's/^# deb-src/deb-src/g' -e 's/^#deb-src/deb-src/g' /etc/apt/sources.list
sudo apt-get update
sudo apt-get build-dep -y android-platform-system-core

rm -rf ~/adb-fuzz
mkdir ~/adb-fuzz && cd ~/adb-fuzz

apt-get source android-platform-system-core
cd android-platform-system-core-*/ 

# afl-clang-lto probably works, i think -lbase was a concurrency issue, so we just cant build parallel
export CC=afl-clang-fast
export CXX=afl-clang-fast++
export LD=afl-clang-fast

export DEB_BUILD_MAINT_OPTIONS="hardening=-all"
export DEB_BUILD_OPTIONS="nodwz nostrip"

dpkg-buildpackage -us -uc -b

cd ~/adb-fuzz
mkdir -p ./instrumented_binaries

dpkg-deb -x adb_*.deb ./instrumented_binaries/
dpkg-deb -x android-libadb_*.deb ./instrumented_binaries/
dpkg-deb -x android-libbase_*.deb ./instrumented_binaries/
dpkg-deb -x android-libcutils_*.deb ./instrumented_binaries/
dpkg-deb -x android-liblog_*.deb ./instrumented_binaries/

echo "Probably need to set the library path for this:"
echo '    export LD_LIBRARY_PATH=~/adb-fuzz/instrumented_binaries/usr/lib/x86_64-linux-gnu/android/'

