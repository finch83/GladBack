#ifndef ZBC_GB_LOG_H
#define ZBC_GB_LOG_H

#include <QObject>

class QFile;

typedef QMap<int, QString> MapIntStr;

class ZBC_GB_Log : public QObject
{
    Q_OBJECT
private:
    static const QString        m_strFileName;
    static ZBC_GB_Log           m_logInstance;
    QFile*                      m_pfleLog;
    MapIntStr*                  m_mapEnumStatus;
    MapIntStr*                  m_mapEnumOperation;

    ZBC_GB_Log(QObject *parent = 0);
    ~ZBC_GB_Log();

public:
    enum STATUS{ ERROR, SUCCESS };
    enum OPERATION{ OPEN, PUSH, REMOVE };

    static ZBC_GB_Log& Instance();
    void log(STATUS, OPERATION, QString);
};

#endif // ZBC_GB_LOG_H
