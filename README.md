# They Still Sing

## ⚠️ Content Warning

This game contains:
- Flashing lights and strobing effects
- Jump scares and sudden loud noises
- Disturbing imagery / themes

## About

"They Still Sing" is a FNAF fan horror game set in 1986 within a Fazbear Entertainment storage facility. As a night security guard, you must survive your shift while watching over decommissioned animatronics awaiting scrapping. But in the darkness of the facility, something stirs within their decaying shells - echoes of a tragedy that refuses to be forgotten.

## Current Status

🚧 In Development

## Technical Details

### Requirements
- Docker (for cross-platform development)
- SFML 2.5 or later
- CMake 3.10 or later

### Building with Docker

1. Build the Docker image:
```bash
cd engine
docker build -t they-still-sing .
```

2. Run the container:
```bash
docker run -v $(pwd):/app they-still-sing
```

The builds will be available in:
- `build-linux/game` (Linux executable)
- `build-windows/game.exe` (Windows executable)

### Building Locally on Linux

1. Install dependencies:
```bash
sudo pacman -S cmake sfml
```

2. Build:
```bash
cd engine
mkdir build
cd build
cmake ..
make
```

### Project Structure
```
├── engine/          # Game engine and source code
├── frames/          # Pre-rendered frame sequences
│   └── desk-idle/   # Office desk idle animation
```

### Frame Sequences
The game uses pre-rendered frame sequences stored in the `frames` directory. Each sequence should be placed in its own subdirectory with sequentially numbered frames (e.g., 0001.jpg, 0002.jpg, etc.).

## Acknowledgments

- Original FNAF series created by Scott Cawthon