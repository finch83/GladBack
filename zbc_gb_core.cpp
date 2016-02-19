#include <QDebug>

#include "zbc_gb_core.h"
#include "zbc_gb_taskitembase.h"

#include <QTimer>


//C-tor
ZBC_GB_Core::ZBC_GB_Core(QObject *parent) : QObject(parent)
{
    m_pTimer        = new QTimer(this);
    m_pTaskVector   = new ZBC_GB_TaskVector(this);
    m_pTaskVector->pushTasks();

    connect(m_pTimer,
            &QTimer::timeout,
            m_pTaskVector,
            &ZBC_GB_TaskVector::checkTimeOfTasks);
}


void ZBC_GB_Core::run()
{
    m_pTimer->start(999);
}
