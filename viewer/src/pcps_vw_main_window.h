/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_MAIN_WINDOW_H
#define PCPS_VW_MAIN_WINDOW_H

#include <QMainWindow>

namespace pcps
{
namespace vw
{

class Project;
class Settings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    bool openLastPointCloud();

signals:
    void pointCloudLoaded();

    void pointCloudProcessing();

    void pointCloudProcessed();

    void showOptionsChanged();

public slots:
    bool openPointCloud();

private:
    Project* _project;
    Settings* _settings;

    bool _openPointCloud(const QString& pointCloudPath);
};

}
}

#endif
