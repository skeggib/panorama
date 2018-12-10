# Panorama

School project - creating a panorama from multiple images.

## Requirements

- OpenCV
- A C++ compiler

## Building the project

Generate the project :

```bash
mkdir build
cd build
cmake .. [-G <your_compiler>]
```

Then build the project with your compiler.

## Corner detection

To run the corner detection algorithm on an image :

```
./fast <image>
```

It will open a window highlighting the detected corners and a second window with the corners detected by OpenCV.