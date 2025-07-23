#include <QApplication>
#include <QPixmap>
#include <QDebug>
#include <QDir>
#include <QResource>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Testing resource loading...";
    
    // 列出所有可用的资源
    QDir resourceDir(":/");
    qDebug() << "Available resources in /:" << resourceDir.entryList();
    
    QDir imagesDir(":/images");
    qDebug() << "Available resources in /images:" << imagesDir.entryList();
    
    // 测试加载海洋背景图片
    QPixmap oceanBg;
    bool loaded = oceanBg.load(":/images/ocean-background.png");
    qDebug() << "Ocean background loaded:" << loaded;
    if (loaded) {
        qDebug() << "Image size:" << oceanBg.size();
    }
    
    // 测试加载其他已知存在的资源
    QPixmap testSvg;
    bool svgLoaded = testSvg.load(":/images/spongebob_head.svg");
    qDebug() << "SVG loaded:" << svgLoaded;
    
    return 0;
}