/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_central_widget.h"

#include <QHBoxLayout>
#include "pcps_vw_settings.h"
#include "pcps_vw_options_widget.h"
#include "pcps_vw_stages_manager_widget.h"

namespace pcps
{
namespace vw
{

CentralWidget::CentralWidget(Project& project, Settings& settings, QWidget* parent) :
    QWidget(parent),
    _settings(settings),
    _optionsWidget(new OptionsWidget(project))
{
    _optionsWidget->setVisible(settings.showOptions());
    connect(_optionsWidget, SIGNAL(pointCloudProcessing()), this, SIGNAL(pointCloudProcessing()));
    connect(_optionsWidget, SIGNAL(pointCloudProcessed()), this, SIGNAL(pointCloudProcessed()));

    auto stagesManagerWidget = new StagesManagerWidget(project, settings, this);
    connect(this, SIGNAL(pointCloudLoaded()), stagesManagerWidget, SIGNAL(pointCloudLoaded()));
    connect(this, SIGNAL(pointCloudProcessing()), stagesManagerWidget, SIGNAL(pointCloudProcessing()));
    connect(this, SIGNAL(pointCloudProcessed()), stagesManagerWidget, SIGNAL(pointCloudProcessed()));

    auto layout = new QHBoxLayout(this);
    layout->addWidget(stagesManagerWidget);
    layout->addWidget(_optionsWidget);
}

void CentralWidget::onShowOptionsChanged()
{
    bool show = ! _optionsWidget->isVisible();
    _optionsWidget->setVisible(show);
    _settings.setShowOptions(show);
}

}
}
