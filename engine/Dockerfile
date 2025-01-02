FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsfml-dev \
    mingw-w64 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source files
COPY . .

# Build for Linux
RUN mkdir -p build-linux && cd build-linux && \
    cmake .. && \
    make

# Build for Windows
RUN mkdir -p build-windows && cd build-windows && \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../windows-toolchain.cmake && \
    make 