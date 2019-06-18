/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_PROJECT_H
#define PCPS_VW_PROJECT_H

#include <memory>
#include <QObject>
#include "pcps_context.h"
#include "pcps_plane_segmentator.h"
#include "pcps_vw_result.h"
#include "pcps_vw_point_cloud.h"

namespace pcps
{
namespace vw
{

class Project : public QObject
{

public:
    PlaneSegmentator planeSegmentator;

    struct LoadResult
    {
        std::unique_ptr<PointCloud> pointCloud;
        std::unique_ptr<Result> result;
    };

    using QObject::QObject;

    const PointCloud* getPointCloud() const noexcept
    {
        return _pointCloud.get();
    }

    const Result* getResult() const noexcept
    {
        return _result.get();
    }

    std::unique_ptr<LoadResult> loadPointCloud(const QString& pointCloudPath);

    bool setLoadResult(LoadResult&& loadResult);

    std::unique_ptr<Result> processCurrentPointCloud();

    bool setResult(std::unique_ptr<Result>&& result) noexcept;

private:
    std::unique_ptr<Context> _context;
    std::unique_ptr<PointCloud> _pointCloud;
    std::unique_ptr<Result> _result;

    std::unique_ptr<Result> _processPointCloud(const PointCloud& pointCloud);
};

}
}

#endif
