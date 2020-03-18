import os
from conans import ConanFile, CMake

class XeusConan(ConanFile):
    name = "xeus"
    version = "0.2.38"
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "zmq/4.3.2@bincrafters/stable",
        "cppzmq/4.6.0@_/_",
        "xtl/0.6.12@_/_",
        "openssl/1.0.2u@_/_",
        "nlohmann_json/3.7.3@_/_",
        "libuuid/1.0.3@_/_",
    )
    generators = "cmake", "virtualrunenv"
    build_policy = "missing" 
    exports_sources = "*"

    def build(self):
        cmake = CMake(self)
        cmake.verbose = True
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        self.copy("*.hpp", dst="include", src="hello")
        self.copy("*xeus.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["xeus"]

