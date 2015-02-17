#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <set>
#include <unordered_set>
#include <map>
#include <QString>
#include <QHash>

class CandidataList;
class QSqlError;

class Model
{
public:
    Model(std::shared_ptr<CandidataList> &r_listView);

    void resetView();
    bool initDb();
    void initCandiList();
    void resetDb();

    bool addCandidate(const QString &r_name, const QString &r_id, const QString &r_type);
    bool luckCandidate(const QString &r_name, const QString &r_id, const QString &r_type);
    bool getNextCandidate(const QString &r_type, QString &r_name, QString &r_id);
    int  getLuckyCandCount(const QString &r_type);
    int  getAllCandCount(const QString &r_type);
    void importCandiList();
    void exportLucky();
    void displayListView();

private:
    bool getCandListBroken() { return m_candListBroken; }
    void setCandListBroken(bool r_broken) { m_candListBroken = r_broken; }

private:
    bool m_candListBroken = false;

private:
    struct item
    {
        QString name;
        QString id;
        QString type;
        bool lucky;
        item(const QString &r_name, const QString &r_id, const QString &r_type, bool r_lucky)
            :name(r_name), id(r_id), type(r_type), lucky(r_lucky)
        {}

        bool operator == (const item &r) const
        {
            return id == r.id;
        }

    };

    struct itemHash
    {
        size_t operator ()(const item &r_item) const
        {
            return qHash(r_item.id);
        }
    };

    std::shared_ptr<CandidataList> m_listView;
    std::map<QString, std::pair<std::unordered_set<item, itemHash>, std::unordered_set<item, itemHash>>> m_mapAllCandidate;
};

#endif // MODEL_H
