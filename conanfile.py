import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy

class json2cppRecipe(ConanFile):
    name = "json2cpp"
    version = "0.0.1"
    package_type = "header-library"

    # Optional metadata
    license = "MIT"
    author = "<Put your name here> <And your email here>"
    url = "https://github.com/lefticus/json2cpp"
    description = "Compiles JSON into static constexpr C++ data structures with nlohmann::json API "
    topics = ("json", "constexpr")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "test/*", "examples/*"

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("valijson/1.0", transitive_headers=True)
        self.requires("spdlog/1.11.0")
        self.requires("nlohmann_json/3.11.2")
        self.requires("fmt/9.1.0")
        self.requires("docopt.cpp/0.6.3")

    def build_requirements(self):
        self.test_requires("catch2/2.13.10")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["ENABLE_LARGE_TESTS"] = "OFF"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(
            self,
            "LICENSE",
            self.source_folder,
            self.package_folder
        )
        cmake = CMake(self)
        cmake.install()

    def package_id(self):
        # the package exports a tool and a header-only library, so compiler and
        # build type don't matter downstream
        self.info.settings.rm_safe("compiler")
        self.info.settings.rm_safe("build_type")

    def package_info(self):
        self.cpp_info.libs = ["json2cpp"]

        # the projects generates its own json2cppConfig.cmake
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.builddirs.append(os.path.join("lib", "cmake", "json2cpp"))
