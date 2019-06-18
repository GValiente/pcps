/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "test_util.h"

#include <pcl/io/pcd_io.h>
#include "catch.hpp"
#include "pcps_pcl.h"
#include "pcps_epsilon.h"
#include "pcps_normal_region.h"

namespace
{
    template<typename Type>
    Type stringToType(const std::string& field, std::string& buffer)
    {
        buffer.clear();
        buffer.insert(0, field.data(), field.size());

        std::istringstream stream(buffer);
        Type result;
        stream >> result;
        REQUIRE(! stream.fail());
        return result;
    }

    int stringToInt(const std::string& field, std::string& buffer)
    {
        return stringToType<int>(field, buffer);
    }

    float stringToFloat(const std::string& field, std::string& buffer)
    {
        if(field == "nan")
        {
            return std::numeric_limits<float>::quiet_NaN();
        }

        if(field == "-nan")
        {
            return -std::numeric_limits<float>::quiet_NaN();
        }

        return stringToType<float>(field, buffer);
    }
}

pcps::Cloud loadPointCloud(const std::string& filePath)
{
    pcl::PointCloud<pcl::PointXYZ> pclPointCloud;
    REQUIRE(! pcl::io::loadPCDFile(filePath, pclPointCloud));

    pcps::Cloud pointCloud;
    pcps::fromPCL(pclPointCloud, pointCloud);
    return pointCloud;
}

void savePointCloud(const pcps::Cloud& pointCloud, const std::string& filePath)
{
    pcl::PointCloud<pcl::PointXYZ> pclPointCloud;
    pcps::toPCL(pointCloud, pclPointCloud);
    REQUIRE(! pcl::io::savePCDFile(filePath, pclPointCloud));
}

pcps::Cloud loadNormalCloud(const std::string& filePath)
{
    std::string line;
    std::string field;
    std::string buffer;
    pcps::Cloud result;
    std::ifstream inputFileStream(filePath, std::ios::in | std::ios::binary);
    REQUIRE(inputFileStream.good());

    {
        REQUIRE(std::getline(inputFileStream, line));

        std::stringstream lineStream(line);
        REQUIRE(std::getline(lineStream, field, ','));
        result.sensorOrigin.x = stringToFloat(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        result.sensorOrigin.y = stringToFloat(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        result.sensorOrigin.z = stringToFloat(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        result.width = stringToInt(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        result.height = stringToInt(field, buffer);
    }

    while(std::getline(inputFileStream, line))
    {
        std::stringstream lineStream(line);
        pcps::Point normal;
        REQUIRE(std::getline(lineStream, field, ','));
        normal.x = stringToFloat(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        normal.y = stringToFloat(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        normal.z = stringToFloat(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        normal.aux = stringToFloat(field, buffer);
        result.points.push_back(normal);
    }

    return result;
}

void saveNormalCloud(const pcps::Cloud& normalCloud, const std::string& filePath)
{
    std::ofstream outputFileStream(filePath, std::ios::out | std::ios::binary);
    REQUIRE(outputFileStream.good());
    outputFileStream << normalCloud.sensorOrigin.x << ',';
    outputFileStream << normalCloud.sensorOrigin.y << ',';
    outputFileStream << normalCloud.sensorOrigin.z << ',';
    outputFileStream << normalCloud.width << ',';
    outputFileStream << normalCloud.height << '\n';

    for(const pcps::Point& normal : normalCloud.points)
    {
        outputFileStream << normal.x << ',';
        outputFileStream << normal.y << ',';
        outputFileStream << normal.z << ',';
        outputFileStream << normal.aux << '\n';
    }

    outputFileStream.close();
    REQUIRE(outputFileStream.good());
}

std::vector<pcps::NormalRegion> loadNormalRegions(const std::string& filePath)
{
    std::string line;
    std::string field;
    std::string buffer;
    std::vector<pcps::NormalRegion> result;
    std::ifstream inputFileStream(filePath, std::ios::in | std::ios::binary);
    REQUIRE(inputFileStream.good());

    while(std::getline(inputFileStream, line))
    {
        std::stringstream lineStream(line);
        pcps::NormalRegion normalRegion;
        REQUIRE(std::getline(lineStream, field, ','));
        normalRegion.x = stringToInt(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        normalRegion.y = stringToInt(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        normalRegion.width = stringToInt(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        normalRegion.height = stringToInt(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        normalRegion.normal.x = stringToFloat(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        normalRegion.normal.y = stringToFloat(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        normalRegion.normal.z = stringToFloat(field, buffer);
        REQUIRE(std::getline(lineStream, field, ','));
        normalRegion.normal.aux = stringToFloat(field, buffer);
        result.push_back(normalRegion);
    }

    return result;
}

void saveNormalRegions(const std::vector<pcps::NormalRegion>& normalRegions, const std::string& filePath)
{
    std::ofstream outputFileStream(filePath, std::ios::out | std::ios::binary);
    REQUIRE(outputFileStream.good());

    for(const pcps::NormalRegion& normalRegion : normalRegions)
    {
        outputFileStream << normalRegion.x << ',';
        outputFileStream << normalRegion.y << ',';
        outputFileStream << normalRegion.width << ',';
        outputFileStream << normalRegion.height << ',';
        outputFileStream << normalRegion.normal.x << ',';
        outputFileStream << normalRegion.normal.y << ',';
        outputFileStream << normalRegion.normal.z << ',';
        outputFileStream << normalRegion.normal.aux << '\n';
    }

    outputFileStream.close();
    REQUIRE(outputFileStream.good());
}

bool areSimilarClouds(const pcps::Cloud& a, const pcps::Cloud& b, int maxDifferentPoints)
{
    return areSimilarClouds(a, b, maxDifferentPoints, pcps::epsilon);
}

bool areSimilarClouds(const pcps::Cloud& a, const pcps::Cloud& b, int maxDifferentPoints, float precision)
{
    std::size_t numPoints = a.points.size();
    REQUIRE(maxDifferentPoints >= 0);
    REQUIRE(precision > 0);

    if(numPoints != b.points.size() || a.width != b.width || a.height != b.height || a.sensorOrigin != b.sensorOrigin)
    {
        return false;
    }

    int differentPoints = 0;

    for(std::size_t index = 0; index < numPoints; ++index)
    {
        const pcps::Point& ap = a.points[index];
        const pcps::Point& bp = b.points[index];

        if(! ap.isEqualTo(bp, precision) || std::abs(ap.aux - bp.aux) > precision)
        {
            ++differentPoints;
        }
    }

    return differentPoints <= maxDifferentPoints;
}
