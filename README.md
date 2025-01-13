# They Still Sing

A Five Nights at Freddy's fan game built with SFML and modern C++, featuring dynamic audio, responsive UI, and cross-platform support.

## Project Overview

They Still Sing is a horror game that puts players in a tense office environment where they must manage power systems, monitor security cameras, and defend against animatronic threats. The game features:

- Dynamic 3D audio system with positional sound
- Responsive UI with resolution-independent scaling
- State-based game management
- Advanced animation system
- Comprehensive options menu
- Cross-platform support (Linux, macOS, Windows)

## Development Setup

### Prerequisites

- CMake 3.10 or higher
- SFML 2.5 or higher
- nlohmann_json 3.11.2
- C++17 compatible compiler

### Docker Setup (Recommended)

The easiest way to build and run the game is using Docker.

#### Prerequisites for Docker Setup

- Docker installed and running
- Platform-specific requirements:
  - **Linux**: X11 server
  - **macOS**: XQuartz and socat (`brew install --cask xquartz && brew install socat`)
  - **Windows**: WSL2 with X server

#### Building and Running with Docker

1. Clone the repository

2. Run the game using the provided script:
   ```bash
   ./run-docker.sh
   ```

The script will automatically:
- Detect your operating system
- Set up the necessary display forwarding
- Build the Docker image
- Run the game in a container

#### Managing Docker Resources

1. List running containers:
   ```bash
   docker ps
   ```

2. Stop a running container:
   ```bash
   docker stop $(docker ps -q --filter ancestor=they-still-sing)
   ```

3. Remove the container and image:
   ```bash
   # Remove container
   docker rm $(docker ps -a -q --filter ancestor=they-still-sing)
   
   # Remove image
   docker rmi they-still-sing
   ```

4. Clean up all unused Docker resources:
   ```bash
   docker system prune -a
   ```

### Manual Setup (Alternative)

If you prefer not to use Docker, you can build the game directly:

1. Install dependencies:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake libsfml-dev libjsoncpp-dev

   # Arch Linux
   sudo pacman -S base-devel cmake sfml jsoncpp

   # macOS
   brew install cmake sfml nlohmann-json
   ```

2. Build the project:
   ```bash
   cd engine
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```

3. Run the game:
   ```bash
   ./TheyStillSing
   ```

## Development

The project uses a modular architecture with the following key components:

- `engine/src/states/`: Game state management
- `engine/src/systems/`: Core game systems (audio, animation, etc.)
- `engine/src/ui/`: UI components and scaling system
- `engine/assets/`: Game assets (textures, sounds, configurations)