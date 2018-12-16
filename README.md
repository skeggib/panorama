# Panorama

School project - creating a panorama from multiple images.

## Requirements

- OpenCV
- A C++ compiler

## Building the project

To generate the project:

```bash
mkdir build
cd build
cmake .. [-G <your_compiler>]
```

Then build the project with your compiler.

## Corner detection

To run the corner detection algorithm on an image:

```
./corner_detection <image>
```

It will open a window highlighting the detected corners and a second window with the corners detected by OpenCV.

## Corner pairing

To run the corner pairing algorithm on two images:

```
./corner_pairing <image1> <image2>
```

It will open a window showing the pairs of corners on the two images.

## Panorama reconstruction

Tu construct a panorama with two images:

```
./panorama <image1> <image2> <out>
```