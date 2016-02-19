#ifndef ZBC_GB_CORE_H
#define ZBC_GB_CORE_H

#include <QObject>

class QTimer;
class ZBC_GB_TaskVector;

class ZBC_GB_Core : public QObject
{
    Q_OBJECT
private:
    QTimer*                     m_pTimer;
    ZBC_GB_TaskVector*          m_pTaskVector;

public:
    explicit ZBC_GB_Core(QObject *parent = 0);

signals:

public slots:
    void run();
};

#endif // ZBC_GB_CORE_H
