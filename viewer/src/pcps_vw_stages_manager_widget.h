/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_STAGES_MANAGER_WIDGET_H
#define PCPS_VW_STAGES_MANAGER_WIDGET_H

#include <QWidget>

namespace pcps
{
namespace vw
{

class Project;
class Settings;

class StagesManagerWidget : public QWidget
{
    Q_OBJECT

public:
    StagesManagerWidget(const Project& project, const Settings& settings, QWidget* parent = nullptr);

signals:
    void pointCloudLoaded();

    void pointCloudProcessing();

    void pointCloudProcessed();

private:
    QWidget* _getContainerWidget(const QString& title, QWidget* widget);
};

}
}

#endif
