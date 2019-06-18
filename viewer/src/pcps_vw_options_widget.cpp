/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_options_widget.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QtConcurrent/qtconcurrentrun.h>
#include "pcps_logger.h"
#include "pcps_vw_project.h"
#include "pcps_vw_options_tree_widget.h"
#include "pcps_vw_show_progress_dialog.h"

namespace pcps
{
namespace vw
{

namespace
{
    struct ProcessResult
    {
        std::unique_ptr<Result> result;
    };

    std::shared_ptr<ProcessResult> processCurrentPointCloud(Project* project)
    {
        std::unique_ptr<Result> result = project->processCurrentPointCloud();
        std::shared_ptr<ProcessResult> processResult;

        if(result)
        {
            processResult.reset(new ProcessResult{ std::move(result) });
        }
        else
        {
            PCPS_LOG_ERROR << "Current point cloud process failed" << std::endl;
        }

        return processResult;
    }
}

OptionsWidget::OptionsWidget(Project& project, QWidget* parent) :
    QWidget(parent),
    _project(project)
{
    _reloadButton = new QPushButton("Reload", this);
    _reloadButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _reloadButton->setDisabled(true);
    connect(_reloadButton, SIGNAL(clicked()), this, SLOT(_onReloadButtonClicked()));

    auto buttonWidget = new QWidget(this);
    auto buttonLayout = new QVBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->addWidget(_reloadButton, 0, Qt::AlignCenter);

    auto optionsTreeWidget = new OptionsTreeWidget(project, this);
    connect(optionsTreeWidget, SIGNAL(fieldChanged()), this, SLOT(_onTreeWidgetFieldChanged()));

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(optionsTreeWidget);
    layout->addWidget(buttonWidget);

    setFixedWidth(310);
}

void OptionsWidget::_onTreeWidgetFieldChanged()
{
    _reloadButton->setDisabled(false);
}

void OptionsWidget::_onReloadButtonClicked()
{
    auto future = QtConcurrent::run(processCurrentPointCloud, &_project);
    showProgressDialog(future);

    std::shared_ptr<ProcessResult> processResult = future.result();

    if(! processResult)
    {
        PCPS_LOG_ERROR << "Current point cloud process failed" << std::endl;
        return;
    }

    pointCloudProcessing();

    if(! _project.setResult(std::move(processResult->result)))
    {
        PCPS_LOG_ERROR << "Project result setup failed" << std::endl;
        pointCloudProcessed();
        return;
    }

    _reloadButton->setDisabled(true);
    pointCloudProcessed();
}

}
}
