# DELTACAST Video Viewer

Video monitoring module that manages to create a window with OpenGL context that displays incoming video data.

OS Support: Windows and Linux

## How to build

VideoViewer requires some dependencies to be installed on the system:
- glfw v3.3.6
- Python 3.x

### Retrieve dependencies with Conan (optional)

To use Conan 1.x to retrieve the dependencies, create the `modules`` directory and use the install command:

```shell
mkdir /path/to/modules
cd /path/to/modules
conan install /path/to/video-viewer -b missing -g cmake_find_package
```

### Building with CMake

If you used Conan to retrieve your dependencies, you can use the following commands to build the project:

```shell
cd /path/to/video-viewer
cmake -S . -B build -DCMAKE_MODULE_PATH:PATH=/path/to/modules
cmake --build build
```


