pcps
====

pcps is a small C++11 library which provides point cloud segmentation into planes with similar surface normals.

Since pcps allows computing the segmentation into the GPU besides the CPU, it can segment an [organized point cloud](http://pointclouds.org/documentation/tutorials/basic_structures.php) of 320x320 in less than 10 milliseconds on a Nvidia Geforce GTX 1060 6GB.

![pcps viewer screenshot](screenshot.png?raw=true)

## Features

- [Organized and disorganized point clouds](http://pointclouds.org/documentation/tutorials/basic_structures.php) are allowed as input.
- Three implemenentation backends are provided: CPU, OpenCL and CUDA.
- Allocated memory can be reused between different segmentations.
- Multiple tests are present for each provided algorithm.
- A 3D test viewer allows to see the output of each segmentation algorithm and to change their thresholds.
- Base library without external dependencies (the viewer and the tests require [PCL](http://www.pointclouds.org) to read [PCD files](http://www.pointclouds.org/documentation/tutorials/pcd_file_format.php)).
- Doxygen documentation provided for API reference (see doc/index.html).
- Licensed under [MIT license](LICENSE).

## Algorithms

To perform a point cloud segmentation, pcps provides the following algorithms:

- Organizer: organizes the given [disorganized point cloud](http://pointclouds.org/documentation/tutorials/basic_structures.php).
- NormalExtractor: estimates local surface normals at each point of the given organized point cloud.
- NormalSplitter: splits a given organized normal cloud into regions with low standard deviation.
- NormalMerger: merges the given normal regions into planes (groups) with low standard deviation.

## Tested build configurations (minimum required versions may be lower)

* Ubuntu 16.04 with gcc 5.4.0.
* Ubuntu 18.04 with gcc 7.3.0.

## Usage

The following example shows how to load a [PCL point cloud](http://www.pointclouds.org) stored in a PCL file [PCD file](http://www.pointclouds.org/documentation/tutorials/pcd_file_format.php)) and segment it into planes:

```C++
#include <iostream>
#include <pcl/io/pcd_io.h>
#include "catch.hpp"
#include "pcps_pcl.h"
#include "pcps_context.h"
#include "pcps_plane_segmentator.h"

int main(int argc, char** argv)
{
    pcl::PointCloud<pcl::PointXYZ> pclPointCloud;

    if(pcl::io::loadPCDFile("/path/to/pcl_cloud_file.pcd", pclPointCloud))
    {
        std::cerr << "pcl::io::loadPCDFile failed" << std::endl;
        return -1;
    }

    pcps::Cloud pointCloud;
    pcps::fromPCL(pclPointCloud, pointCloud);

    pcps::PlaneSegmentator planeSegmentator;
    std::vector<pcps::Plane> planes;
    std::unique_ptr<pcps::Context> context = pcps::Context::buildDefault();

    if(! planeSegmentator.segmentate(pointCloud, planes, *context))
    {
        std::cerr << "pcl::io::loadPCDFile failed" << std::endl;
        return -1;
    }

    return 0;
}
```

## How to build

A CMakeLists.txt is provided with this library, so in order to use it you only need to include this file in your CMake project.

### How to build the library with the CPU implementation

For the CPU implementation there's no external dependencies required, so the only software requirements are a C++11-compatible compiler and CMake >= 3.4:

```
sudo apt install build-essential cmake
mkdir build
cd build
cmake -DPCPS_IMPLEMENTATION=CPU -DCMAKE_BUILD_TYPE=Release ..
make -j<number_of_cpu_cores>
```

### How to build the library with the OpenCL implementation

The OpenCL implementation can segment an [unorganized point cloud](http://pointclouds.org/documentation/tutorials/basic_structures.php) of 320x320 on a Nvidia Geforce GTX 1060 6GB more than 23 times faster than the CPU implementation:

```
sudo apt install build-essential cmake
sudo apt install ocl-icd-libopencl1 opencl-headers clinfo ocl-icd-opencl-dev
mkdir build
cd build
cmake -DPCPS_IMPLEMENTATION=OpenCL -DCMAKE_BUILD_TYPE=Release ..
make -j<number_of_cpu_cores>
```

### How to build the library with the CUDA implementation

The CUDA implementation can segment an [unorganized point cloud](http://pointclouds.org/documentation/tutorials/basic_structures.php) of 320x320 on a Nvidia Geforce GTX 1060 6GB more than 30 times faster than the CPU implementation:

```
sudo apt install build-essential cmake
mkdir build
cd build
cmake -DPCPS_IMPLEMENTATION=CUDA -DCMAKE_BUILD_TYPE=Release ..
make -j<number_of_cpu_cores>
```

### How to build and run the unit tests

The unit tests require [PCL](http://www.pointclouds.org) to read [PCD files](http://www.pointclouds.org/documentation/tutorials/pcd_file_format.php).

For example, to build and run the unit tests with the OpenCL implementation:

```
sudo apt install build-essential cmake
sudo apt install ocl-icd-libopencl1 opencl-headers clinfo ocl-icd-opencl-dev
sudo apt install libproj-dev libpcl-dev
mkdir build
cd build
cmake -DPCPS_BUILD_TESTS=ON -DPCPS_IMPLEMENTATION=OpenCL -DCMAKE_BUILD_TYPE=Release ..
make -j<number_of_cpu_cores>
./tests/pcps-tests
```

### How to build and run the viewer

The provided 3D test viewer allows to see the output of each segmentation algorithm and to change their thresholds.

The viewer requires [PCL](http://www.pointclouds.org) to read [PCD files](http://www.pointclouds.org/documentation/tutorials/pcd_file_format.php).

For example, to build and run the viewer with the OpenCL implementation:

```
sudo apt install build-essential cmake
sudo apt install ocl-icd-libopencl1 opencl-headers clinfo ocl-icd-opencl-dev
sudo apt install libproj-dev libpcl-dev
mkdir build
cd build
cmake -DPCPS_BUILD_VIEWER=ON -DPCPS_IMPLEMENTATION=OpenCL -DCMAKE_BUILD_TYPE=Release ..
make -j<number_of_cpu_cores>
./tests/pcps-viewer
```

