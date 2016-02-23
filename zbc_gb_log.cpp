#include "zbc_gb_log.h"

#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QMap>
#include <QTextStream>
#include <QTime>

const QString ZBC_GB_Log::m_strFileName = QDir::currentPath() + QString("/log.gbs");
ZBC_GB_Log ZBC_GB_Log::m_logInstance;


//C-tor
ZBC_GB_Log::ZBC_GB_Log(QObject *parent) : QObject(parent)
{
    m_pfleLog               = new QFile(this);
    m_pfleLog->setFileName(m_strFileName);
    if ( !m_pfleLog->open(QIODevice::Append | QIODevice::Text) ){
        qFatal("Error open log file");
        QCoreApplication::exit(1);
    }

    m_mapEnumStatus         = new QMap<int, QString>;
    m_mapEnumStatus->insert(0, QString("ERROR"));
    m_mapEnumStatus->insert(1, QString("SUCCESS"));

    m_mapEnumOperation      = new QMap<int, QString>;
    m_mapEnumOperation->insert(0, QString("OPEN"));
    m_mapEnumOperation->insert(1, QString("PUSH"));
    m_mapEnumOperation->insert(2, QString("REMOVE"));
}


//D-tor
ZBC_GB_Log::~ZBC_GB_Log()
{
    m_pfleLog->close();
    delete m_mapEnumStatus;
    delete m_mapEnumOperation;
}


//Return instance of log file (Singleton)
ZBC_GB_Log& ZBC_GB_Log::Instance()
{
    return m_logInstance;
}


//Write info to log
void ZBC_GB_Log::log(STATUS _status, OPERATION _operation, QString _sInfo)
{
    QTextStream     outStream(m_pfleLog);
    outStream << QTime::currentTime().toString();
    outStream << " ";
    outStream << m_mapEnumStatus->value(_status);
    outStream << " ";
    outStream << m_mapEnumOperation->value(_operation);
    outStream << " ";
    outStream << _sInfo;
    outStream << "\n";
}



