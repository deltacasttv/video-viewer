# DELTACAST Video Viewer

Video monitoring module that manages to create a window with OpenGL context that displays incoming video data.

OS Support: Windows, MacOS and Linux.

Opengl Cor Profile : 4.1.

See https://www.deltacast.tv for more video products.

## How to build

VideoViewer requires some dependencies to be installed on the system:
- glfw 3.4
- glm 20230113
- Python 3.x

### Retrieve dependencies with Conan (optional)

To use Conan 2.x to retrieve the dependencies, use the following command line

```shell
cd /path/to/video-viewer
conan install conanfile.txt -b missing
```

### Building with CMake

If you used Conan to retrieve your dependencies, you can use the following commands to build the project:

```shell
cd /path/to/video-viewer
cmake --preset conan-release
cmake --build --preset conan-release
```


