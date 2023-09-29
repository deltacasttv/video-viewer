from conan import ConanFile

class MyPkg(ConanFile):
    settings = "os"

    def requirements(self):
        self.requires("glfw/3.3.6")
        self.requires("opengl/system")
        if self.settings.os == "Linux":
            self.requires("xorg/system")

