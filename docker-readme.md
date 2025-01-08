# Docker Setup for They Still Sing

This document explains how to run the game using Docker, which provides a consistent development environment across different machines.

## Prerequisites

- Docker
- Docker Compose
- X11 server (for Linux users, this is already installed)
- For Windows users: An X server like VcXsrv or Xming

## Setup Instructions

### Linux Users

1. Allow X server connections:
```bash
xhost +local:docker
```

2. Build and run the container:
```bash
docker-compose up --build
```

### Windows Users

1. Install and start an X server (VcXsrv recommended)
   - During setup, enable "Native OpenGL" and "Disable access control"
   - Set DISPLAY environment variable: `set DISPLAY=host.docker.internal:0.0`

2. Build and run the container:
```bash
docker-compose up --build
```

### macOS Users

1. Install and start XQuartz
2. Allow connections from network clients in XQuartz preferences
3. Run in terminal:
```bash
xhost +localhost
```

4. Build and run the container:
```bash
docker-compose up --build
```

## Development Workflow

- The project directory is mounted as a volume, so any changes you make to the source code will be reflected in the container
- To rebuild the project after code changes:
```bash
docker-compose exec game sh -c "cd /app/engine/build && make -j$(nproc)"
```

## Troubleshooting

1. If you see "Cannot open display" error:
   - Make sure X11 forwarding is properly set up
   - Check that the DISPLAY environment variable is correct
   - Verify that the X server is running and accepting connections

2. If graphics are slow or not working:
   - Ensure your graphics drivers are up to date
   - Try running with software rendering: Add `LIBGL_ALWAYS_SOFTWARE=1` to the environment section in docker-compose.yml

3. For permission issues:
   - Run `xhost +local:docker` on the host machine
   - Make sure you have proper permissions for /dev/dri 