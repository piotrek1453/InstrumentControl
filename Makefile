# ----------------------------------------
# Build parameters (can be overridden from CLI)
# ----------------------------------------
BUILD_DIR ?= build
IMPLEMENTATION ?= VISA
BUILD_TYPE ?= Debug   # Debug or Release
BUILD_EXAMPLES ?= OFF # ON to build example app(s)
SRCS := $(shell find examples lib -name '*.cpp' -o -name '*.hpp' -o -name '*.h')

.PHONY: build clean init_venv help conan

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
	@echo "Building InstrumentControlLib (examples=$(BUILD_EXAMPLES)) with backend: $(IMPLEMENTATION) in $(BUILD_TYPE) mode"
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && \
	cmake -GNinja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DIMPLEMENTATION=$(IMPLEMENTATION) -DBUILD_EXAMPLES=$(BUILD_EXAMPLES) .. && \
	cmake --build . -- -j$(shell nproc)

# ----------------------------------------
# Clean build directory
# ----------------------------------------
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

# ----------------------------------------
# Setup pre-commit hooks
# ----------------------------------------
setup_pre_commit:
	pre-commit install

# ----------------------------------------
# Check code formatting with clang-format
# ----------------------------------------
format-check:
	@echo "Checking C++ formatting..."
	@clang-format -style=file --dry-run --Werror $(SRCS)
	@echo "clang-format passed."

# ----------------------------------------
# Check code style with clang-tidy
# ----------------------------------------
tidy-check:
	@echo "Running clang-tidy..."
	@run-clang-tidy -p build -quiet
	@echo "clang-tidy passed."

# ----------------------------------------
# Full source code check
# ----------------------------------------
check: format-check tidy-check
	@echo "All checks passed!"

# ----------------------------------------
# CLI help / usage hints
# ----------------------------------------
help:
	@echo "Usage:"
	@echo "  make build [IMPLEMENTATION=VISA/MOCK/OTHER] [BUILD_TYPE=Debug/Release] [BUILD_EXAMPLES=ON/OFF]"
	@echo "      Build the library with selected backend and build type"
	@echo "  make clean"
	@echo "      Remove build directory"
	@echo "  make init_venv"
	@echo "      Create Python virtual environment and install requirements"
	@echo ""
	@echo "Defaults:"
	@echo "  IMPLEMENTATION=$(IMPLEMENTATION)"
	@echo "  BUILD_TYPE=$(BUILD_TYPE)"
	@echo "  BUILD_EXAMPLES=$(BUILD_EXAMPLES)"
