#!/usr/bin/env bash

set -euo pipefail

# Resolve the project root relative to this script.
PROJECT_ROOT="$(
    cd "$(dirname "${BASH_SOURCE[0]}")/.." &&
    pwd
)"

BUILD_DIR="${PROJECT_ROOT}/build"
BUILD_TYPE="${BUILD_TYPE:-Debug}"

echo "Configuring mip..."
cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DMIP_BUILD_TESTS=ON \
    -DMIP_BUILD_EXAMPLES=ON

echo "Building mip..."
cmake --build "${BUILD_DIR}" --parallel

echo "Running tests..."
ctest --test-dir "${BUILD_DIR}" --output-on-failure

echo "Build and tests completed successfully."