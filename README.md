# Neko Upscale

A simple Qt6 GUI for the [realesrgan-ncnn-vulkan](https://github.com/xinntao/Real-ESRGAN-ncnn-vulkan) command-line tool.

This application provides a user-friendly interface to upscale images and directories of images without needing to use the terminal.

## Features

- Process a single image file or a directory of images.
- Drag-and-drop support (via file manager context menu integration).
- Launch with a path from the command line.
- Simple UI to adjust upscale ratio (2x, 3x, 4x) and output format (PNG, JPG, WEBP).
- Advanced panel to specify a custom output path and thread count.
- Real-time progress bar for both single-file and batch processing.
- Automatically exits after a successful upscale operation.

## Prerequisites

Before building, ensure you have the following installed:

1.  **`realesrgan-ncnn-vulkan`**: The executable must be available in your system's `PATH`.
2.  **Qt 6**: The development libraries for Qt6 are required (e.g., `qt6-base-dev` on Debian/Ubuntu).
3.  **CMake**: A modern version of CMake (3.16+).
4.  **A C++ Compiler**: A compiler that supports C++17 (like GCC or Clang).

## How to Build

1.  **Clone the repository (or download the source code):**
    ```bash
    git clone <repository-url>
    cd neko-upscale
    ```

2.  **Create a build directory:**
    ```bash
    mkdir build && cd build
    ```

3.  **Configure the project with CMake:**
    ```bash
    cmake ..
    ```

4.  **Compile the application:**
    ```bash
    make
    ```

    The final executable, `neko-upscale`, will be located in the `build` directory.

## How to Use

There are two ways to run the application:

1.  **Directly:**
    ```bash
    ./neko-upscale
    ```
    The application will open, and you can use the "Browse..." button to select an image or a directory.

2.  **From the Command Line with a Path:**
    You can pass an image file or a directory as the first argument.
    ```bash
    # Process a single image
    ./neko-upscale /path/to/my_image.png

    # Process all images in a directory
    ./neko-upscale /path/to/my_images/
    ```

This is ideal for integrating with file manager context menus (e.g., in Dolphin, Nautilus, or Finder).

## License

This project is licensed under the MIT License.
