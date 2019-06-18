/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include <iostream>
#include <pcl/io/pcd_io.h>
#include "catch.hpp"
#include "pcps_pcl.h"
#include "pcps_context.h"
#include "pcps_plane_segmentator.h"

namespace
{

int testMainFunction()
{
    pcl::PointCloud<pcl::PointXYZ> pclPointCloud;

    if(pcl::io::loadPCDFile(std::string(PCPS_TEST_DATA_FOLDER) + "/organizer/input.pcd", pclPointCloud))
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

}

TEST_CASE("Example")
{
    REQUIRE(testMainFunction() == 0);
}
