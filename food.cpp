#include "food.h"
#include <QRandomGenerator>
#include <QPixmap>
#include <QPainter>

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
    // 尝试加载海绵宝宝主题的食物图片
    normalFoodPixmap = QPixmap(":/images/krabby_patty.png");
    specialFoodPixmap = QPixmap(":/images/golden_spatula.png");
    
    // 如果资源文件不存在，创建默认食物
    if (normalFoodPixmap.isNull()) {
        normalFoodPixmap = QPixmap(20, 20);
        QPainter painter(&normalFoodPixmap);
        painter.fillRect(0, 0, 20, 20, Qt::red);
        painter.setPen(Qt::darkRed);
        painter.drawEllipse(2, 2, 16, 16);
    }
    
    if (specialFoodPixmap.isNull()) {
        specialFoodPixmap = QPixmap(20, 20);
        QPainter painter(&specialFoodPixmap);
        painter.fillRect(0, 0, 20, 20, Qt::yellow);
        painter.setPen(Qt::darkYellow);
        painter.drawEllipse(2, 2, 16, 16);
        painter.setPen(Qt::red);
        painter.drawText(6, 14, "★");
    }
}