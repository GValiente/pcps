/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_NORMAL_MERGE_CLOUD_WIDGET_H
#define PCPS_VW_NORMAL_MERGE_CLOUD_WIDGET_H

#include <unordered_map>
#include "pcps_vw_cloud_widget.h"

namespace pcps
{
namespace vw
{

class NormalMergeCloudWidget : public CloudWidget
{

public:
    using CloudWidget::CloudWidget;

protected:
    QString _updateViewer(const PointCloud* pointCloud, const Result* result, Q3DObserver& viewer) override;

private:
    std::unordered_map<int, QColor> _colorMap;
};

}
}

#endif
