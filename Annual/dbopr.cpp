
#include "dbopr.h"

#include <QtSql>
#include <QMessageBox>
#include <QSqlDatabase>

QSqlError DbOpr::initDb()
{
    QSqlDatabase db(QSqlDatabase::addDatabase("QSQLITE"));
    db.setDatabaseName("Annual.db");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("CandidateList", Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(QLatin1String("create table if not exists CandidateList(id integer primary key, "
                              "name varchar, "
                              "work_id varchar unique, "
                              "type varchar, "
                              "status INTEGER default 0 "
                              ")")))
        return q.lastError();

    return QSqlError();
}

void DbOpr::resetDb()
{
    QSqlQuery q;
    if (!q.exec(QLatin1String("update CandidateList set status = 0 where status is not 0")))
    {
        QMessageBox::warning(nullptr, "Del Db Err...", q.lastError().nativeErrorCode() );
    }
}

QSqlError DbOpr::addCandidate(const QString &r_name, const QString &r_id, const QString &r_type)
{
    QSqlQuery q;

    if (!q.prepare(QLatin1String("insert into CandidateList(name, work_id, type) values(?, ?, ?)")))
        return q.lastError();

    q.addBindValue(r_name);
    q.addBindValue(r_id);
    q.addBindValue(r_type);
    q.exec();

    return QSqlError();
}

bool DbOpr::exec(const QString &sqlStr)
{
    return query.exec(sqlStr);
}

bool DbOpr::iterNextItem(QString &r_name, QString &r_id, QString &r_type, bool &r_lucky)
{
    int nameNo = query.record().indexOf("name");
    int idNo = query.record().indexOf("work_id");
    int typeNo = query.record().indexOf("type");
    int luckyNo = query.record().indexOf("status");

    while (query.next())
    {
        r_name = query.value(nameNo).toString();
        r_id = query.value(idNo).toString();
        r_type = query.value(typeNo).toString();
        r_lucky = query.value(luckyNo).toInt();

        return true;
    }

    return false;
}
