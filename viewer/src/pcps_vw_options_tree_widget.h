/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_OPTIONS_TREE_WIDGET_H
#define PCPS_VW_OPTIONS_TREE_WIDGET_H

#include <QTreeWidget>

class QDoubleSpinBox;

namespace pcps
{

class PlaneSegmentator;

namespace vw
{

class Project;

class OptionsTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    OptionsTreeWidget(Project& project, QWidget* parent = nullptr);

signals:
    void fieldChanged();

private:
    PlaneSegmentator& _planeSegmentator;
    QDoubleSpinBox* _nmMinimumStdDsvThresholdSpinBox;
    QDoubleSpinBox* _nmMaximumStdDsvThresholdSpinBox;

    void _addField(QTreeWidgetItem* parent, const QString& labelText, const QString& toolTip, QWidget* editor);

private slots:
    void _onOrgMaximumWidthSpinBoxValueChanged(int value);

    void _onOrgMaximumHeightSpinBoxValueChanged(int value);

    void _onNeSearchRadiusSpinBoxValueChanged(double value);

    void _onNsMinimumRegionWidthSpinBoxValueChanged(int value);

    void _onNsMinimumRegionHeightSpinBoxValueChanged(int value);

    void _onNsStdDsvThresholdSpinBoxValueChanged(double value);

    void _onNmStdDsvThresholdSpinBoxValueChanged();
};

}
}

#endif
