#include "food.h"
#include <QRandomGenerator>
#include <QPixmap>
#include <QPainter>
#include <QSvgRenderer>
#include <QDebug>

Food::Food(QObject *parent)
    : QObject(parent)
    , position(0, 0)
    , special(false)
    , value(10)
    , timer(new QTimer(this))
{
    loadFoodPixmaps();
    
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &Food::onTimeout);
}

void Food::generateFood(int width, int height, const QSet<Point>& occupiedPositions)
{
    Point newPos;
    int attempts = 0;
    const int maxAttempts = width * height;
    
    do {
        newPos.x = QRandomGenerator::global()->bounded(width);
        newPos.y = QRandomGenerator::global()->bounded(height);
        attempts++;
    } while (occupiedPositions.contains(newPos) && attempts < maxAttempts);
    
    position = newPos;
    special = false;
    value = 10;
    stopTimer();
}

void Food::generateSpecialFood(int width, int height, const QSet<Point>& occupiedPositions)
{
    Point newPos;
    int attempts = 0;
    const int maxAttempts = width * height;
    
    do {
        newPos.x = QRandomGenerator::global()->bounded(width);
        newPos.y = QRandomGenerator::global()->bounded(height);
        attempts++;
    } while (occupiedPositions.contains(newPos) && attempts < maxAttempts);
    
    position = newPos;
    special = true;
    value = 50;
    
    // 特殊食物5秒后消失
    startTimer();
}

QPixmap Food::getPixmap() const
{
    return special ? specialFoodPixmap : normalFoodPixmap;
}

bool Food::isExpired() const
{
    return special && !timer->isActive();
}

void Food::startTimer()
{
    if (special) {
        timer->start(5000); // 5秒
    }
}

void Food::stopTimer()
{
    timer->stop();
}

void Food::onTimeout()
{
    if (special) {
        emit foodExpired();
    }
}

void Food::loadFoodPixmaps()
{
    // 使用QSvgRenderer加载SVG文件
    QSvgRenderer normalRenderer;
    QSvgRenderer specialRenderer;
    normalRenderer.load(QString(":/images/krabby_patty.svg"));
    specialRenderer.load(QString(":/images/golden_spatula.svg"));
    
    if (normalRenderer.isValid()) {
        qDebug() << "Successfully loaded normal food SVG";
        normalFoodPixmap = QPixmap(20, 20);
        normalFoodPixmap.fill(Qt::transparent);
        QPainter painter(&normalFoodPixmap);
        normalRenderer.render(&painter);
    } else {
        qDebug() << "Failed to load normal food SVG";
        normalFoodPixmap = QPixmap(20, 20);
        QPainter painter(&normalFoodPixmap);
        painter.fillRect(0, 0, 20, 20, Qt::red);
        painter.setPen(Qt::darkRed);
        painter.drawEllipse(2, 2, 16, 16);
    }
    
    if (specialRenderer.isValid()) {
        qDebug() << "Successfully loaded special food SVG";
        specialFoodPixmap = QPixmap(20, 20);
        specialFoodPixmap.fill(Qt::transparent);
        QPainter painter(&specialFoodPixmap);
        specialRenderer.render(&painter);
    } else {
        qDebug() << "Failed to load special food SVG";
        specialFoodPixmap = QPixmap(20, 20);
        QPainter painter(&specialFoodPixmap);
        painter.fillRect(0, 0, 20, 20, Qt::yellow);
        painter.setPen(Qt::darkYellow);
        painter.drawEllipse(2, 2, 16, 16);
        painter.setPen(Qt::red);
        painter.drawText(6, 14, "★");
    }
}