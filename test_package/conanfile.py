import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import can_run


class json2cppTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("valijson/1.0")
        self.requires(self.tested_reference_str, headers=True, run=True, build=True)

    def generate(self):
        deps = CMakeDeps(self)
        # generate the config files for the tool require
        # tried, but doesn't work
        # deps.build_context_activated = ["json2cpp"]
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "example")
            self.run(cmd, env="conanrun")
