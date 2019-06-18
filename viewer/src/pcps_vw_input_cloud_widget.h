/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_INPUT_CLOUD_WIDGET_H
#define PCPS_VW_INPUT_CLOUD_WIDGET_H

#include "pcps_vw_cloud_widget.h"

namespace pcps
{
namespace vw
{

class InputCloudWidget : public CloudWidget
{

public:
    using CloudWidget::CloudWidget;

protected:
    QString _updateViewer(const PointCloud* pointCloud, const Result* result, Q3DObserver& viewer) override;
};

}
}

#endif
