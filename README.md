# Physics Engine

A React + Three.js application integrated with a C++ physics engine via WebAssembly.

## Prerequisites

- Node.js (v20 or later recommended)
- Docker (for building the physics engine) OR Emscripten SDK installed locally

## Installation

Install the dependencies:

```bash
npm install
```

## Building the Physics Engine

The project uses a C++ physics engine compiled to WebAssembly. You need to compile it before running the app.

### Using Docker (Recommended)

Run the following command from the project root:

```bash
docker run --rm -v $(pwd):/src emscripten/emsdk emcc src/physics/engine.cpp -o public/wasm/physics.js -lembind -s MODULARIZE=1 -s EXPORT_NAME='createPhysicsModule' -s ALLOW_MEMORY_GROWTH=1 -s ENVIRONMENT=web -O3
```

### Using Local Emscripten

If you have `emcc` in your path, you can run the build script directly:

```bash
cd src/physics
./build.sh
cd ../..
```

## Running the Application

Start the development server:

```bash
npm run dev
```

Open your browser at http://localhost:5173 (or the URL shown in the terminal).

## Building for Production

To create a production build:

```bash
npm run build
```

The output will be in the `dist` directory.
