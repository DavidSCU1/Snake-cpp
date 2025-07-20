#ifndef FOOD_H
#define FOOD_H

#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <QSet>
#include "gamestate.h"

class Food : public QObject
{
    Q_OBJECT

public:
    explicit Food(QObject *parent = nullptr);
    
    void generateFood(int width, int height, const QSet<Point>& occupiedPositions);
    void generateSpecialFood(int width, int height, const QSet<Point>& occupiedPositions);
    
    Point getPosition() const { return position; }
    bool isSpecial() const { return special; }
    int getValue() const { return value; }
    
    QPixmap getPixmap() const;
    
    bool isExpired() const;
    void startTimer();
    void stopTimer();
    
signals:
    void foodExpired();
    
private slots:
    void onTimeout();
    
private:
    Point position;
    bool special;
    int value;
    QTimer* timer;
    
    QPixmap normalFoodPixmap;
    QPixmap specialFoodPixmap;
    
    void loadFoodPixmaps();
};

// 为Point类型添加qHash函数支持
inline size_t qHash(const Point& point, size_t seed = 0)
{
    return qHash(static_cast<int>(point.x), seed) ^ qHash(static_cast<int>(point.y), seed << 1);
}

#endif // FOOD_H