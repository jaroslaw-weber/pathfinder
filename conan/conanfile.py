import os

from conans import ConanFile, CMake, tools

class CppEuropeBuild(ConanFile):
    name="cpp_europe"
    author="Jaroslaw Weber"
    version="0.1"
    settings = "cppstd", "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires="boost/1.66.0@conan/stable"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')
