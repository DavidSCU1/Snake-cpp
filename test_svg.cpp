#include <QApplication>
#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    // 测试加载SVG文件
    QPixmap headPixmap(":/images/spongebob_head.svg");
    QPixmap bodyPixmap(":/images/spongebob_body.svg");
    QPixmap foodPixmap(":/images/krabby_patty.svg");
    
    qDebug() << "Head pixmap null:" << headPixmap.isNull();
    qDebug() << "Body pixmap null:" << bodyPixmap.isNull();
    qDebug() << "Food pixmap null:" << foodPixmap.isNull();
    
    if (!headPixmap.isNull()) {
        QLabel *headLabel = new QLabel();
        headLabel->setPixmap(headPixmap.scaled(50, 50, Qt::KeepAspectRatio));
        layout->addWidget(headLabel);
    }
    
    if (!bodyPixmap.isNull()) {
        QLabel *bodyLabel = new QLabel();
        bodyLabel->setPixmap(bodyPixmap.scaled(50, 50, Qt::KeepAspectRatio));
        layout->addWidget(bodyLabel);
    }
    
    if (!foodPixmap.isNull()) {
        QLabel *foodLabel = new QLabel();
        foodLabel->setPixmap(foodPixmap.scaled(50, 50, Qt::KeepAspectRatio));
        layout->addWidget(foodLabel);
    }
    
    window.show();
    return app.exec();
}