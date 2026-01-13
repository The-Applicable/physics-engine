#!/bin/bash
mkdir -p ../../public/wasm

emcc engine.cpp -o ../../public/wasm/physics.js \
  -lembind \
  -s MODULARIZE=1 \
  -s EXPORT_NAME='createPhysicsModule' \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s ENVIRONMENT=web \
  -O3
  
echo "Compilation complete. Files generated in public/wasm/"