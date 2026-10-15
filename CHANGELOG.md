# Changelog

## [Unreleased]

### Added
- `VideoViewer::set_line_alignment()` to display `ycbcr_422_10_le_msb` buffers whose lines are padded, e.g. to 128 bytes as in standard v210 (#24)

### Fixed
- Pixel sliding artifacts in `ycbcr_422_10_le_msb` when the width is not a multiple of 6 pixels (e.g. 1280x720, 2048x1536, 4096x2160): every line now starts on a new group of 6 pixels, so the buffer size is `ceil(width / 6) * 16 * height` bytes (unchanged for widths that are a multiple of 6), and the height no longer has to be a multiple of 3 (#24)

## [2.0.0]

### Added
- Add GitHub Actions workflows for CI and tagged releases
- Make test and example building as optional

### Changed
- Build and test on Windows, Linux and macOS in CI
- Remove gl3w dependency and use glad instead to use conan dependency management and avoid gl3w generation

## 1.5.0

### Fixed

- Check framebuffer completeness before unbinding in `create_framebuffers()`
- Prevent buffer overruns in color bar drawing routines
- Install `videoviewer.hpp` as the public header
- Avoid sleeping with negative durations in the render loops (#29)
- Ensure OpenGL Framebuffers are deleted (#31)
- Fix some typos in comments and README

### Added
- ColorBar unit test
- NV12 and P010 support


## 1.4.1

### Fixed
- VideoViewer cannot be initialized again after calling the release method [issue #26, PR !27]

## 1.4.0

### Changed
- Rollback version of OpenGL to 4.1 to introduce MacOS support [issue #9, PR !23]

## 1.3.0
 
### Added
- Upgrade dependencies to conan 2.x

## 1.2.0

### Added
- Add support for BGR 4:4:4:4 8bits (little endian + padding) [Issue #19, PR !20]

## 1.1.2

### Fixes
- Fix GL_CHECK and GLFW_CHECK macro on GCC with < C++20
  
## 1.1.1

### Fixes
- Improve GL_CHECK and GLFW_CHECK macros to support conditional statements

## 1.1.0

### Added
- Add support BGR 4:4:4 8bits

## 1.0.2

### Added
- Fix wrong operands types in shader [Issue #11]

## 1.0.1

### Added
- Git describe version protection [Issue #7]

## 1.0.0

### Added
- Initial pixel packings support:
    - YCbCr 4:2:2 8bits
    - YCbCr 4:2:2 10bits (little endian + padding)
    - YCbCr 4:2:2 10bits (Big endian)
    - YCbCr 4:4:4 8bits
    - RGB 4:4:4 8bits
- Static library that supports Windows and Linux OS
