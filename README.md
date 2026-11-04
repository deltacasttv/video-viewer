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

## Input formats

`VideoViewer::init()` takes the size of the frames (`texture_width` x `texture_height`, W x H below)
and an `InputFormat`. `lock_data()` then gives access to a buffer that holds one progressive frame in
that format: lines from top to bottom, pixels from left to right, no line padding. The exact byte and
bit layout of each format is documented on its enumerator in
[`include/videoviewer/videoviewer.hpp`](include/videoviewer/videoviewer.hpp).

| `InputFormat` | Memory layout | Buffer size (bytes) | Size constraints |
|---|---|---|---|
| `ycbcr_422_8` | UYVY: `Cb0 Y0 Cr0 Y1` per pair of pixels | `W*H*2` | W even |
| `ycbcr_422_10_le_msb` | v210: 6 pixels in 4 little-endian 32-bit words of three 10-bit samples, one continuous stream over the frame (lines are not padded) | `W*H*8/3` | H multiple of 3, W even |
| `ycbcr_422_10_be` | 10-bit samples `Cb0 Y0 Cr0 Y1` packed most significant bit first, 5 bytes per pair of pixels | `W*H*5/2` | W multiple of 8 (multiple of 64 for a correct display today, see the header) |
| `ycbcr_444_8` | `Cb Y Cr` | `W*H*3` | W multiple of 4 |
| `rgb_444_8` | `R G B` | `W*H*3` | W multiple of 4 |
| `bgr_444_8` | `R G B`, currently the same byte order as `rgb_444_8` | `W*H*3` | W multiple of 4 |
| `bgr_444_8_le_msb` | `B G R X` (X is ignored), i.e. the little-endian word `0xXXRRGGBB` | `W*H*4` | none |
| `nv12` | Y plane (`W*H` bytes), then interleaved `Cb Cr` plane (H/2 lines of W/2 pairs) | `W*H*3/2` | W multiple of 4, H even |
| `p010` | Same planes as `nv12` with 16-bit little-endian samples, value in bits 15..6 (`sample << 6`) | `W*H*3` | W and H even |

YCbCr formats are converted with BT.709 coefficients and expect limited range values (Y 16-235 and
Cb/Cr 16-240 for 8-bit samples, 4 times these values for 10-bit samples). `init()` returns false for the
`reserved_*` formats, which are not supported, and for sizes that break the constraints above, except
for the few constraints marked "not checked" in the header: frames that break those are displayed garbled.
