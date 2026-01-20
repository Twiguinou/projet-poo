#include "MainWindow.h"

#include <QDockWidget>
#include <QLabel>
#include <QListWidget>
#include <QTableWidget>

MainWindow::MainWindow() : QMainWindow(nullptr)
{
    setObjectName("projet-poo");
    resize(1280, 720);

    auto* tableDock = new QDockWidget("Tables", this);
    tableDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    auto tableList = new QListWidget(tableDock);
    tableList->addItems(QStringList() << "caca 1" << "caca 2" << "caca 3");
    tableDock->setWidget(tableList);

    addDockWidget(Qt::LeftDockWidgetArea, tableDock);
}
