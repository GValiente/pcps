/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include <QApplication>
#include <QSurfaceFormat>
#include "pcps_vw_main_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    pcps::vw::MainWindow mainWindow;
    mainWindow.show();

    if(! mainWindow.openLastPointCloud() && ! mainWindow.openPointCloud())
    {
        return -1;
    }

    return QApplication::exec();
}
