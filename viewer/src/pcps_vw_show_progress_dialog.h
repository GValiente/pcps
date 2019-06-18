/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_SHOW_PROGRESS_DIALOG_H
#define PCPS_VW_SHOW_PROGRESS_DIALOG_H

#include <QApplication>
#include <QFutureWatcher>
#include <QProgressDialog>

namespace pcps
{
namespace vw
{

template<typename T>
static void showProgressDialog(const QFuture<T>& future, const QString& text = "Loading...")
{
    QProgressDialog dialog(QApplication::activeWindow());
    dialog.setWindowTitle(QCoreApplication::applicationName());
    dialog.setLabelText(text);
    dialog.setCancelButton(nullptr);
    dialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    QFutureWatcher<T> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    futureWatcher.setFuture(future);
    dialog.exec();
    futureWatcher.waitForFinished();
}

}
}

#endif
