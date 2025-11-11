from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain


class InstrumentControlConan(ConanFile):
    name = "InstrumentControl"
    version = "0.1"

    settings = "os", "compiler", "build_type", "arch"

    requires = ["spdlog/1.12.0"]

    generators = ("CMakeToolchain", "CMakeDeps")

    def layout(self):
        self.folders.build = "build"
        self.folders.generators = "build"
