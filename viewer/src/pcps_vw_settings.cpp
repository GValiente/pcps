/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_settings.h"

#include <QSettings>

namespace pcps
{
namespace vw
{

Settings::Settings(QObject* parent) :
    QObject(parent),
    _qSettings(new QSettings("pcps", "pcps viewer", this))
{
}

QString Settings::getOrganizationName() const
{
    return _qSettings->organizationName();
}

QString Settings::getApplicationName() const
{
    return _qSettings->applicationName();
}

QString Settings::getLastPointCloudPath() const
{
    return _qSettings->value("lastPointCloudPath").toString();
}

void Settings::setLastPointCloudPath(const QString& lastPointCloudPath)
{
    _qSettings->setValue("lastPointCloudPath", lastPointCloudPath);
}

bool Settings::showOptions() const
{
    return _qSettings->value("showOptions", true).toBool();
}

void Settings::setShowOptions(bool show)
{
    _qSettings->setValue("showOptions", show);
}

QVariant Settings::getMainWindowGeometry() const
{
    return _qSettings->value("mainWindowGeometry");
}

void Settings::setMainWindowGeometry(const QVariant& mainWindowGeometry)
{
    _qSettings->setValue("mainWindowGeometry", mainWindowGeometry);
}

QVariant Settings::getMainWindowState() const
{
    return _qSettings->value("mainWindowState");
}

void Settings::setMainWindowState(const QVariant& mainWindowState)
{
    _qSettings->setValue("mainWindowState", mainWindowState);
}

}
}
