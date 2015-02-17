#include "mainwindow.h"
#include "candidatalist.h"
#include "model.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication::addLibraryPath("./");
    QApplication::setStyle(QStyleFactory::create("gtk+"));
    QApplication a(argc, argv);
    a.setApplicationDisplayName("Annual");

    std::shared_ptr<CandidataList> listViewInst = std::make_shared<CandidataList>(nullptr);
    std::shared_ptr<Model> modelInst = std::make_shared<Model>(listViewInst);

    MainWindow w(nullptr, modelInst);
    w.show();

    return a.exec();
}
