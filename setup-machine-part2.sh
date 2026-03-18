export ANDROID_SDK_ROOT="$HOME/android-sdk"
export ANDROID_HOME="$ANDROID_SDK_ROOT"
export PATH="$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$ANDROID_SDK_ROOT/cmdline-tools/bin:$ANDROID_SDK_ROOT/platform-tools:$ANDROID_SDK_ROOT/emulator:$PATH"

which adb
which emulator
which sdkmanager
which avdmanager

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

export QT_QPA_PLATFORM=offscreen
emulator -avd test34 -no-window -no-audio -no-boot-anim -no-metrics -no-snapshot > /tmp/emulator.log 2>&1 &

adb wait-for-device

echo ">> Waiting for device"

until [ "$(adb shell getprop sys.boot_completed | tr -d '\r')" = "1" ]; do
  sleep 2
done

echo ">> Installation of system completed"

adb devices -l

git clone https://github.com/honeynet/droidbot.git
cd droidbot

python3 -m pip uninstall -y androguard
python3 -m pip install "androguard>=3.4.0a1,<4"

python3 -m pip install -e .

adb -s emulator-5554 install droidbot/resources/droidbotApp.apk
adb -s emulator-5554 shell ime enable io.github.ylimit.droidbotapp/.DroidBotIME

echo ">> DONE."
