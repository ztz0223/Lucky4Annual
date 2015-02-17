#ifndef DBOPR
#define DBOPR

#include <QSqlQuery>

class QSqlError;
class QString;

class DbOpr
{
public:
    static QSqlError initDb();
    static void resetDb();
    static QSqlError addCandidate(const QString &r_name, const QString &r_id, const QString &r_type);

public:
    bool iterNextItem(QString &r_name, QString &r_id, QString &r_type, bool &lucky);
    bool exec(const QString &sqlStr);

private:
    QSqlQuery query;
};



#endif // DBOPR

