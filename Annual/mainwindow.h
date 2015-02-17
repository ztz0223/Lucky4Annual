#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <vector>

namespace Ui {
class MainWindow;
}

class Model;
class CandidataList;
class QSqlError;
class QPushButton;
class QLCDNumber;
class QString;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, std::shared_ptr<Model> r_model);
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent *event);

private:
    bool initDb();
    void initBtnLCD();
    void initConnection();
    void buttonOperation(const QString &r_type);
    void updateButtonLcd(const QString &r_type);
    void rollCandiListText();
    std::tuple<QString, QString> getLuckyText();

private slots:
    void OnImport();
    void OnExport();
    void OnQuit();
    void OnResetDb();
    void OnViewCandiList();
    void OnAbout();

    void OnClickBtnZero();
    void OnClickBtnLessThree();
    void OnClickBtnNoLessThree();
    void OnClickBtnUndefined();

private:
    enum
    {
        CountTypeA = 4,     //Married without child
        CountTypeB = 7,     //Married with child under 3
        CountTypeC = 11,    //Married with child above 3
        CountTypeD = 5,     //Single
    };

    std::unique_ptr<Ui::MainWindow> ui;
    std::shared_ptr<CandidataList> m_candiList;
    std::shared_ptr<Model> m_model;
    std::map<QString, std::pair<QPushButton *, QLCDNumber *>> type2ButtonAndLCD;
    std::map<QString, int> type2Count;

    int timer_id = -1;
    QString curType = "";
};

#endif // MAINWINDOW_H
