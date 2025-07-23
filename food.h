#ifndef FOOD_H
#define FOOD_H

#include <QtCore>
#include <QObject>
#include <QPixmap>
#include <QTimer>
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
    
    void setPosition(const Point& pos) { position = pos; }
    void setSpecial(bool isSpecial) { special = isSpecial; value = isSpecial ? 20 : 10; }
    
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

#endif // FOOD_H