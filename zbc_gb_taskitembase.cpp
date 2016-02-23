#include <QDebug>

#include "zbc_gb_taskitembase.h"
#include "zbc_gb_log.h"

#include <QDir>
#include <QMap>
#include <QTextStream>
#include <QVector>


//Settrers
void ZBC_GB_TaskItemBase::setName(const QString& _name)
{ m_strName = _name; }

void ZBC_GB_TaskItemBase::setPath(const QString& _path)
{
    if (QDir(_path).exists())
        m_strPath =  _path;
    else
        m_bGoog = false;
}

void ZBC_GB_TaskItemBase::setStartTime(const QTime& _time)
{
    if (_time.isValid())
        m_tmeStartTime = _time;
    else
        m_bGoog = false;
}


//Getters
QString ZBC_GB_TaskItemBase::getName() const
{ return m_strName; }
QString ZBC_GB_TaskItemBase::getPath() const
{ return m_strPath; }
QTime ZBC_GB_TaskItemBase::getStartTime() const
{ return m_tmeStartTime; }


//Setters
void ZBC_GB_TaskItemFiFo::setKeepDays(int _days)
{ m_nKeepDays = _days; }


//Getters
int ZBC_GB_TaskItemFiFo::getKeepDays() const
{ return m_nKeepDays; }


//Remove files according to policy
void ZBC_GB_TaskItemFiFo::removeFiles()
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
        it.previous();
        if(counter > numberOfFiles){
            if ( QFile::remove(it.value()) )
                ZBC_GB_Log::Instance().log(ZBC_GB_Log::SUCCESS,
                                           ZBC_GB_Log::REMOVE,
                                           it.value());
            else
                ZBC_GB_Log::Instance().log(ZBC_GB_Log::ERROR,
                                           ZBC_GB_Log::REMOVE,
                                           it.value());
        }
        ++counter;
    }
}


//C-tor
ZBC_GB_TaskVector::ZBC_GB_TaskVector(QObject* pobj) : QObject(pobj)
{
    m_pfleSettings          = new QFile(this);
    m_pvectTasks            = new QVector<ZBC_GB_TaskItemBase*>;
}


//D-tor
ZBC_GB_TaskVector::~ZBC_GB_TaskVector()
{
    m_pvectTasks->clear();
    delete m_pvectTasks;
}


//Open File with settings
bool ZBC_GB_TaskVector::openFile()
{
    m_strSettingsPath = QDir::currentPath() + QString("/settings.gbs");
    if (m_pfleSettings->isOpen())
        m_pfleSettings->close();
    m_pfleSettings->setFileName(m_strSettingsPath);
        if( m_pfleSettings->open(QIODevice::ReadOnly | QIODevice::Text)){
            ZBC_GB_Log::Instance().log(ZBC_GB_Log::SUCCESS,
                                       ZBC_GB_Log::OPEN,
                                       QDir::toNativeSeparators(m_strSettingsPath));
            return true;
        }
        else{
            ZBC_GB_Log::Instance().log(ZBC_GB_Log::ERROR,
                                       ZBC_GB_Log::OPEN,
                                       QDir::toNativeSeparators(m_strSettingsPath));
            return false;
        }
}


//Read File and push tasks pointers into vector
void ZBC_GB_TaskVector::pushTasks()
{
    if ( !openFile() ){
        ZBC_GB_Log::Instance().log(ZBC_GB_Log::ERROR,
                                   ZBC_GB_Log::PUSH,
                                   QString(" task"));
        return;
    }
    if (!m_pvectTasks->isEmpty())
        m_pvectTasks->clear();
    QTextStream txtStream(m_pfleSettings);
    QString tmpType;
    while ( !txtStream.atEnd() ){
        tmpType = txtStream.readLine();
        if ( tmpType == QString("FiFo") ){
            ZBC_GB_TaskItemFiFo* pfifo = new ZBC_GB_TaskItemFiFo(this);
            pfifo->setName(txtStream.readLine());
            pfifo->setPath(txtStream.readLine());
            pfifo->setKeepDays(txtStream.readLine().toInt());
            pfifo->setStartTime(QTime::fromString(txtStream.readLine()));
            if (pfifo->isGood())
                m_pvectTasks->push_back(pfifo);
            else
                delete pfifo;
        }
    }
}


//get signal from timer and compare it with time of tasks
void ZBC_GB_TaskVector::checkTimeOfTasks()
{
    qDebug() << QTime::currentTime().toString();
    for( QVector<ZBC_GB_TaskItemBase*>::iterator it = m_pvectTasks->begin();
                                                 it != m_pvectTasks->end();
                                                 ++it)
        if ( (*it)->getStartTime().toString() == QTime::currentTime().toString() )
            (*it)->removeFiles();
}
