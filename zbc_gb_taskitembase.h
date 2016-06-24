#ifndef TASKITEMBASE_H
#define TASKITEMBASE_H

#include <QObject>
#include <QTime>

class QFile;

typedef QVector<unsigned> VecInt;

//
class ZBC_GB_TaskItemBase : public QObject
{
    Q_OBJECT
private:
    QString             m_strName;
    QString             m_strPath;
    QTime               m_tmeStartTime;

protected:
    bool                m_bGoog;

public:
    ZBC_GB_TaskItemBase(QObject* pobj = 0) : QObject(pobj), m_bGoog(true){}
    virtual ~ZBC_GB_TaskItemBase(){}

    void setName(const QString&);
    void setPath(const QString&);
    void setStartTime(const QTime&);
    virtual void removeFiles() = 0;
    QString getName() const;
    QString getPath() const;
    QTime getStartTime() const;
    bool isGood() { return m_bGoog; }
};



//
class ZBC_GB_TaskItemFiFo : public ZBC_GB_TaskItemBase
{
    Q_OBJECT
public:
    ZBC_GB_TaskItemFiFo(QObject* pobj = 0) : ZBC_GB_TaskItemBase(pobj) {}
    ~ZBC_GB_TaskItemFiFo(){}

    void setKeepDays(int);
    int getKeepDays() const;

    void removeFiles();

private:
    int                 m_nKeepDays;
};



//
class ZBC_GB_TaskItemGFS : public ZBC_GB_TaskItemBase
{
    Q_OBJECT
public:
    ZBC_GB_TaskItemGFS(QObject* pobj = 0);
    ~ZBC_GB_TaskItemGFS();

    void setKeepDays(unsigned);
    void setKeepWeeks(unsigned);
    void setKeepMonthes(unsigned);
//    void setKeepTime(int, int, int);
    VecInt  getKeepTime();

    void removeFiles();

private:
    VecInt*             m_pvecKeepTime;

};



//
class ZBC_GB_TaskVector : public QObject
{
    Q_OBJECT
public:
    ZBC_GB_TaskVector(QObject* pobj = 0);
    ~ZBC_GB_TaskVector();

private:
    QVector<ZBC_GB_TaskItemBase*>*      m_pvectTasks;
    QFile*                              m_pfleSettings;
    QString                             m_strSettingsPath;

     bool openFile();

public slots:
    void pushTasks();
    void checkTimeOfTasks();
};


#endif // TASKITEMBASE_H
