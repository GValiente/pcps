/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_OPTIONS_WIDGET_H
#define PCPS_VW_OPTIONS_WIDGET_H

#include <QWidget>

class QPushButton;

namespace pcps
{
namespace vw
{

class Project;

class OptionsWidget : public QWidget
{
    Q_OBJECT

public:
    OptionsWidget(Project& project, QWidget* parent = nullptr);

signals:
    void pointCloudProcessing();

    void pointCloudProcessed();

private:
    Project& _project;
    QPushButton* _reloadButton;

private slots:
    void _onTreeWidgetFieldChanged();

    void _onReloadButtonClicked();
};

}
}

#endif
