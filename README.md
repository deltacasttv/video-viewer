# DELTACAST Video Viewer

Video monitoring module that manages to create a window with OpenGL context that displays incoming video data.

OS Support: Windows, MacOS and Linux.

OpenGL Core Profile: 4.1.

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

### Run the sample application

Once the project is configured, you can build and execute the example
`videoviewer_sample` located in `sample/`:

```shell
cmake --build --preset conan-release --target videoviewer_sample
./build/Release/sample/videoviewer_sample
```

### Run the tests

The project provides Catch2 based tests in the `tests/` directory. After
building, run them with CTest:

```shell
ctest --preset conan-release
```


