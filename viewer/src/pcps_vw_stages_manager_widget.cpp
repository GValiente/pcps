/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_stages_manager_widget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "pcps_vw_input_cloud_widget.h"
#include "pcps_vw_organization_cloud_widget.h"
#include "pcps_vw_normal_extraction_cloud_widget.h"
#include "pcps_vw_normal_split_cloud_widget.h"
#include "pcps_vw_normal_merge_cloud_widget.h"

namespace pcps
{
namespace vw
{

StagesManagerWidget::StagesManagerWidget(const Project& project, const Settings& settings, QWidget* parent) :
    QWidget(parent)
{
    auto inputCloudWidget = new InputCloudWidget(project, settings, this);
    connect(this, SIGNAL(pointCloudLoaded()), inputCloudWidget, SLOT(onPointCloudLoaded()));
    connect(this, SIGNAL(pointCloudProcessing()), inputCloudWidget, SLOT(onPointCloudProcessing()));
    connect(this, SIGNAL(pointCloudProcessed()), inputCloudWidget, SLOT(onPointCloudProcessed()));

    auto organizationCloudWidget = new OrganizationCloudWidget(project, settings, this);
    connect(this, SIGNAL(pointCloudLoaded()), organizationCloudWidget, SLOT(onPointCloudLoaded()));
    connect(this, SIGNAL(pointCloudProcessing()), organizationCloudWidget, SLOT(onPointCloudProcessing()));
    connect(this, SIGNAL(pointCloudProcessed()), organizationCloudWidget, SLOT(onPointCloudProcessed()));

    auto normalExtractionCloudWidget = new NormalExtractionCloudWidget(project, settings, this);
    connect(this, SIGNAL(pointCloudLoaded()), normalExtractionCloudWidget, SLOT(onPointCloudLoaded()));
    connect(this, SIGNAL(pointCloudProcessing()), normalExtractionCloudWidget, SLOT(onPointCloudProcessing()));
    connect(this, SIGNAL(pointCloudProcessed()), normalExtractionCloudWidget, SLOT(onPointCloudProcessed()));

    auto normalSplitCloudWidget = new NormalSplitCloudWidget(project, settings, this);
    connect(this, SIGNAL(pointCloudLoaded()), normalSplitCloudWidget, SLOT(onPointCloudLoaded()));
    connect(this, SIGNAL(pointCloudProcessing()), normalSplitCloudWidget, SLOT(onPointCloudProcessing()));
    connect(this, SIGNAL(pointCloudProcessed()), normalSplitCloudWidget, SLOT(onPointCloudProcessed()));

    auto normalMergeCloudWidget = new NormalMergeCloudWidget(project, settings, this);
    connect(this, SIGNAL(pointCloudLoaded()), normalMergeCloudWidget, SLOT(onPointCloudLoaded()));
    connect(this, SIGNAL(pointCloudProcessing()), normalMergeCloudWidget, SLOT(onPointCloudProcessing()));
    connect(this, SIGNAL(pointCloudProcessed()), normalMergeCloudWidget, SLOT(onPointCloudProcessed()));

    auto horizontalLayout1 = new QHBoxLayout();
    horizontalLayout1->setContentsMargins(0, 0, 0, 0);
    horizontalLayout1->addWidget(_getContainerWidget("Input", inputCloudWidget));
    horizontalLayout1->addWidget(_getContainerWidget("Organization", organizationCloudWidget));
    horizontalLayout1->addWidget(_getContainerWidget("Normal extraction", normalExtractionCloudWidget));

    auto horizontalLayout2 = new QHBoxLayout();
    horizontalLayout2->setContentsMargins(0, 0, 0, 0);
    horizontalLayout2->addWidget(_getContainerWidget("Normal split", normalSplitCloudWidget));
    horizontalLayout2->addWidget(_getContainerWidget("Normal merge", normalMergeCloudWidget));

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(horizontalLayout1);
    layout->addLayout(horizontalLayout2);
}

QWidget* StagesManagerWidget::_getContainerWidget(const QString& title, QWidget* widget)
{
    auto containerWidget = new QWidget(this);
    auto label = new QLabel(title, containerWidget);
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

    auto layout = new QVBoxLayout(containerWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    layout->addWidget(widget);

    return containerWidget;
}

}
}
