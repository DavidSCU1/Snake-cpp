#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <iostream>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    // 在Windows下分配控制台以显示调试输出
    if (AllocConsole()) {
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
    }
#endif
    
    std::cout << "Starting application..." << std::endl;
    QApplication app(argc, argv);
    std::cout << "QApplication created successfully" << std::endl;
    qDebug() << "QApplication created successfully";



    std::cout << "Creating MainWindow..." << std::endl;
    
    // 测试Qt是否正常工作
    // QMessageBox::information(nullptr, "测试", "Qt应用程序启动成功！");
    
    MainWindow w;
    std::cout << "MainWindow created successfully" << std::endl;
    
    w.show();
    std::cout << "MainWindow shown successfully" << std::endl;
    
    std::cout << "Starting event loop..." << std::endl;
    return app.exec();
}
