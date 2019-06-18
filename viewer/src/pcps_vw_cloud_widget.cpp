/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_cloud_widget.h"

#include <QLabel>
#include <QVBoxLayout>
#include "Q3DObserver"
#include "pcps_vw_project.h"

namespace pcps
{
namespace vw
{

CloudWidget::CloudWidget(const Project& project, const Settings& settings, QWidget* parent) :
    QWidget(parent),
    _project(project),
    _settings(settings),
    _viewer(new Q3DObserver(this)),
    _viewerLabel(new QLabel(this))
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_viewer);
    _viewer->setPointSize(1);

    QWidget* viewerOverlayWidget = _viewer->overlayWidget();
    _viewerLabel->setStyleSheet("QLabel { color : white; }");

    auto viewerOverlayLayout = new QVBoxLayout(viewerOverlayWidget);
    viewerOverlayLayout->addWidget(_viewerLabel, 0, Qt::AlignTop | Qt::AlignLeft);
}

void CloudWidget::onPointCloudLoaded()
{
    _viewer->reset();
}

void CloudWidget::onPointCloudProcessing()
{
    _viewer->clear();
}

void CloudWidget::onPointCloudProcessed()
{
    onPointCloudProcessing();
    _viewerLabel->setText(_updateViewer(_project.getPointCloud(), _project.getResult(), *_viewer));
}

}
}
