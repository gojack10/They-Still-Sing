FROM archlinux:latest

# Update system and install base dependencies
RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm \
    base-devel \
    cmake \
    git \
    sfml \
    gcc \
    gdb \
    zsh \
    vim \
    xorg-server-xvfb \
    mesa \
    libgl \
    && pacman -Scc --noconfirm

# Set up working directory
WORKDIR /app

# Copy the project files
COPY . .

# Create build directory and build the project
RUN cd engine && \
    mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

# Set up ZSH as default shell
ENV SHELL=/usr/bin/zsh
RUN chsh -s /usr/bin/zsh

# Set the working directory to the engine directory
WORKDIR /app/engine

# Command to run the game
CMD ["./build/TheyStillSing"] 