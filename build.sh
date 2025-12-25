#!/usr/bin/env bash
# Run makefiles in specified subdirectories under src.

set -euo pipefail

if [ "$#" -eq 0 ]; then
    echo "Usage: $0 <target_dir> [<target_dir> ...]" >&2
    exit 1
fi

for dir in "$@"; do
    target="src/$dir"
    if [ ! -d "$target" ]; then
        echo "Skip: $target (not a directory)" >&2
        continue
    fi
    if [ ! -f "$target/Makefile" ] && [ ! -f "$target/makefile" ]; then
        echo "Skip: $target (no Makefile found)" >&2
        continue
    fi
    echo "Building in $target..."
    make -C "$target"
done