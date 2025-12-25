#!/bin/bash

# Build script for tassan project
# Usage: ./build.sh [mode]
# Modes: normal (default), directRead

MODE=${1:-normal}

# Set compiler flags based on mode
if [ "$MODE" = "directRead" ]; then
    echo "Building in directRead mode..."
    CFLAGS="-DDIRECT_READ_MODE"
else
    CFLAGS=""
fi

# Run make with the appropriate flags
make CFLAGS="$CFLAGS"

if [ $? -eq 0 ]; then
    echo "Build completed successfully in $MODE mode"
else
    echo "Build failed"
    exit 1
fi