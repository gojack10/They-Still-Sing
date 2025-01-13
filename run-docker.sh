#!/bin/bash

# Function to detect OS
detect_os() {
    case "$(uname -s)" in
        Linux*)     echo "linux";;
        Darwin*)    echo "mac";;
        MINGW*|MSYS*|CYGWIN*)     echo "windows";;
        *)          echo "unknown";;
    esac
}

# Function to ensure xhost is installed on Linux
ensure_xhost() {
    if ! command -v xhost &> /dev/null; then
        echo "xhost is not installed. Installing x11-xserver-utils..."
        if command -v apt-get &> /dev/null; then
            sudo apt-get update && sudo apt-get install -y x11-xserver-utils
        elif command -v pacman &> /dev/null; then
            sudo pacman -Sy --noconfirm xorg-xhost
        else
            echo "Could not install xhost. Please install x11-xserver-utils manually."
            exit 1
        fi
    fi
}

# Function to setup X11 permissions
setup_x11() {
    if command -v xhost &> /dev/null; then
        xhost +local:docker
    else
        echo "Warning: xhost not available. X11 forwarding might not work properly."
        XAUTH=/tmp/.docker.xauth
        touch $XAUTH
        xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -
        chmod 777 $XAUTH
    fi
}

# Function to cleanup X11 permissions
cleanup_x11() {
    if command -v xhost &> /dev/null; then
        xhost -local:docker
    fi
}

# Function to check macOS dependencies
check_mac_deps() {
    local missing_deps=0

    if ! command -v socat &> /dev/null; then
        echo "socat is required. Please install it with: brew install socat"
        missing_deps=1
    fi

    if ! command -v xquartz &> /dev/null; then
        echo "XQuartz is required. Please install it with: brew install --cask xquartz"
        missing_deps=1
    fi

    if ! command -v sndcpy &> /dev/null; then
        echo "sndcpy is required. Please install it with: brew install sndcpy"
        missing_deps=1
    fi

    if [ $missing_deps -eq 1 ]; then
        exit 1
    fi
}

# Function to setup audio for Linux
setup_linux_audio() {
    # Create PulseAudio socket directory if it doesn't exist
    mkdir -p /tmp/pulse-socket
    
    # Get user's PulseAudio socket
    PULSE_SOCKET=$(pactl info | grep "Server String" | cut -d' ' -f3)
    
    if [ -z "$PULSE_SOCKET" ]; then
        # Fallback to default socket location
        PULSE_SOCKET="/run/user/$(id -u)/pulse/native"
    fi
    
    # Create symlink to the socket
    ln -sf "$PULSE_SOCKET" /tmp/pulse-socket/native
}

# Build the Docker image
echo "Building Docker image..."
docker build -t they-still-sing .

OS=$(detect_os)
case $OS in
    "linux")
        ensure_xhost
        setup_x11
        setup_linux_audio
        echo "Running on Linux..."
        docker run -it --rm \
            -e DISPLAY=$DISPLAY \
            -v /tmp/.X11-unix:/tmp/.X11-unix \
            -v $(pwd):/app/source \
            -v /tmp/.docker.xauth:/tmp/.docker.xauth:rw \
            -e XAUTHORITY=/tmp/.docker.xauth \
            --device /dev/dri:/dev/dri \
            --device /dev/snd:/dev/snd \
            -v /tmp/pulse-socket:/run/pulse \
            -e PULSE_SERVER=unix:/run/pulse/native \
            --group-add $(getent group audio | cut -d: -f3) \
            --network host \
            they-still-sing
        cleanup_x11
        ;;
    "mac")
        check_mac_deps

        echo "Starting XQuartz..."
        open -a XQuartz
        sleep 2

        echo "Setting up audio forwarding..."
        mkdir -p /tmp/pulse-socket
        sndcpy --daemon &
        SNDCPY_PID=$!

        echo "Setting up X11 forwarding..."
        socat TCP-LISTEN:6000,reuseaddr,fork UNIX-CLIENT:"$DISPLAY" &
        SOCAT_PID=$!

        IP=$(ifconfig en0 | grep inet | awk '$1=="inet" {print $2}')
        xhost + $IP

        echo "Running on macOS..."
        docker run -it --rm \
            -e DISPLAY=$IP:0 \
            -v $(pwd):/app/source \
            -e PULSE_SERVER=unix:/run/pulse/native \
            -v /tmp/pulse-socket:/run/pulse \
            they-still-sing

        # Cleanup
        kill $SOCAT_PID
        kill $SNDCPY_PID
        rm -rf /tmp/pulse-socket
        ;;
    "windows")
        if ! command -v vcxsrv &> /dev/null; then
            echo "VcXsrv is required. Please install it from: https://sourceforge.net/projects/vcxsrv/"
            echo "After installation, make sure to run XLaunch and configure it with:"
            echo "- Multiple windows"
            echo "- Display number: 0"
            echo "- Start no client"
            echo "- Disable access control"
            exit 1
        fi

        echo "Checking for running VcXsrv..."
        if ! tasklist | grep -q "vcxsrv"; then
            echo "Please start VcXsrv first using XLaunch"
            exit 1
        fi

        # For Windows, we'll use the host's audio system through WSL
        echo "Running on Windows..."
        DISPLAY=$(ipconfig | grep -A 4 "Ethernet adapter vEthernet (WSL)" | grep "IPv4 Address" | cut -d: -f2 | tr -d ' \r\n'):0
        docker run -it --rm \
            -e DISPLAY=$DISPLAY \
            -v $(pwd):/app/source \
            -e PULSE_SERVER=tcp:localhost:4713 \
            --network host \
            they-still-sing
        ;;
    *)
        echo "Unsupported operating system"
        exit 1
        ;;
esac