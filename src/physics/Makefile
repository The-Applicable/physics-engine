CXX = emcc

CXXFLAGS = -O3 -std=c++17 -I. -s MODULARIZE=1 -s EXPORT_NAME='createPhysicsModule' -s ALLOW_MEMORY_GROWTH=1 -s ENVIRONMENT=web --bind

OUTPUT_DIR = ../../public/wasm
OUTPUT_FILE = $(OUTPUT_DIR)/physics.js

SOURCES = engine.cpp core/Vector3.cpp

all: $(OUTPUT_FILE)

$(OUTPUT_FILE): $(SOURCES)
		mkdir -p $(OUTPUT_DIR)
		$(CXX) $(SOURCES) -o $(OUTPUT_FILE) $(CXXFLAGS)
		@echo "The thing is built successfully: $(OUTPUT_DIR)"

clean:
		rm -f $(OUTPUT_FILE) $(OUTPUT_DIR)/physics.wasm