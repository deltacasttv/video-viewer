## Unreleased

### Fixed
- Check framebuffer completeness before unbinding in `create_framebuffers()`

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
