#ifndef TASKITEMBASE_H
#define TASKITEMBASE_H

#include <QObject>
#include <QTime>

class QFile;

class TaskItemBase : public QObject
{
    Q_OBJECT
public:
    TaskItemBase(QObject* pobj = 0) : QObject(pobj){}
//    TaskItemBase(const QString&,const QString&,const QTime&);
    virtual ~TaskItemBase();

//Set
    void setName(const QString&);
    void setPath(const QString&);
    void setStartTime(const QTime&);
    virtual void removeFiles() = 0;
//Get
    QString getName() const;
    QString getPath() const;
    QTime getStartTime() const;

protected:
    QString             m_strName;
    QString             m_strPath;
    QTime               m_tmeStartTime;
};



class TaskItemFiFo : public TaskItemBase
{
public:
    TaskItemFiFo(){}
//    TaskItemFiFo(const QString&, const QString&, const QTime&);
    ~TaskItemFiFo(){}

//Set
    void setKeepDays(int);
//Get
    int getKeepDays() const;

    void removeFiles();

private:
    int                 m_nKeepDays;
};



class TaskVector : public QObject
{
    Q_OBJECT
public:
    TaskVector() {}
    ~TaskVector() {}

private:
    QVector<TaskItemBase*>*     m_pvectTasks;
    QFile*                      m_pfleSettings;
    QString                     m_strSettingsPath;

     void openFile();

public slots:
    void pushTasks();
};


#endif // TASKITEMBASE_H
