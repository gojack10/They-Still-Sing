# Builder stage
FROM ubuntu:23.10 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    ccache \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libgl1-mesa-dev \
    x11-xserver-utils \
    libudev-dev \
    libopenal-dev \
    libvorbis-dev \
    libflac-dev \
    libxinerama-dev \
    libfreetype6-dev \
    libxcb-image0-dev \
    libxcb-randr0-dev \
    libpulse-dev \
    libasound2-dev \
    && rm -rf /var/lib/apt/lists/*

# Install SFML 2.6 from source
RUN git clone https://github.com/SFML/SFML.git /usr/src/sfml && \
    cd /usr/src/sfml && \
    git checkout 2.6.x && \
    mkdir build && \
    cd build && \
    cmake -DBUILD_SHARED_LIBS=TRUE \
          -DSFML_BUILD_EXAMPLES=FALSE \
          -DSFML_BUILD_DOC=FALSE \
          -DSFML_BUILD_AUDIO=TRUE \
          -DSFML_BUILD_GRAPHICS=TRUE \
          .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Install nlohmann_json
RUN git clone https://github.com/nlohmann/json.git /usr/src/json && \
    cd /usr/src/json && \
    git checkout v3.11.2 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make install

# Copy source code and build the game
COPY . /app/
WORKDIR /app
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

# Runtime stage
FROM ubuntu:23.10

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libx11-6 \
    libxrandr2 \
    libxcursor1 \
    libgl1-mesa-glx \
    libudev1 \
    libopenal1 \
    libvorbis0a \
    libflac8 \
    libxinerama1 \
    libfreetype6 \
    libxcb-image0 \
    libxcb-randr0 \
    pulseaudio \
    alsa-utils \
    libasound2 \
    && rm -rf /var/lib/apt/lists/*

# Create input directory and dummy joystick device
RUN mkdir -p /dev/input && \
    mknod /dev/input/js0 c 13 0 && \
    chmod 666 /dev/input/js0

# Copy SFML and json libraries from builder
COPY --from=builder /usr/local/lib/libsfml-* /usr/local/lib/
COPY --from=builder /usr/local/lib/libjson* /usr/local/lib/

# Copy the built game
COPY --from=builder /app/build/TheyStillSing /app/TheyStillSing
COPY --from=builder /app/engine/assets /app/engine/assets

# Update library cache
RUN ldconfig

# Set working directory
WORKDIR /app

# Set environment variables for display and audio
ENV DISPLAY=${DISPLAY}
ENV PULSE_SERVER=unix:/run/pulse/native
ENV ALSA_CARD=default

# Create startup script with audio initialization
RUN echo '#!/bin/bash\n\
# Start PulseAudio if available\n\
if command -v pulseaudio >/dev/null 2>&1; then\n\
    pulseaudio --start || true\n\
fi\n\
\n\
# Initialize ALSA if available\n\
if command -v alsactl >/dev/null 2>&1; then\n\
    alsactl init || true\n\
fi\n\
\n\
# Run the game\n\
/app/TheyStillSing' > /app/start.sh && \
    chmod +x /app/start.sh

# Set entry point
ENTRYPOINT ["/app/start.sh"]