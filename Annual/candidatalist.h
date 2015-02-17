#ifndef CANDIDATALIST_H
#define CANDIDATALIST_H

#include <QWidget>
#include <memory>

namespace Ui {
class CandidataList;
}

class QTableWidget;
class QStringList;
class QCloseEvent;
class CandidataList : public QWidget
{
    Q_OBJECT

public:
    struct candiListItem
    {
        std::vector<QString> dataInStr;

        candiListItem() = default;
        candiListItem(const QString &r_name, const QString &r_id, const QString &r_type, const bool r_luck)
            :dataInStr{std::move(r_name), std::move(r_id), std::move(r_type), getLuckyStr(r_luck)}
        {}

        QString getLuckyStr(const bool luck)
        {
            return luck ? "Yes" : "No";
        }
    };

    enum
    {
        AttriCount = 4,
    };

public:
    explicit CandidataList(QWidget *parent = 0);
    ~CandidataList();
    void insertNewCandidate(const QString &r_name, const QString &r_id, const QString &r_type, const int r_lucky);
    void insertNewLuckyCandidate(const QString &r_name, const QString &r_id, const QString &r_type, const int r_lucky);
    QStringList genLuckyList();
    void delAllTableWidgetItem();
    void display();

protected:
    virtual void closeEvent(QCloseEvent * event);

private:
    void initTableWidget(QTableWidget *tableWidget);
    void appendTableWidgetItem(QTableWidget *tableWidget, const candiListItem &candItem);

private slots:
    void find();

private:
    std::unique_ptr<Ui::CandidataList> ui;
};

#endif // CANDIDATALIST_H
