#include <QtWidgets/QtWidgets>

#include <Database.h>
#include <fstream>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    std::ifstream rawDb(QApplication::arguments().at(1).toStdString());
    std::unique_ptr<Database> db = Database::read(nlohmann::json::parse(rawDb));

    QWidget window;
    window.resize(1280, 720);
    window.setWindowTitle(QApplication::translate("toplevel", "projet-poo"));
    window.show();

    QHBoxLayout* paneLayout = new QHBoxLayout(&window);

    QWidget* leftPane = new QWidget();
    leftPane->show();
    paneLayout->addWidget(leftPane, 1);

    QWidget* rightPane = new QWidget();
    rightPane->show();
    paneLayout->addWidget(rightPane, 1);

    QVBoxLayout* tableLayout1 = new QVBoxLayout(leftPane);

    QTableWidget* bibliotheque = new QTableWidget(1, 3);
    bibliotheque->show();
    bibliotheque->setItem(0, 0, new QTableWidgetItem("code"));
    bibliotheque->setItem(0, 1, new QTableWidgetItem("nom"));
    bibliotheque->setItem(0, 2, new QTableWidgetItem("adresse"));
    tableLayout1->addWidget(bibliotheque, 1);

    QTableWidget* emprunte = new QTableWidget(1, 2);
    emprunte->show();
    emprunte->setItem(0, 0, new QTableWidgetItem("livre"));
    emprunte->setItem(0, 1, new QTableWidgetItem("adherent"));
    tableLayout1->addWidget(emprunte, 1);

    QTableWidget* detient = new QTableWidget(1, 2);
    detient->show();
    detient->setItem(0, 0, new QTableWidgetItem("livre"));
    detient->setItem(0, 1, new QTableWidgetItem("bibliotheque"));
    tableLayout1->addWidget(detient, 1);

    QTableWidget* prete = new QTableWidget(1, 3);
    prete->show();
    prete->setItem(0, 0, new QTableWidgetItem("livre"));
    prete->setItem(0, 1, new QTableWidgetItem("source"));
    prete->setItem(0, 2, new QTableWidgetItem("destination"));
    tableLayout1->addWidget(prete, 1);

    QVBoxLayout* tableLayout2 = new QVBoxLayout(rightPane);

    return app.exec();
}
