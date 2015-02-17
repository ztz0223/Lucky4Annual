#include "candidatalist.h"
#include "ui_candidatalist.h"
#include "dbopr.h"

#include <QSqlDatabase>
#include <QCloseEvent>
#include <QSqlRecord>

CandidataList::CandidataList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CandidataList)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);

    initTableWidget(ui->m_tableWidgetAll);
    initTableWidget(ui->m_tableWidgetLucky);

    connect(ui->m_lineEditFind, &QLineEdit::returnPressed, this, &CandidataList::find);
    connect(ui->m_pushButtonFind, &QPushButton::clicked, this, &CandidataList::find);
}

CandidataList::~CandidataList()
{
}

void CandidataList::delAllTableWidgetItem()
{
    for(int i = ui->m_tableWidgetAll->rowCount() - 1; i >= 0 ; i--)
        ui->m_tableWidgetAll->removeRow(i);

    for(int i = ui->m_tableWidgetLucky->rowCount() - 1; i >= 0 ; i--)
        ui->m_tableWidgetLucky->removeRow(i);
}

void CandidataList::closeEvent(QCloseEvent *event)
{
    delAllTableWidgetItem();
    event->accept();
}

void CandidataList::initTableWidget(QTableWidget *tableWidget)
{
    QStringList strList{"Name", "ID", "Type", "Lucky"};
    tableWidget->setColumnCount(AttriCount);
    tableWidget->setHorizontalHeaderLabels(strList);
}

void CandidataList::appendTableWidgetItem(QTableWidget *tableWidget, const candiListItem &candItem)
{
    int rowCount = tableWidget->rowCount();
    tableWidget->insertRow(rowCount);

    for(int i = 0; i < std::min<int>(tableWidget->columnCount(), candItem.dataInStr.size()); i++)
    {
        tableWidget->setItem(rowCount, i, new QTableWidgetItem(candItem.dataInStr[i]));
    }
}

void CandidataList::insertNewCandidate(const QString &r_name, const QString &r_id, const QString &r_type, const int r_lucky)
{
    appendTableWidgetItem(ui->m_tableWidgetAll, candiListItem(r_name, r_id, r_type, r_lucky));
}

void CandidataList::find()
{    
    QString findText = ui->m_lineEditFind->text();

    if(findText.isEmpty())
    {
        return;
    }

    QTableWidget *tableWidgt = nullptr;
    QWidget *wdgt = ui->m_tabWidgetCandidate->currentWidget();
    if(wdgt == ui->m_candidateAll)
    {
        tableWidgt = ui->m_tableWidgetAll;
    }
    else
    {
        tableWidgt = ui->m_tableWidgetLucky;
    }

    int index = -1;
    int rowCount = tableWidgt->rowCount();
    if(rowCount <= 0)
    {
        return;
    }

    for(int i  = 0; i < rowCount; i++)
    {
        if(tableWidgt->item(i, 0)->text().indexOf(findText, 0, Qt::CaseInsensitive) != -1 ||
           tableWidgt->item(i, 1)->text().indexOf(findText, 0, Qt::CaseInsensitive) != -1 )
        {
            index = i;
            break;
        }
    }

    if(index == -1)
    {
        return;
    }
    tableWidgt->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidgt->selectRow(index);
    tableWidgt->scrollToItem(tableWidgt->item(index, 0));
}

void CandidataList::insertNewLuckyCandidate(const QString &r_name, const QString &r_id, const QString &r_type, const int r_lucky)
{
    appendTableWidgetItem(ui->m_tableWidgetLucky, candiListItem(r_name, r_id, r_type, r_lucky));
}

QStringList CandidataList::genLuckyList()
{
    QStringList strList;

    int rowCount = ui->m_tableWidgetLucky->rowCount();

    for(int i  = 0; i < rowCount; i++)
    {
        QString str;
        for(int j = 0; j < AttriCount; j++)
        {
            auto t = ui->m_tableWidgetLucky->item(i, j);
            if(t == nullptr)
                break;

            if(!str.isEmpty())
            {
                str += ',';
            }
            str += t->text();
        }

        if(!str.isEmpty())
        {
            strList.append(str);
        }
    }

    return strList;
}

void CandidataList::display()
{
    this->show();
}
