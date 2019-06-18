/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_normal_extraction_cloud_widget.h"

#include "Q3DObserver"
#include "pcps_epsilon.h"
#include "pcps_vw_result.h"
#include "pcps_vw_color_map.h"
#include "pcps_vw_point_cloud.h"

namespace pcps
{
namespace vw
{

QString NormalExtractionCloudWidget::_updateViewer(const PointCloud* pointCloud, const Result* result,
                                                   Q3DObserver& viewer)
{
    QString text;

    if(pointCloud && result)
    {
        float minZ = pointCloud->minZ;
        float rangeZ = std::max(pointCloud->maxZ - minZ, epsilon);

        const Point* pointsData = result->organizationResult.points.data();
        const Cloud& normalCloud = result->normalExtractionResult;
        const Point* normalsData = normalCloud.points.data();
        int cols = normalCloud.width;
        int rows = normalCloud.height;
        viewer.reservePoints(cols * rows);

        for(int r = 0; r < rows; r += 4)
        {
            const Point* pointsRow = pointsData + (r * cols);
            const Point* normalsRow = normalsData + (r * cols);

            for(int c = 0; c < cols; c += 4)
            {
                const Point& normal = normalsRow[c];

                if(normal.aux > 0)
                {
                    const Point& point = pointsRow[c];
                    QVector3D from(point.x, point.y, point.z);
                    QVector3D to(point.x + normal.x, point.y + normal.y, point.z + normal.z);
                    float colorValue = (point.z - minZ) / rangeZ;
                    viewer.addLine(from, to, ColorMap::get(colorValue));
                }
            }
        }

        text += "Elapsed microseconds: ";
        text += QString::number(result->normalExtractionElapsedMcs);
    }

    return text;
}

}
}
