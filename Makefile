# ----------------------------------------
# Build parameters (can be overridden from CLI)
# ----------------------------------------
BUILD_DIR ?= build
IMPLEMENTATION ?= VISA            # VISA | VISAClient | ESP32 | RP2040
BUILD_TYPE ?= Release             # Debug or Release
BUILD_EXAMPLES ?= OFF             # ON to build example app(s)
GENERATOR ?= Ninja                # CMake generator
TOOLCHAIN ?=                      # Optional: path to CMake toolchain file (MCU)
CMAKE_ARGS ?=                     # Extra CMake -D flags, e.g. -DPICO_SDK_PATH=...
SRCS := $(shell find examples lib -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) -not -path '*/build/*' -not -path '*/third_party/*')

.PHONY: build clean init_venv help conan pre_build build-visa build-visaclient build-esp32 build-rp2040 run-example

# Decide if we need Conan dependencies by default
# PC impls use Conan (VISA, VISAClient); MCU impls default to NO
ifeq ($(filter $(IMPLEMENTATION),VISA VISAClient),$(IMPLEMENTATION))
	USE_CONAN ?= ON
else
	USE_CONAN ?= OFF
endif

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

# Optional pre-build step depending on USE_CONAN
ifeq ($(USE_CONAN),ON)
pre_build: conan
else
pre_build:
	@echo "Skipping Conan (USE_CONAN=$(USE_CONAN))"
endif

# ----------------------------------------
# CMake Build (Debug or Release) with backend selection
# ----------------------------------------
build: pre_build
	@echo "Building InstrumentControlLib (examples=$(BUILD_EXAMPLES)) with backend: $(IMPLEMENTATION) in $(BUILD_TYPE) mode"
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && \
	cmake -G$(GENERATOR) \
	  -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	  -DEXAMPLE_BACKEND=$(IMPLEMENTATION) \
	  -DINSTRUMENTCONTROL_BUILD_VISA=$(if $(filter VISA,$(IMPLEMENTATION)),ON,OFF) \
	  -DINSTRUMENTCONTROL_BUILD_VISACLIENT=$(if $(filter VISAClient,$(IMPLEMENTATION)),ON,OFF) \
	  -DINSTRUMENTCONTROL_BUILD_ESP32=$(if $(filter ESP32,$(IMPLEMENTATION)),ON,OFF) \
	  -DINSTRUMENTCONTROL_BUILD_RP2040=$(if $(filter RP2040,$(IMPLEMENTATION)),ON,OFF) \
	  -DBUILD_EXAMPLES=$(BUILD_EXAMPLES) \
	  $(if $(TOOLCHAIN),-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN),) \
	  $(CMAKE_ARGS) \
	  .. && \
	cmake --build . -- -j$(shell nproc)

# Convenience targets per implementation
build-visa:
	$(MAKE) build IMPLEMENTATION=VISA USE_CONAN=ON BUILD_EXAMPLES=ON

build-visaclient:
	$(MAKE) build IMPLEMENTATION=VISAClient USE_CONAN=ON BUILD_EXAMPLES=ON

build-esp32:
	$(MAKE) build IMPLEMENTATION=ESP32 USE_CONAN=OFF BUILD_EXAMPLES=OFF

build-rp2040:
	$(MAKE) build IMPLEMENTATION=RP2040 USE_CONAN=OFF BUILD_EXAMPLES=OFF

# Run example (only meaningful for PC impls)
run-example:
	./$(BUILD_DIR)/examples/VISA/InstrumentControlVISA

run-example-visaclient:
	./$(BUILD_DIR)/examples/VISAClient/InstrumentControlVISAClient

# ----------------------------------------
# Clean build directory
# ----------------------------------------
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

# ----------------------------------------
# Format code in-place with clang-format
# ----------------------------------------
format:
	@echo "Formatting C++ files in place..."
	@clang-format -style=file -i $(SRCS)
	@echo "clang-format complete."

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
	@run-clang-tidy -p $(BUILD_DIR) -quiet
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
	@echo "  make build [IMPLEMENTATION=VISA|VISAClient|ESP32|RP2040] [BUILD_TYPE=Debug/Release] [BUILD_EXAMPLES=ON/OFF] [USE_CONAN=ON/OFF] [GENERATOR=Ninja|Unix\ Makefiles] [TOOLCHAIN=/path/to/toolchain.cmake] [CMAKE_ARGS='-D...']"
	@echo "      Build the library with selected backend and build type"
	@echo "  make build-visa | build-visaclient | build-esp32 | build-rp2040"
	@echo "      Convenience aliases for common builds"
	@echo "  make clean"
	@echo "      Remove build directory"
	@echo "  make init_venv"
	@echo "      Create Python virtual environment and install requirements"
	@echo "  make run-example"
	@echo "      Run the example application (PC impls only)"
	@echo ""
	@echo "Defaults:"
	@echo "  IMPLEMENTATION=$(IMPLEMENTATION)"
	@echo "  BUILD_TYPE=$(BUILD_TYPE)"
	@echo "  BUILD_EXAMPLES=$(BUILD_EXAMPLES)"
	@echo "  USE_CONAN=$(USE_CONAN)"
	@echo "  GENERATOR=$(GENERATOR)"
