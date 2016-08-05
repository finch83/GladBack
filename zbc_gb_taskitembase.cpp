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
//Set of dates of the files which must be...
    QSet<QDate> setLeaveDate;
    int nCounterDWM;
//Dayly...
    for( nCounterDWM = 0;  static_cast<unsigned>(nCounterDWM) != m_pvecKeepTime->at(0); ++nCounterDWM )
        setLeaveDate.insert(QDate::currentDate().addDays(-nCounterDWM));
//Weekly...
    const short WEEK_DAYS = 7;
    if (m_pvecKeepTime->at(1) > 0)
        for( nCounterDWM = 0; static_cast<unsigned>(nCounterDWM) != m_pvecKeepTime->at(1); ++nCounterDWM )
            setLeaveDate.insert( QDate::currentDate().addDays( -QDate::currentDate().dayOfWeek() - WEEK_DAYS * nCounterDWM ) );
//Monthly...
    if (m_pvecKeepTime->at(2) > 0)
        for( nCounterDWM = 0; static_cast<unsigned>(nCounterDWM) != m_pvecKeepTime->at(2); ++nCounterDWM )
            setLeaveDate.insert( QDate(QDate::currentDate().year() ,QDate::currentDate().month(), 1).addMonths(-nCounterDWM) );

//List of files for remove
    QDir            dir( this->getPath() );
    dir.setNameFilters(QStringList(QString("*.zip;*.rar").split(QString(";"))));
    for( int counter = 0; counter != dir.entryList().size(); ++ counter )
        if (!setLeaveDate.contains(QFileInfo((this->getPath() + dir.entryList().at(counter))).lastModified().date())){
            if ( QFile::remove( this->getPath() + dir.entryList().at(counter) ) )
                ZBC_GB_Log::Instance().log(ZBC_GB_Log::SUCCESS,
                                           ZBC_GB_Log::REMOVE,
                                           this->getPath() + dir.entryList().at(counter));
            else
                ZBC_GB_Log::Instance().log(ZBC_GB_Log::ERROR,
                                           ZBC_GB_Log::REMOVE,
                                           this->getPath() + dir.entryList().at(counter));
        }
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
    QString strType;
    while ( !txtStream.atEnd() ){
        strType = txtStream.readLine();
        if (strType == QString("FiFo")){
            ZBC_GB_TaskItemFiFo* pfifo = new ZBC_GB_TaskItemFiFo(this);
            pfifo->setName(txtStream.readLine());
            pfifo->setPath(txtStream.readLine());
            pfifo->setKeepDays(txtStream.readLine().toInt());
            pfifo->setStartTime(QTime::fromString(txtStream.readLine()));
            if (pfifo->isGood()){
                m_pvectTasks->push_back(pfifo);
                ZBC_GB_Log::Instance().log(ZBC_GB_Log::SUCCESS,
                                           ZBC_GB_Log::PUSH,
                                           pfifo->getName());
            }
            else{
                delete pfifo;
                ZBC_GB_Log::Instance().log(ZBC_GB_Log::ERROR,
                                           ZBC_GB_Log::PUSH,
                                           pfifo->getName());
            }
        }
        if (strType == QString("GFS"))
        {
            ZBC_GB_TaskItemGFS* pgfs = new ZBC_GB_TaskItemGFS(this);
            pgfs->setName(txtStream.readLine());
            pgfs->setPath(txtStream.readLine());
            pgfs->setKeepDays(txtStream.readLine().toInt());
            pgfs->setKeepWeeks(txtStream.readLine().toInt());
            pgfs->setKeepMonthes(txtStream.readLine().toInt());
            pgfs->setStartTime(QTime::fromString(txtStream.readLine()));
            if (pgfs->isGood()){
                m_pvectTasks->push_back(pgfs);
                ZBC_GB_Log::Instance().log(ZBC_GB_Log::SUCCESS,
                                           ZBC_GB_Log::PUSH,
                                           pgfs->getName());
            }
            else{
                delete pgfs;
                ZBC_GB_Log::Instance().log(ZBC_GB_Log::ERROR,
                                           ZBC_GB_Log::PUSH,
                                           pgfs->getName());
            }
        }
    }
}


// Get signal from timer and compare it with time of tasks
void ZBC_GB_TaskVector::checkTimeOfTasks()
{
//    ZBC_GB_Log::Instance().log(ZBC_GB_Log::SUCCESS,
//                               ZBC_GB_Log::PUSH,
//                               QLatin1String("checkTimeOfTasks"));

//    qDebug() << QTime::currentTime().toString();
/*
    ZBC_GB_Log::Instance().log(ZBC_GB_Log::SUCCESS,
                               ZBC_GB_Log::PUSH,
                               QTime::currentTime().toString());
*/

    for( QVector<ZBC_GB_TaskItemBase*>::iterator it = m_pvectTasks->begin();
                                                 it != m_pvectTasks->end();
                                                 ++it)
        if ( (*it)->getStartTime().toString() == QTime::currentTime().toString() )
            (*it)->removeFiles();
}
