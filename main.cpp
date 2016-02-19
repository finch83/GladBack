#include "zbc_gb_core.h"

#include <QCoreApplication>

int main(int argc, char** argv)
{
    QCoreApplication    app(argc, argv);
    ZBC_GB_Core sysCore;
    sysCore.run();

return app.exec();
}
