#include <QDebug>

#include "zbc_gb_taskitembase.h"
#include "zbc_gb_log.h"

#include <QDir>
#include <QMap>
#include <QTextStream>
#include <QVector>


// ##########
// ZBC_GB_TaskItemBase
// Settrers
void ZBC_GB_TaskItemBase::setName(const QString& _name)
{
    if (_name != "")
        m_strName = _name;
    else
        m_bGoog = false;

}

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


// Getters
QString ZBC_GB_TaskItemBase::getName() const
{ return m_strName; }
QString ZBC_GB_TaskItemBase::getPath() const
{ return m_strPath; }
QTime ZBC_GB_TaskItemBase::getStartTime() const
{ return m_tmeStartTime; }



// ##########
// ZBC_GB_TaskItemFiFo
// Setter
void ZBC_GB_TaskItemFiFo::setKeepDays(int _days)
{
    if (_days >= 0)
        m_nKeepDays = _days;
    else
        m_bGoog = false;
}

// Getter
int ZBC_GB_TaskItemFiFo::getKeepDays() const
{ return m_nKeepDays; }


// Remove files according to policy
void ZBC_GB_TaskItemFiFo::removeFiles()
{
//Create map of files for current task
    QMap<QDateTime, QString>    mDateFile;
    QDir                        dir( this->getPath() );
    dir.setNameFilters(QStringList(QString("*.zip;*.rar").split(QString(";"))));
    int                         counter = 0;
    QString                     tmpFileName;
    for(counter = 0; counter != dir.entryList().size(); ++counter)
    {
        tmpFileName = this->getPath() + dir.entryList().at(counter);
        mDateFile.insert(QFileInfo(tmpFileName).lastModified(),
                         tmpFileName);
    }

//Iterate map and remove files
    QMapIterator<QDateTime, QString> it(mDateFile);
    it.toBack();
    counter = 1;
    while( it.hasPrevious() ){
        it.previous();
        if(counter > getKeepDays()){
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



// ##########
// ZBC_GB_TaskItemGFS
// C-tor
ZBC_GB_TaskItemGFS::ZBC_GB_TaskItemGFS(QObject *pobj) : ZBC_GB_TaskItemBase(pobj){
    m_pvecKeepTime              = new QVector<unsigned>(0);

//    this->removeFiles();
}


//D-tor
ZBC_GB_TaskItemGFS::~ZBC_GB_TaskItemGFS()
{
    m_pvecKeepTime->clear();
    delete m_pvecKeepTime;
}


// Setters
void ZBC_GB_TaskItemGFS::setKeepDays(unsigned _d)
{
    m_pvecKeepTime->push_back(_d);
}
void ZBC_GB_TaskItemGFS::setKeepWeeks(unsigned _w)
{
    m_pvecKeepTime->push_back(_w);
}
void ZBC_GB_TaskItemGFS::setKeepMonthes(unsigned _m)
{
    m_pvecKeepTime->push_back(_m);
}


// Get vector of times for GFS
VecInt ZBC_GB_TaskItemGFS::getKeepTime()
{
    return *m_pvecKeepTime;
}


// Remove files according to GFS policy
void ZBC_GB_TaskItemGFS::removeFiles()
{
//List of dates of the files which must be
    QList<QDate> lstLeaveDate;
    for( int d = 0;  static_cast<unsigned>(d) != m_pvecKeepTime->at(0); ++d ){
        lstLeaveDate.push_back( QDate::currentDate().addDays(-d) );
    }

//List of files for remove
    QDir            dir( this->getPath() );
    dir.setNameFilters(QStringList(QString("*.zip;*.rar").split(QString(";"))));
    QStringList     lstRemoveFile;
    for( int counter = 0; counter != dir.entryList().size(); ++ counter ){
        if ( lstLeaveDate.indexOf(QFileInfo((this->getPath() + dir.entryList().at(counter))).lastModified().date()) == -1 )
            lstRemoveFile.push_back(this->getPath() + dir.entryList().at(counter));
    }

//Remove files and log it
    for (QString fName : lstRemoveFile)
        if ( QFile::remove(fName) )
            ZBC_GB_Log::Instance().log(ZBC_GB_Log::SUCCESS,
                                       ZBC_GB_Log::REMOVE,
                                       fName);
        else
            ZBC_GB_Log::Instance().log(ZBC_GB_Log::ERROR,
                                       ZBC_GB_Log::REMOVE,
                                       fName);
}



// ##########
// ZBC_GB_TaskVector
// C-tor
ZBC_GB_TaskVector::ZBC_GB_TaskVector(QObject* pobj) : QObject(pobj)
{
    m_pfleSettings          = new QFile(this);
    m_pvectTasks            = new QVector<ZBC_GB_TaskItemBase*>(0);
}


//D-tor
ZBC_GB_TaskVector::~ZBC_GB_TaskVector()
{
    m_pvectTasks->clear();
    delete m_pvectTasks;
}


// Open File with settings
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


// Read File and push tasks pointers into vector
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
    while ( !txtStream.atEnd() ){
        if (txtStream.readLine() == QString("FiFo")){
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
        if (txtStream.readLine() == QString("GFS"))
        {
            ZBC_GB_TaskItemGFS* pgfs = new ZBC_GB_TaskItemGFS(this);
            pgfs->setName(txtStream.readLine());
            pgfs->setPath(txtStream.readLine());
            pgfs->setKeepDays(txtStream.readLine().toInt());
            pgfs->setKeepWeeks(txtStream.readLine().toInt());
            pgfs->setKeepMonthes(txtStream.readLine().toInt());
            pgfs->setStartTime(QTime::fromString(txtStream.readLine()));
            if (pgfs->isGood())
                m_pvectTasks->push_back(pgfs);
            else
                delete pgfs;

        }
    }
}


// Get signal from timer and compare it with time of tasks
void ZBC_GB_TaskVector::checkTimeOfTasks()
{
    qDebug() << QTime::currentTime().toString();
    for( QVector<ZBC_GB_TaskItemBase*>::iterator it = m_pvectTasks->begin();
                                                 it != m_pvectTasks->end();
                                                 ++it)
        if ( (*it)->getStartTime().toString() == QTime::currentTime().toString() )
            (*it)->removeFiles();
}
