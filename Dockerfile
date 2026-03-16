FROM ubuntu:22.04

# Keep apt quiet during install
ENV DEBIAN_FRONTEND=noninteractive

# 1. Install pre-requirements
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    openjdk-17-jdk \
    libxml2-utils \
    apksigner \
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

# Verify binaries
RUN which adb || true
RUN which emulator || true
RUN which sdkmanager
RUN which avdmanager

# Accept licenses and install Android packages (using x86_64 by default)
RUN yes | sdkmanager --licenses && \
    sdkmanager --sdk_root=$ANDROID_SDK_ROOT \
    "platform-tools" \
    "emulator" \
    "platforms;android-34" \
    "system-images;android-34;google_apis;x86_64" \
    "build-tools;34.0.0"

# Tool verification
RUN adb version
RUN sdkmanager --list | head
RUN avdmanager list target | head
RUN apkanalyzer --help | head

# 4. Create the AVD
RUN echo "no" | avdmanager create avd -n test34 -k "system-images;android-34;google_apis;x86_64"

# List AVDs
RUN emulator -list-avds
                
# 5. Install Droidbot
WORKDIR /opt
RUN git clone https://github.com/honeynet/droidbot.git && \
    cd droidbot && \
    python3 -m pip install "androguard>=3.4.0a1,<4" && \
    python3 -m pip install -e .

# 6. Get the fuzzer
WORKDIR /opt
RUN git clone https://github.com/karineek/ApkVulFuzz.git

WORKDIR /workspace

CMD ["bash"]
