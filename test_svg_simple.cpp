#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QSvgRenderer>
#include <QDebug>
#include <QPixmap>

class TestWidget : public QWidget {
public:
    TestWidget() {
        setFixedSize(400, 300);
        setWindowTitle("SVG Test");
        
        // 测试加载SVG
        QSvgRenderer renderer;
        bool loaded = renderer.load(QString(":/images/spongebob_head.svg"));
        
        qDebug() << "SVG loading test:" << loaded;
        qDebug() << "SVG is valid:" << renderer.isValid();
        
        if (loaded && renderer.isValid()) {
            testPixmap = QPixmap(100, 100);
            testPixmap.fill(Qt::transparent);
            QPainter painter(&testPixmap);
            renderer.render(&painter);
            qDebug() << "SVG rendered to pixmap";
        } else {
            testPixmap = QPixmap(100, 100);
            testPixmap.fill(Qt::red);
            qDebug() << "Using fallback red square";
        }
    }
    
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::white);
        painter.drawPixmap(50, 50, testPixmap);
        painter.drawText(10, 20, "SVG Test - Check console for debug output");
    }
    
private:
    QPixmap testPixmap;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    TestWidget widget;
    widget.show();
    
    return app.exec();
}