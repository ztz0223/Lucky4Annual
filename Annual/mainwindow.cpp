/*
 * GPL V3
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "candidatalist.h"
#include "about.h"
#include "dbopr.h"
#include "model.h"

#include <utility>
#include <QStringList>
#include <QMessageBox>
#include <QStyleFactory>
#include <Qtimer>

namespace
{
    const QString typeA = "A";
    const QString typeB = "B";
    const QString typeC = "C";
    const QString typeD = "D";
}

MainWindow::MainWindow(QWidget *parent, std::shared_ptr<Model> r_model) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_model(r_model)
{
    setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);
    ui->m_centralWidget->setStyleSheet("#m_centralWidget {"
                                       "background-image: url(:/resource/20155.jpg);"
                                       "background-repeat: no-repeat;"
                                       "background-position: center;"
                                       "}");

    initConnection();
    initDb();
    initBtnLCD();
}

MainWindow::~MainWindow()
{}

void MainWindow::initConnection()
{
    connect(ui->actionImport,               &QAction::triggered,   this, &MainWindow::OnImport);
    connect(ui->actionExport,               &QAction::triggered,   this, &MainWindow::OnExport);
    connect(ui->actionQuit,                 &QAction::triggered,   this, &MainWindow::OnQuit);
    connect(ui->actionReset_Db,             &QAction::triggered,   this, &MainWindow::OnResetDb);
    connect(ui->actionCandidataList,        &QAction::triggered,   this, &MainWindow::OnViewCandiList);
    connect(ui->actionAbout,                &QAction::triggered,   this, &MainWindow::OnAbout);
    connect(ui->m_pushButtonLessZero,       &QPushButton::clicked, this, &MainWindow::OnClickBtnZero);
    connect(ui->m_pushButtonLessThree,      &QPushButton::clicked, this, &MainWindow::OnClickBtnLessThree);
    connect(ui->m_pushButtonNoLessThree,    &QPushButton::clicked, this, &MainWindow::OnClickBtnNoLessThree);
    connect(ui->m_pushButtonUndefined,      &QPushButton::clicked, this, &MainWindow::OnClickBtnUndefined);
}

bool MainWindow::initDb()
{
    return m_model->initDb();
}

void MainWindow::initBtnLCD()
{
    type2ButtonAndLCD.clear();
    type2ButtonAndLCD[typeA] = {ui->m_pushButtonLessZero,    ui->m_lcdNumberZero};
    type2ButtonAndLCD[typeB] = {ui->m_pushButtonLessThree,   ui->m_lcdNumberLessThree};
    type2ButtonAndLCD[typeC] = {ui->m_pushButtonNoLessThree, ui->m_lcdNumberNoLessThree};
    type2ButtonAndLCD[typeD] = {ui->m_pushButtonUndefined,   ui->m_lcdNumberUndefined};

    type2Count.clear();
    type2Count[typeA] = CountTypeA;
    type2Count[typeB] = CountTypeB;
    type2Count[typeC] = CountTypeC;
    type2Count[typeD] = CountTypeD;

    std::for_each(type2ButtonAndLCD.begin(), type2ButtonAndLCD.end(), [this](std::pair<const QString, std::pair<QPushButton *, QLCDNumber *>> &pair)
    {
        pair.second.first->setEnabled(true);

        pair.second.second->setSegmentStyle(QLCDNumber::Flat);
        pair.second.second->setDecMode();
        pair.second.second->setStyleSheet("color: black;"
                                          "background-color: yellow;");

        updateButtonLcd(pair.first);
    } );
}

void MainWindow::OnImport()
{
    m_model->importCandiList();
}

void MainWindow::OnExport()
{
    m_model->exportLucky();
}

void MainWindow::OnQuit()
{
    this->close();
}

void MainWindow::OnViewCandiList()
{
    m_model->displayListView();
}

void MainWindow::OnResetDb()
{
    m_model->resetDb();
    initBtnLCD();
}

void MainWindow::OnAbout()
{
    About about(this);

    about.setModal(true);
    about.exec();
}

void MainWindow::OnClickBtnZero()
{
    buttonOperation(typeA);
}

void MainWindow::OnClickBtnLessThree()
{
    buttonOperation(typeB);
}

void MainWindow::OnClickBtnNoLessThree()
{
    buttonOperation(typeC);
}

void MainWindow::OnClickBtnUndefined()
{
    buttonOperation(typeD);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(timer_id != event->timerId())
        return;

    rollCandiListText();
}

void MainWindow::buttonOperation(const QString &r_type)
{
    static bool curState = false;
    static QString buttonText;

    if(!curState && m_model->getAllCandCount(r_type) == 0)
    {
        QMessageBox::warning(nullptr, "Error ...", "No any candidates!");
        return;
    }

    curType = r_type;

    auto typeList = {typeA, typeB, typeC, typeD};
    for(auto iter : typeList)
    {
        if(iter != r_type && m_model->getLuckyCandCount(iter) < type2Count[iter])
        {
            type2ButtonAndLCD[iter].first->setEnabled(curState);
        }
    }

    if(curState) //stop running of candi list, and get lucky candidate
    {
        killTimer(timer_id);

        //notify model
        QString name, id;
        std::tie(name, id) = getLuckyText();

        if(name.isEmpty() || id.isEmpty())
        {
            QMessageBox::warning(nullptr, "Error ...", "Get lucky list failed!");
            return;
        }

        if(!m_model->luckCandidate(name, id, r_type))
        {
            QMessageBox::warning(nullptr, "Error ...", "Update lucky list failed!");
            return;
        }

        type2ButtonAndLCD[r_type].first->setText(buttonText);
        updateButtonLcd(r_type);
    }
    else //start the running
    {
        buttonText = type2ButtonAndLCD[r_type].first->text();
        type2ButtonAndLCD[r_type].first->setText("Stop");
        timer_id = startTimer(4);
    }

    curState = !curState;
}

void MainWindow::updateButtonLcd(const QString &r_type)
{
    int luckyCandiCount = m_model->getLuckyCandCount(r_type);
    type2ButtonAndLCD[r_type].second->display(type2Count[r_type] - luckyCandiCount);

    //check the button status
    if(m_model->getLuckyCandCount(r_type) >= type2Count[r_type])
    {
        type2ButtonAndLCD[r_type].first->setEnabled(false);
    }
}

//for timer
void MainWindow::rollCandiListText()
{
    QString name;
    QString id;
    QString type = curType;
    if(m_model->getNextCandidate(type, name, id))
    {
        ui->m_labelCandidate->setText(name + "   " + id);
    }
}

std::tuple<QString, QString> MainWindow::getLuckyText()
{
    QString name;
    QString id;
    QString str = ui->m_labelCandidate->text();
    QStringList strList = str.split("   ");
    if(strList.size() == 2)
    {
        name = strList[0];
        id = strList[1];
    }

    return std::make_tuple(name, id);
}
