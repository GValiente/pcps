/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_options_tree_widget.h"

#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include "pcps_logger.h"
#include "pcps_epsilon.h"
#include "pcps_vw_project.h"

namespace pcps
{
namespace vw
{

OptionsTreeWidget::OptionsTreeWidget(Project& project, QWidget* parent) :
    QTreeWidget(parent),
    _planeSegmentator(project.planeSegmentator)
{
    setColumnCount(1);
    setHeaderLabel("Options");
    setSelectionMode(QAbstractItemView::NoSelection);
    setFocusPolicy(Qt::NoFocus);

    auto orgItem = new QTreeWidgetItem(this, QStringList("Organization"));
    auto neItem = new QTreeWidgetItem(this, QStringList("Normal extraction"));
    auto nsItem = new QTreeWidgetItem(this, QStringList("Normal split"));
    auto nmItem = new QTreeWidgetItem(this, QStringList("Normal merge"));

    auto orgMaximumWidthSpinBox = new QSpinBox(this);
    orgMaximumWidthSpinBox->setMinimum(1);
    orgMaximumWidthSpinBox->setMaximum(std::numeric_limits<int>::max());
    orgMaximumWidthSpinBox->setValue(_planeSegmentator.organizer.getMaximumWidth());
    _addField(orgItem, "Max width:", "Maximum organized cloud width in cells [1..inf)", orgMaximumWidthSpinBox);
    connect(orgMaximumWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(_onOrgMaximumWidthSpinBoxValueChanged(int)));

    auto orgMaximumHeightSpinBox = new QSpinBox(this);
    orgMaximumHeightSpinBox->setMinimum(1);
    orgMaximumHeightSpinBox->setMaximum(std::numeric_limits<int>::max());
    orgMaximumHeightSpinBox->setValue(_planeSegmentator.organizer.getMaximumHeight());
    _addField(orgItem, "Max height:", "Maximum organized cloud height in cells [1..inf)", orgMaximumHeightSpinBox);
    connect(orgMaximumHeightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(_onOrgMaximumHeightSpinBoxValueChanged(int)));

    auto neSearchRadiusSpinBox = new QDoubleSpinBox(this);
    neSearchRadiusSpinBox->setMinimum(0);
    neSearchRadiusSpinBox->setMaximum(std::numeric_limits<double>::max());
    neSearchRadiusSpinBox->setValue(double(_planeSegmentator.normalExtractor.getSearchRadius()));
    _addField(neItem, "Search radius:", "Surface normal search radius in metres (epsilon..inf)", neSearchRadiusSpinBox);
    connect(neSearchRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(_onNeSearchRadiusSpinBoxValueChanged(double)));

    auto neFlipNormalsCheckBox = new QCheckBox(this);
    neFlipNormalsCheckBox->setChecked(_planeSegmentator.normalExtractor.flipNormals());
    _addField(neItem, "Flip normals:", "Indicates if the output normals must be flipped towards the view point", neFlipNormalsCheckBox);
    connect(neFlipNormalsCheckBox, &QCheckBox::clicked, this, [=](bool checked) {
        _planeSegmentator.normalExtractor.setFlipNormals(checked);
        fieldChanged();
    });

    auto nsMinimumRegionWidthSpinBox = new QSpinBox(this);
    nsMinimumRegionWidthSpinBox->setMinimum(1);
    nsMinimumRegionWidthSpinBox->setMaximum(std::numeric_limits<int>::max());
    nsMinimumRegionWidthSpinBox->setValue(_planeSegmentator.normalSplitter.getMinimumRegionWidth());
    _addField(nsItem, "Min region width:", "Minimum region width in cells [1..inf)", nsMinimumRegionWidthSpinBox);
    connect(nsMinimumRegionWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(_onNsMinimumRegionWidthSpinBoxValueChanged(int)));

    auto nsMinimumRegionHeightSpinBox = new QSpinBox(this);
    nsMinimumRegionHeightSpinBox->setMinimum(1);
    nsMinimumRegionHeightSpinBox->setMaximum(std::numeric_limits<int>::max());
    nsMinimumRegionHeightSpinBox->setValue(_planeSegmentator.normalSplitter.getMinimumRegionHeight());
    _addField(nsItem, "Min region height:", "Minimum region height in cells [1..inf)", nsMinimumRegionHeightSpinBox);
    connect(nsMinimumRegionHeightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(_onNsMinimumRegionHeightSpinBoxValueChanged(int)));

    auto nsStdDsvThresholdSpinBox = new QDoubleSpinBox(this);
    nsStdDsvThresholdSpinBox->setMinimum(0);
    nsStdDsvThresholdSpinBox->setMaximum(std::numeric_limits<double>::max());
    nsStdDsvThresholdSpinBox->setValue(double(_planeSegmentator.normalSplitter.getStdDsvThreshold()));
    _addField(nsItem, "Standard deviation:", "Minimum standard deviation of each output region in radians (epsilon..inf)", nsStdDsvThresholdSpinBox);
    connect(nsStdDsvThresholdSpinBox, SIGNAL(valueChanged(double)), this, SLOT(_onNsStdDsvThresholdSpinBoxValueChanged(double)));

    _nmMinimumStdDsvThresholdSpinBox = new QDoubleSpinBox(this);
    _nmMinimumStdDsvThresholdSpinBox->setMinimum(0);
    _nmMinimumStdDsvThresholdSpinBox->setMaximum(std::numeric_limits<double>::max());
    _nmMinimumStdDsvThresholdSpinBox->setValue(double(_planeSegmentator.normalMerger.getMinimumStdDsvThreshold()));
    _addField(nmItem, "Min standard deviation:", "Standard deviation in radians by which two normal regions are always merged (epsilon..inf)", _nmMinimumStdDsvThresholdSpinBox);
    connect(_nmMinimumStdDsvThresholdSpinBox, SIGNAL(valueChanged(double)), this, SLOT(_onNmStdDsvThresholdSpinBoxValueChanged()));

    _nmMaximumStdDsvThresholdSpinBox = new QDoubleSpinBox(this);
    _nmMaximumStdDsvThresholdSpinBox->setMinimum(0);
    _nmMaximumStdDsvThresholdSpinBox->setMaximum(std::numeric_limits<double>::max());
    _nmMaximumStdDsvThresholdSpinBox->setValue(double(_planeSegmentator.normalMerger.getMaximumStdDsvThreshold()));
    _addField(nmItem, "Max standard deviation:", "Standard deviation in radians by which two normal regions can be merged (epsilon..inf)", _nmMaximumStdDsvThresholdSpinBox);
    connect(_nmMaximumStdDsvThresholdSpinBox, SIGNAL(valueChanged(double)), this, SLOT(_onNmStdDsvThresholdSpinBoxValueChanged()));
}

void OptionsTreeWidget::_addField(QTreeWidgetItem* parent, const QString& labelText, const QString& toolTip,
                                  QWidget* editor)
{
    editor->setFixedWidth(80);

    auto label = new QLabel(labelText, this);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto widget = new QWidget(this);
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 2, -1, 4);
    layout->addWidget(label);
    layout->addWidget(editor);

    auto widgetItem = new QTreeWidgetItem(parent);
    widgetItem->setToolTip(0, toolTip);
    setItemWidget(widgetItem, 0, widget);
}

void OptionsTreeWidget::_onOrgMaximumWidthSpinBoxValueChanged(int value)
{
    if(! _planeSegmentator.organizer.setMaximumWidth(value))
    {
        PCPS_LOG_ERROR << "PlaneSegmentator setup failed" << std::endl;
        return;
    }

    fieldChanged();
}

void OptionsTreeWidget::_onOrgMaximumHeightSpinBoxValueChanged(int value)
{
    if(! _planeSegmentator.organizer.setMaximumHeight(value))
    {
        PCPS_LOG_ERROR << "PlaneSegmentator setup failed" << std::endl;
        return;
    }

    fieldChanged();
}

void OptionsTreeWidget::_onNeSearchRadiusSpinBoxValueChanged(double value)
{
    if(! _planeSegmentator.normalExtractor.setSearchRadius(std::max(float(value), epsilon * 2)))
    {
        PCPS_LOG_ERROR << "PlaneSegmentator setup failed" << std::endl;
        return;
    }

    fieldChanged();
}

void OptionsTreeWidget::_onNsMinimumRegionWidthSpinBoxValueChanged(int value)
{
    if(! _planeSegmentator.normalSplitter.setMinimumRegionWidth(value))
    {
        PCPS_LOG_ERROR << "PlaneSegmentator setup failed" << std::endl;
        return;
    }

    fieldChanged();
}

void OptionsTreeWidget::_onNsMinimumRegionHeightSpinBoxValueChanged(int value)
{
    if(! _planeSegmentator.normalSplitter.setMinimumRegionHeight(value))
    {
        PCPS_LOG_ERROR << "PlaneSegmentator setup failed" << std::endl;
        return;
    }

    fieldChanged();
}

void OptionsTreeWidget::_onNsStdDsvThresholdSpinBoxValueChanged(double value)
{
    if(! _planeSegmentator.normalSplitter.setStdDsvThreshold(std::max(float(value), epsilon * 2)))
    {
        PCPS_LOG_ERROR << "PlaneSegmentator setup failed" << std::endl;
        return;
    }

    fieldChanged();
}

void OptionsTreeWidget::_onNmStdDsvThresholdSpinBoxValueChanged()
{
    float minValue = std::max(float(_nmMinimumStdDsvThresholdSpinBox->value()), epsilon * 2);
    float maxValue = std::max(float(_nmMaximumStdDsvThresholdSpinBox->value()), minValue + (epsilon * 2));

    if(! _planeSegmentator.normalMerger.setStdDsvThresholds(minValue, maxValue))
    {
        PCPS_LOG_ERROR << "PlaneSegmentator setup failed" << std::endl;
        return;
    }

    fieldChanged();
}

}
}
