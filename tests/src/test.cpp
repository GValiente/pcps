/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include <chrono>
#include <iostream>
#include "catch.hpp"
#include "pcps_context.h"
#include "pcps_device_cloud.h"
#include "pcps_plane_segmentator.h"
#include "test_util.h"

TEST_CASE("Organizer 1x1")
{
    pcps::Cloud inputPointCloud;
    inputPointCloud.points.push_back(pcps::Point{ 0, 0, 0, 0 });

    pcps::Cloud expectedPointCloud = inputPointCloud;
    expectedPointCloud.width = 1;
    expectedPointCloud.height = 1;

    std::unique_ptr<pcps::Context> context = pcps::Context::buildDefault();
    pcps::Cloud outputPointCloud;
    pcps::Organizer organizer;
    REQUIRE(organizer.organize(inputPointCloud, outputPointCloud, *context));
    REQUIRE(outputPointCloud == expectedPointCloud);
}

TEST_CASE("Organizer")
{
    std::string testDataPath = std::string(PCPS_TEST_DATA_FOLDER) + "/organizer";
    pcps::Cloud inputPointCloud = loadPointCloud(testDataPath + "/input.pcd");
    pcps::Cloud expectedPointCloud = loadPointCloud(testDataPath + "/expected.pcd");
    std::unique_ptr<pcps::Context> context = pcps::Context::buildDefault();
    pcps::Cloud outputPointCloud;
    pcps::Organizer organizer;
    REQUIRE(organizer.organize(inputPointCloud, outputPointCloud, *context));
    REQUIRE(outputPointCloud == expectedPointCloud);

    auto startTime = std::chrono::high_resolution_clock::now();
    bool success = organizer.organize(inputPointCloud, outputPointCloud, *context);
    auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    REQUIRE(success);
    REQUIRE(outputPointCloud == expectedPointCloud);

    auto elapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    std::cout << "Organizer elapsed mcs: " << elapsedMcs << std::endl;
}

TEST_CASE("NormalExtractor 1x1")
{
    pcps::Cloud inputPointCloud;
    inputPointCloud.points.push_back(pcps::Point{ 0, 0, 0, 0 });
    inputPointCloud.width = 1;
    inputPointCloud.height = 1;

    std::unique_ptr<pcps::Context> context = pcps::Context::buildDefault();
    pcps::Cloud outputNormalCloud;
    pcps::NormalExtractor normalExtractor;
    REQUIRE(! normalExtractor.extract(inputPointCloud, outputNormalCloud, *context));
}

TEST_CASE("NormalExtractor")
{
    std::string testDataPath = std::string(PCPS_TEST_DATA_FOLDER) + "/normal_extractor";
    pcps::Cloud inputPointCloud = loadPointCloud(testDataPath + "/input.pcd");
    pcps::Cloud expectedNormalCloud = loadNormalCloud(testDataPath + "/expected.txt");
    std::unique_ptr<pcps::Context> context = pcps::Context::buildDefault();
    pcps::Cloud outputNormalCloud;
    pcps::NormalExtractor normalExtractor;
    REQUIRE(normalExtractor.extract(inputPointCloud, outputNormalCloud, *context));
    REQUIRE(areSimilarClouds(outputNormalCloud, expectedNormalCloud, 1168, 0.1f));

    auto startTime = std::chrono::high_resolution_clock::now();
    bool success = normalExtractor.extract(inputPointCloud, outputNormalCloud, *context);
    auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    REQUIRE(success);
    REQUIRE(areSimilarClouds(outputNormalCloud, expectedNormalCloud, 1168, 0.1f));

    auto elapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    std::cout << "NormalExtractor elapsed mcs without DeviceCloud: " << elapsedMcs << std::endl;

    outputNormalCloud = inputPointCloud;

    pcps::DeviceCloud inputPointDeviceCloud(inputPointCloud, *context);
    pcps::DeviceCloud outputDeviceCloud(outputNormalCloud, *context);
    startTime = std::chrono::high_resolution_clock::now();
    success = normalExtractor.extract(inputPointDeviceCloud, outputDeviceCloud, *context);
    elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    REQUIRE(success);
    REQUIRE(outputDeviceCloud.updateHostCloud(*context));
    REQUIRE(areSimilarClouds(outputNormalCloud, expectedNormalCloud, 1168, 0.1f));

    elapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    std::cout << "NormalExtractor elapsed mcs with DeviceCloud: " << elapsedMcs << std::endl;
}

TEST_CASE("NormalSplitter 1x1")
{
    pcps::Cloud inputNormalCloud;
    inputNormalCloud.points.push_back(pcps::Point{ 1, 0, 0, 1 });
    inputNormalCloud.width = 1;
    inputNormalCloud.height = 1;

    std::unique_ptr<pcps::Context> context = pcps::Context::buildDefault();
    std::vector<pcps::NormalRegion> outputNormalRegions;
    pcps::NormalSplitter normalSplitter;
    REQUIRE(normalSplitter.split(inputNormalCloud, outputNormalRegions, *context));
    REQUIRE(outputNormalRegions.size() == 1);
}

TEST_CASE("NormalSplitter")
{
    std::string testDataPath = std::string(PCPS_TEST_DATA_FOLDER) + "/normal_splitter";
    pcps::Cloud inputNormalCloud = loadNormalCloud(testDataPath + "/input.txt");
    std::vector<pcps::NormalRegion> expectedNormalRegions = loadNormalRegions(testDataPath + "/expected.txt");
    std::unique_ptr<pcps::Context> context = pcps::Context::buildDefault();
    std::vector<pcps::NormalRegion> outputNormalRegions;
    pcps::NormalSplitter normalSplitter;
    REQUIRE(normalSplitter.split(inputNormalCloud, outputNormalRegions, *context));
    REQUIRE(outputNormalRegions.size() == expectedNormalRegions.size());

    auto startTime = std::chrono::high_resolution_clock::now();
    bool success = normalSplitter.split(inputNormalCloud, outputNormalRegions, *context);
    auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    REQUIRE(success);
    REQUIRE(outputNormalRegions.size() == expectedNormalRegions.size());

    auto elapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    std::cout << "NormalSplitter elapsed mcs without DeviceCloud: " << elapsedMcs << std::endl;

    pcps::DeviceCloud inputNormalDeviceCloud(inputNormalCloud, *context);
    startTime = std::chrono::high_resolution_clock::now();
    success = normalSplitter.split(inputNormalDeviceCloud, outputNormalRegions, *context);
    elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    REQUIRE(success);
    REQUIRE(outputNormalRegions.size() == expectedNormalRegions.size());

    elapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    std::cout << "NormalSplitter elapsed mcs with DeviceCloud: " << elapsedMcs << std::endl;
}

TEST_CASE("NormalMerger 0")
{
    std::vector<pcps::NormalRegion> inputNormalRegions;
    pcps::NormalMerger normalMerger;
    std::vector<pcps::Plane> outputPlanes;
    normalMerger.merge(inputNormalRegions, outputPlanes);
    REQUIRE(outputPlanes.empty());
}

TEST_CASE("NormalMerger 1")
{
    std::vector<pcps::NormalRegion> inputNormalRegions;
    inputNormalRegions.push_back(pcps::NormalRegion{ pcps::Point{ 1, 0, 0, 1 }, 0, 0, 1, 1});

    pcps::NormalMerger normalMerger;
    std::vector<pcps::Plane> outputPlanes;
    normalMerger.merge(inputNormalRegions, outputPlanes);
    REQUIRE(outputPlanes.size() == 1);
}

TEST_CASE("NormalMerger")
{
    std::string testDataPath = std::string(PCPS_TEST_DATA_FOLDER) + "/normal_merger";
    std::vector<pcps::NormalRegion> inputNormalRegions = loadNormalRegions(testDataPath + "/input.txt");
    pcps::NormalMerger normalMerger;
    std::vector<pcps::Plane> outputPlanes;
    normalMerger.merge(inputNormalRegions, outputPlanes);
    REQUIRE(outputPlanes.size() == 135);

    auto startTime = std::chrono::high_resolution_clock::now();
    normalMerger.merge(inputNormalRegions, outputPlanes);
    auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    REQUIRE(outputPlanes.size() == 135);

    auto elapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    std::cout << "NormalMerger elapsed mcs: " << elapsedMcs << std::endl;
}

TEST_CASE("PlaneSegmentator disorganized")
{
    std::string testDataPath = std::string(PCPS_TEST_DATA_FOLDER) + "/organizer";
    pcps::Cloud inputPointCloud = loadPointCloud(testDataPath + "/input.pcd");
    std::unique_ptr<pcps::Context> context = pcps::Context::buildDefault();
    std::vector<pcps::Plane> outputPlanes;
    pcps::PlaneSegmentator planeSegmentator;
    REQUIRE(planeSegmentator.segmentate(inputPointCloud, outputPlanes, *context));
    REQUIRE(std::abs(int(outputPlanes.size()) - 135) <= 38);

    auto startTime = std::chrono::high_resolution_clock::now();
    bool success = planeSegmentator.segmentate(inputPointCloud, outputPlanes, *context);
    auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    REQUIRE(success);
    REQUIRE(std::abs(int(outputPlanes.size()) - 135) <= 38);

    auto elapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    std::cout << "PlaneSegmentator disorganized elapsed mcs: " << elapsedMcs << std::endl;
}

TEST_CASE("PlaneSegmentator organized")
{
    std::string testDataPath = std::string(PCPS_TEST_DATA_FOLDER) + "/organizer";
    pcps::Cloud inputPointCloud = loadPointCloud(testDataPath + "/expected.pcd");
    std::unique_ptr<pcps::Context> context = pcps::Context::buildDefault();
    std::vector<pcps::Plane> outputPlanes;
    pcps::PlaneSegmentator planeSegmentator;
    REQUIRE(planeSegmentator.segmentate(inputPointCloud, outputPlanes, *context));
    REQUIRE(std::abs(int(outputPlanes.size()) - 135) <= 38);

    auto startTime = std::chrono::high_resolution_clock::now();
    bool success = planeSegmentator.segmentate(inputPointCloud, outputPlanes, *context);
    auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    REQUIRE(success);
    REQUIRE(std::abs(int(outputPlanes.size()) - 135) <= 38);

    auto elapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    std::cout << "PlaneSegmentator organized elapsed mcs without DeviceCloud: " << elapsedMcs << std::endl;

    pcps::DeviceCloud inputPointDeviceCloud(inputPointCloud, *context);
    startTime = std::chrono::high_resolution_clock::now();
    success = planeSegmentator.segmentate(inputPointDeviceCloud, outputPlanes, *context);
    elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    REQUIRE(success);
    REQUIRE(std::abs(int(outputPlanes.size()) - 135) <= 38);

    elapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    std::cout << "PlaneSegmentator organized elapsed mcs with DeviceCloud: " << elapsedMcs << std::endl;
}
