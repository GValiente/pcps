/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_CENTRAL_WIDGET_H
#define PCPS_VW_CENTRAL_WIDGET_H

#include <QWidget>

namespace pcps
{
namespace vw
{

class Project;
class Settings;
class OptionsWidget;

class CentralWidget : public QWidget
{
    Q_OBJECT

public:
    CentralWidget(Project& project, Settings& settings, QWidget* parent = nullptr);

signals:
    void pointCloudLoaded();

    void pointCloudProcessing();

    void pointCloudProcessed();

public slots:
    void onShowOptionsChanged();

private:
    Settings& _settings;
    OptionsWidget* _optionsWidget;
};

}
}

#endif
