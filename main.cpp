#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << "Starting application...";
    QApplication a(argc, argv);
    qDebug() << "QApplication created";
    
    MainWindow w;
    qDebug() << "MainWindow created";
    
    w.show();
    qDebug() << "MainWindow shown";
    
    qDebug() << "Starting event loop...";
    return a.exec();
}
