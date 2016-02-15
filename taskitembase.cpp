#include "taskitembase.h"

#include <QDir>
#include <QMap>
#include <QTextStream>

//C-tor
TaskItemBase::TaskItemBase(const QString& _name, const QString& _path, const QTime& _start) :
            m_strName(_name),
            m_strPath(_path),
            m_tmeStartTime(_start){}

//D-tor
TaskItemBase::~TaskItemBase(){}


//Settrers
void TaskItemBase::setName(const QString& _name)
{ m_strName = _name; }
void TaskItemBase::setPath(const QString& _path)
{ m_strPath = _path; }
void TaskItemBase::setStartTime(const QTime& _time)
{ m_tmeStartTime = _time; }


//Getters
QString TaskItemBase::getName() const
{ return m_strName; }
QString TaskItemBase::getPath() const
{ return m_strPath; }
QTime TaskItemBase::getStartTime() const
{ return m_tmeStartTime; }



//C-tor
TaskItemFiFo::TaskItemFiFo(const QString& _name, const QString& _path, const QTime& _start):
            TaskItemBase(_name, _path, _start) {}

TaskItemFiFo::~TaskItemFiFo() {}


//Setters
void TaskItemFiFo::setKeepDays(int _days)
{ m_nKeepDays = _days; }


//Getters
int TaskItemFiFo::getKeepDays() const
{ return m_nKeepDays; }


//Remove files according to policy
void TaskItemFiFo::removeFiles()
{
//Create map of files for current task
    QMap<QDateTime, QString>    mDateFile;
    QDir                        dir( getPath() );
    dir.setNameFilters(QStringList(QString("*.zip;*.rar").split(QString(";"))));
    QStringList                 listOfFiles;
    listOfFiles = dir.entryList();
    int                         counter = 0;
    QString                     tmpFileName;
    for(counter = 0; counter != listOfFiles.size(); ++counter)
    {
        tmpFileName = getPath() + listOfFiles[counter];
        mDateFile.insert(QFileInfo(tmpFileName).lastModified(),
                         tmpFileName);
    }

//Iterate map and remove files
    QMapIterator<QDateTime, QString> it(mDateFile);
    it.toBack();
    counter = 1;
    int                         numberOfFiles = getKeepDays();
    while( it.hasPrevious() ){
        if(counter > numberOfFiles)
            QFile::remove(it.value());
        ++counter;
    }
}



//Open File with settings
void TaskVector::openFile()
{
   m_strSettingsPath = QDir::currentPath() + QString("/settings.gbs");
   m_pfleSettings =new QFile(m_strSettingsPath);
   m_pfleSettings->open(QIODevice::ReadOnly | QIODevice::Text);
}


//Read File and push tasks pointers into vector
void TaskVector::pushTasks()
{
    openFile();
    QTextStream txtStream(m_pfleSettings);
/*
    int counter = 0;
    QString     tmpName;
    QString     tmpPath;
    QString     tmpTime;
    QString     tmpType;
    while (!txtStream.atEnd())
    {
        switch(counter)
        {
        case 0:
            tmpName = Service::getStringValue(txtStream.readLine());
            ++counter;
            break;
        case 1:
            tmpPath = Service::getStringValue(txtStream.readLine());
            ++counter;
            break;
        case 2:
            tmpTime = Service::getStringValue(txtStream.readLine());
            ++counter;
            break;
        case 3:
            tmpType = Service::getStringValue(txtStream.readLine());
            ++counter;

            if(tmpType == "FiFo")
            {
//                TaskItemFiFo*   tmpTaskItemFiFo =
//                        new TaskItemFiFo(tmpName, tmpPath, tmpTime);

//                tmpTaskItemFiFo->setName(tmpName);
//                tmpTaskItemFiFo->setPath(tmpPath);
//                tmpTaskItemFiFo->setStartTime(tmpTime);
//                tmpTaskItemFiFo->setKeepDays(Service::getStringValue(txtStream.readLine()).toInt());
                break;
            }

            if(tmpType == "GFS")
            {
                break;
            }
            else
            {
//                qDebug() << "Fuck";
            }
        }
    }
*/

}
