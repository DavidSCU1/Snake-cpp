#include <QApplication>
#include <QPixmap>
#include <QDebug>
#include <QDir>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    std::cout << "Testing resource loading..." << std::endl;
    
    // 测试加载海洋背景图片
    QPixmap oceanBg;
    bool loaded = oceanBg.load(":/images/ocean-background.png");
    std::cout << "Ocean background loaded: " << (loaded ? "YES" : "NO") << std::endl;
    
    if (loaded) {
        std::cout << "Image size: " << oceanBg.width() << "x" << oceanBg.height() << std::endl;
    } else {
        std::cout << "Failed to load ocean-background.png" << std::endl;
        
        // 列出可用资源
        QDir resourceDir(":/");
        auto entries = resourceDir.entryList();
        std::cout << "Available resources in /: ";
        for (const auto& entry : entries) {
            std::cout << entry.toStdString() << " ";
        }
        std::cout << std::endl;
        
        QDir imagesDir(":/images");
        auto imageEntries = imagesDir.entryList();
        std::cout << "Available resources in /images: ";
        for (const auto& entry : imageEntries) {
            std::cout << entry.toStdString() << " ";
        }
        std::cout << std::endl;
    }
    
    // 测试加载已知存在的SVG文件
    QPixmap testSvg;
    bool svgLoaded = testSvg.load(":/images/spongebob_head.svg");
    std::cout << "SVG loaded: " << (svgLoaded ? "YES" : "NO") << std::endl;
    
    return 0;
}