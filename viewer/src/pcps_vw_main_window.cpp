/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_main_window.h"

#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QtConcurrent/QtConcurrentRun>
#include "pcps_logger.h"
#include "pcps_vw_project.h"
#include "pcps_vw_settings.h"
#include "pcps_vw_point_cloud.h"
#include "pcps_vw_central_widget.h"
#include "pcps_vw_show_progress_dialog.h"

namespace pcps
{
namespace vw
{

namespace
{
    std::shared_ptr<Project::LoadResult> loadPointCloud(Project* project, const QString& pointCloudPath)
    {
        std::unique_ptr<Project::LoadResult> result = project->loadPointCloud(pointCloudPath);

        if(! result)
        {
            PCPS_LOG_ERROR << "Point cloud load failed" << std::endl;
            return nullptr;
        }

        return std::shared_ptr<Project::LoadResult>(result.release());
    }
}

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    _project(new Project(this)),
    _settings(new Settings(this))
{
    setWindowTitle(_settings->getApplicationName());
    QCoreApplication::setApplicationName(windowTitle());

    auto openPointCloudAction = new QAction("Open point cloud...", this);
    connect(openPointCloudAction, SIGNAL(triggered()), this, SLOT(openPointCloud()));

    QMenu* fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(openPointCloudAction);

    auto showOptionsAction = new QAction("Show options", this);
    showOptionsAction->setCheckable(true);
    showOptionsAction->setChecked(_settings->showOptions());
    connect(showOptionsAction, SIGNAL(triggered()), this, SIGNAL(showOptionsChanged()));

    QMenu* viewMenu = menuBar()->addMenu("View");
    viewMenu->addAction(showOptionsAction);

    auto centralWidget = new CentralWidget(*_project, *_settings, this);
    setCentralWidget(centralWidget);
    connect(this, SIGNAL(pointCloudLoaded()), centralWidget, SIGNAL(pointCloudLoaded()));
    connect(this, SIGNAL(pointCloudProcessing()), centralWidget, SIGNAL(pointCloudProcessing()));
    connect(this, SIGNAL(pointCloudProcessed()), centralWidget, SIGNAL(pointCloudProcessed()));
    connect(this, SIGNAL(showOptionsChanged()), centralWidget, SLOT(onShowOptionsChanged()));

    if(restoreGeometry(_settings->getMainWindowGeometry().toByteArray()))
    {
        restoreState(_settings->getMainWindowState().toByteArray());
    }
    else
    {
        resize(1280, 720);
    }
}

bool MainWindow::openLastPointCloud()
{
    QString lastPointCloudPath = _settings->getLastPointCloudPath();

    if(lastPointCloudPath.isEmpty())
    {
        return false;
    }

    if(! _openPointCloud(lastPointCloudPath))
    {
        PCPS_LOG_ERROR << "Open last point cloud file failed: " << lastPointCloudPath.toStdString() << std::endl;
        return false;
    }

    return true;
}

bool MainWindow::openPointCloud()
{
    QString pointCloudPath = QFileDialog::getOpenFileName(this, "Open point cloud", _settings->getLastPointCloudPath(),
                                                          "PCL point cloud files (*.pcd)");

    if(pointCloudPath.isEmpty())
    {
        return false;
    }

    if(! _openPointCloud(pointCloudPath))
    {
        PCPS_LOG_ERROR << "Open selected point cloud file failed: " << pointCloudPath.toStdString() << std::endl;
        QMessageBox::critical(this, QCoreApplication::applicationName(), "Point cloud load failed.");
        return false;
    }

    return true;
}

bool MainWindow::_openPointCloud(const QString& pointCloudPath)
{
    auto future = QtConcurrent::run(loadPointCloud, _project, pointCloudPath);
    showProgressDialog(future);

    std::shared_ptr<Project::LoadResult> result = future.result();

    if(! result)
    {
        PCPS_LOG_ERROR << "Point cloud load failed: " << pointCloudPath.toStdString() << std::endl;
        return false;
    }

    pointCloudProcessing();

    if(! _project->setLoadResult(std::move(*result)))
    {
        PCPS_LOG_ERROR << "Project setup failed: " << pointCloudPath.toStdString() << std::endl;
        pointCloudProcessed();
        return false;
    }

    QFileInfo sourceInfo(pointCloudPath);
    setWindowTitle(sourceInfo.fileName() + " (" + sourceInfo.absoluteDir().path() + ") - " + QCoreApplication::applicationName());
    _settings->setLastPointCloudPath(pointCloudPath);
    pointCloudLoaded();
    pointCloudProcessed();
    return true;
}

}
}
