#ifndef ZBC_GB_LOG_H
#define ZBC_GB_LOG_H

#include <QObject>

class QFile;

class ZBC_GB_Log : public QObject
{
    Q_OBJECT
private:
    QFile*              m_pfleLog;
public:
    explicit ZBC_GB_Log(QObject *parent = 0);

signals:

public slots:
};

#endif // ZBC_GB_LOG_H
