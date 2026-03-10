FROM ubuntu:22.04

# Keep apt quiet during install
ENV DEBIAN_FRONTEND=noninteractive

# 1. Install pre-requirements
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    openjdk-17-jdk \
    unzip \
    wget \
    git \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m pip install --upgrade pip setuptools wheel

# 2. Setup Android SDK root environment
ENV ANDROID_SDK_ROOT=/opt/android-sdk
ENV ANDROID_HOME=$ANDROID_SDK_ROOT
ENV PATH=$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$ANDROID_SDK_ROOT/platform-tools:$ANDROID_SDK_ROOT/emulator:$PATH

# Download and install cmdline-tools
RUN mkdir -p /tmp/cmdline-tools-unpacked && \
    wget -q https://dl.google.com/android/repository/commandlinetools-linux-13114758_latest.zip -O /tmp/cmdline-tools.zip && \
    unzip -q /tmp/cmdline-tools.zip -d /tmp/cmdline-tools-unpacked && \
    mkdir -p $ANDROID_SDK_ROOT/cmdline-tools/latest && \
    mv /tmp/cmdline-tools-unpacked/cmdline-tools/* $ANDROID_SDK_ROOT/cmdline-tools/latest/ && \
    rm -rf /tmp/cmdline-tools*

# Accept licenses and install Android packages (using x86_64 by default)
RUN yes | sdkmanager --licenses && \
    sdkmanager --sdk_root=$ANDROID_SDK_ROOT \
    "platform-tools" \
    "emulator" \
    "platforms;android-34" \
    "system-images;android-34;google_apis;x86_64" \
    "build-tools;34.0.0"

# 4. Create the AVD
RUN echo "no" | avdmanager create avd -n test34 -k "system-images;android-34;google_apis;x86_64"

# 5. Install Droidbot
WORKDIR /opt
RUN git clone https://github.com/honeynet/droidbot.git && \
    cd droidbot && \
    python3 -m pip install "androguard>=3.4.0a1,<4" && \
    python3 -m pip install -e .

# Set up your working directory
WORKDIR /app

# Copy your local project files (scripts, src, etc.) into the container
COPY . /app

# Create a startup wrapper script
RUN cat <<'EOF' > /app/start.sh
#!/bin/bash
echo "Starting Android emulator in the background..."
emulator -avd test34 -no-window -no-audio -no-boot-anim -no-metrics -no-snapshot -gpu swiftshader_indirect > /tmp/emulator.log 2>&1 &

echo "Waiting for device to boot..."
adb wait-for-device
until [ "$(adb shell getprop sys.boot_completed | tr -d '\r')" = "1" ]; do sleep 2; done

echo "Emulator is up! Starting the black box script..."
./scripts/search-blackbox.sh > all.log 2>&1

echo "Done. Dumping the last 50 lines of all.log:"
tail -n 50 all.log
EOF

RUN chmod +x /app/start.sh

# By default, run the startup script
CMD ["/app/start.sh"]
