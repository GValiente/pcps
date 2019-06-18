/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_CLOUD_WIDGET_H
#define PCPS_VW_CLOUD_WIDGET_H

#include <QWidget>

class QLabel;
class Q3DObserver;

namespace pcps
{

namespace vw
{

class Result;
class Project;
class Settings;
class PointCloud;

class CloudWidget : public QWidget
{
    Q_OBJECT

public:
    CloudWidget(const Project& project, const Settings& settings, QWidget* parent = nullptr);

public slots:
    void onPointCloudLoaded();

    void onPointCloudProcessing();

    void onPointCloudProcessed();

protected:
    const Settings& _getSettings() const noexcept
    {
        return _settings;
    }

    virtual QString _updateViewer(const PointCloud* pointCloud, const Result* result, Q3DObserver& viewer) = 0;

private:
    const Project& _project;
    const Settings& _settings;
    Q3DObserver* _viewer;
    QLabel* _viewerLabel;
};

}
}

#endif
