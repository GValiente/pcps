/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_SETTINGS_H
#define PCPS_VW_SETTINGS_H

#include <QObject>

class QSettings;

namespace pcps
{
namespace vw
{

class Settings : public QObject
{

public:
    explicit Settings(QObject* parent = nullptr);

    QString getOrganizationName() const;

    QString getApplicationName() const;

    QString getLastPointCloudPath() const;

    void setLastPointCloudPath(const QString& lastPointCloudPath);

    bool showOptions() const;

    void setShowOptions(bool show);

    QVariant getMainWindowGeometry() const;

    void setMainWindowGeometry(const QVariant& mainWindowGeometry);

    QVariant getMainWindowState() const;

    void setMainWindowState(const QVariant& mainWindowState);

private:
    QSettings* _qSettings;
};

}
}

#endif
