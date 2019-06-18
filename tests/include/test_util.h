/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <string>
#include <vector>

namespace pcps
{
    class Cloud;
    class NormalRegion;
}

pcps::Cloud loadPointCloud(const std::string& filePath);

void savePointCloud(const pcps::Cloud& pointCloud, const std::string& filePath);

pcps::Cloud loadNormalCloud(const std::string& filePath);

void saveNormalCloud(const pcps::Cloud& normalCloud, const std::string& filePath);

std::vector<pcps::NormalRegion> loadNormalRegions(const std::string& filePath);

void saveNormalRegions(const std::vector<pcps::NormalRegion>& normalRegions, const std::string& filePath);

bool areSimilarClouds(const pcps::Cloud& a, const pcps::Cloud& b, int maxDifferentPoints);

bool areSimilarClouds(const pcps::Cloud& a, const pcps::Cloud& b, int maxDifferentPoints, float precision);

#endif
