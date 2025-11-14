# ----------------------------------------
# Build parameters (can be overridden from CLI)
# ----------------------------------------
BUILD_DIR ?= build
IMPLEMENTATION ?= VISA
BUILD_TYPE ?= Debug   # Debug or Release

.PHONY: build clean init_venv generate_compile_commands help conan

# ----------------------------------------
# Python virtual environment (optional)
# ----------------------------------------
init_venv:
	/usr/bin/python3 -m venv .venv
	. .venv/bin/activate && pip install --upgrade pip && pip install -r requirements.txt

# ----------------------------------------
# Conan install dependencies
# ----------------------------------------
conan:
	@echo "Installing Conan dependencies for build type $(BUILD_TYPE)..."
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && conan install .. --build=missing -s build_type=$(BUILD_TYPE)

# ----------------------------------------
# CMake Build (Debug or Release) with backend selection
# ----------------------------------------
build: conan
	@echo "Building InstrumentControlLib with backend: $(IMPLEMENTATION) in $(BUILD_TYPE) mode"
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && \
	cmake -GNinja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DIMPLEMENTATION=$(IMPLEMENTATION) .. && \
	cmake --build . -- -j$(shell nproc)

# ----------------------------------------
# Clean build directory
# ----------------------------------------
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

# ----------------------------------------
# CLI help / usage hints
# ----------------------------------------
help:
	@echo "Usage:"
	@echo "  make build [IMPLEMENTATION=VISA/MOCK/OTHER] [BUILD_TYPE=Debug/Release]"
	@echo "      Build the library with selected backend and build type"
	@echo "  make clean"
	@echo "      Remove build directory"
	@echo "  make init_venv"
	@echo "      Create Python virtual environment and install requirements"
	@echo ""
	@echo "Defaults:"
	@echo "  IMPLEMENTATION=$(IMPLEMENTATION)"
	@echo "  BUILD_TYPE=$(BUILD_TYPE)"
