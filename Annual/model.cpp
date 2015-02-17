#include "model.h"
#include "dbopr.h"
#include "candidatalist.h"

#include <QtGlobal>
#include <QString>
#include <QSqlError>
#include <QMessageBox>
#include <QTime>
#include <QFileDialog>
#include <QFile>

Model::Model(std::shared_ptr<CandidataList> &r_listView)
    :m_listView(r_listView)
{
}

bool Model::initDb()
{
    if(DbOpr::initDb().isValid())
    {
        return false;
    }

    initCandiList();
    return true;
}

void Model::resetDb()
{
    DbOpr::resetDb();
    initCandiList();
}

bool Model::addCandidate(const QString &r_name, const QString &r_id, const QString &r_type)
{
    QSqlError sqlErr = DbOpr::addCandidate(r_name, r_id, r_type);
    if(sqlErr.isValid())
    {
        QMessageBox::warning(nullptr, "Adding candidate failed...", sqlErr.nativeErrorCode());
        return false;
    }

    //m_listView->insertNewCandidate(r_name, r_id, r_type, false);
    m_mapAllCandidate[r_type].first.insert(item(r_name, r_id, r_type, false));

    return true;
}

bool Model::luckCandidate(const QString &r_name, const QString &r_id, const QString &r_type)
{
    //m_listView->insertNewLuckyCandidate(r_name, r_id, r_type, true);
    //m_listView->deleteCandidate(r_name, r_id);

    QString str("update CandidateList set status = 1 where name = '");
    str += r_name;
    str += "' and work_id = '";
    str += r_id + "'";

    if(!DbOpr().exec(str))
    {
        return false;
    }

    m_mapAllCandidate[r_type].first.erase(item(r_name, r_id, r_type, false));
    m_mapAllCandidate[r_type].second.insert(item(r_name, r_id, r_type, true));

    setCandListBroken(true);

    return true;
}

bool Model::getNextCandidate(const QString &r_type, QString &r_name, QString &r_id)
{
    static auto iter = m_mapAllCandidate[r_type].first.cbegin();
    static QString lastType = "";

    size_t size = m_mapAllCandidate[r_type].first.size();

    if(size == 0)
    {
        return false;
    }

    if(lastType.isEmpty() || lastType != r_type || getCandListBroken())
    {
        iter = m_mapAllCandidate[r_type].first.cbegin();
        qsrand(QTime::currentTime().elapsed());
        std::advance(iter, qrand() % size);

        lastType = r_type;
        setCandListBroken(false);
    }

    if(iter == m_mapAllCandidate[r_type].first.cend())
    {
        iter = m_mapAllCandidate[r_type].first.cbegin();
    }

    r_name = iter->name;
    r_id = iter->id;

    iter++;
    return true;
}

int Model::getLuckyCandCount(const QString &r_type)
{
    return m_mapAllCandidate[r_type].second.size();
}

int Model::getAllCandCount(const QString &r_type)
{
    return m_mapAllCandidate[r_type].first.size();
}

void Model::importCandiList()
{
    auto fileName = QFileDialog::getOpenFileName(nullptr, "Import...", "./", "Excel Files (*.csv)");
    QFile csvFile(fileName);

    if (!csvFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    if(!csvFile.isOpen())
    {
        QMessageBox::warning(nullptr, "File...", "File Cant be Opened!");
    }

    while(!csvFile.atEnd())
    {
        QString str = csvFile.readLine();
        if(str.count(',') < 2)
        {
            continue;
        }

        auto splitStr = str.split(',');
        addCandidate(splitStr.at(0).trimmed(), splitStr.at(1).trimmed(), splitStr.at(2).trimmed());
    }
    csvFile.close();
}

void Model::exportLucky()
{
    auto fileName = QFileDialog::getSaveFileName(nullptr, "Export...", "./", "Excel Files (*.csv)");
    auto strList = m_listView->genLuckyList();
    QFile csvFile(fileName);

    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    if(!csvFile.isOpen())
    {
        QMessageBox::warning(nullptr, "File...", "File Cant be Opened!");
    }

    if(strList.size())
    {
        for(auto i : strList)
        {
            csvFile.write(i.toStdString().c_str());
        }
    }
    csvFile.close();
}

void Model::displayListView()
{
    resetView();
    m_listView->display();
}

void Model::initCandiList()
{
    DbOpr opr;

    if(!opr.exec("SELECT * FROM CandidateList"))
    {
        return;
    }

    for(auto &i : m_mapAllCandidate)
    {
        i.second.first.clear();
        i.second.second.clear();
    }

    QString name;
    QString id;
    QString type;
    bool lucky;

    while (opr.iterNextItem(name, id, type, lucky))
    {
        if(lucky)
        {
            m_mapAllCandidate[type].second.insert(item(name, id, type, lucky));
        }
        else
        {
            m_mapAllCandidate[type].first.insert(item(name, id, type, lucky));
        }
    }
}

void Model::resetView()
{
    DbOpr opr;

    if(!opr.exec("SELECT * FROM CandidateList"))
    {
        return;
    }

    QString name;
    QString id;
    QString type;
    bool lucky;

    m_listView->delAllTableWidgetItem();
    while (opr.iterNextItem(name, id, type, lucky))
    {
        m_listView->insertNewCandidate(name, id, type, lucky);
        if(lucky)
        {
            m_listView->insertNewLuckyCandidate(name, id, type, lucky);
        }
    }
}
