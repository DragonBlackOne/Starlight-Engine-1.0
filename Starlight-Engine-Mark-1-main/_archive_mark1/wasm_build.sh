#!/bin/bash
# Starlight Engine - WASM Build Script
# Builds the Rust core for WebAssembly deployment.
#
# Prerequisites:
#   rustup target add wasm32-unknown-unknown
#   cargo install wasm-pack
#
# Usage:
#   ./wasm_build.sh

set -e
echo "Building Starlight Engine Core for WASM..."

cd "$(dirname "$0")"

# Build with WASM feature (no PyO3, no Rapier, no Audio)
cd crates/engine_core

cargo build \
    --target wasm32-unknown-unknown \
    --no-default-features \
    --features wasm \
    --release

echo ""
echo "Build complete!"
echo "Output: target/wasm32-unknown-unknown/release/backend.wasm"
echo ""
echo "For wasm-pack (generates JS bindings):"
echo "  wasm-pack build --target web --no-default-features --features wasm --release"
