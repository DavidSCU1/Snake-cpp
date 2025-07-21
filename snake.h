#ifndef SNAKE_H
#define SNAKE_H

#include <QObject>
#include <QPixmap>
#include <deque>
#include "gamestate.h"

class Snake : public QObject
{
    Q_OBJECT

public:
    explicit Snake(QObject *parent = nullptr);
    
    void setCharacter(CharacterType character);
    void setBody(const std::deque<Point>& newBody) { body = newBody; }
    void reset(const Point& startPos);
    void move();
    void grow();
    void setDirection(Direction dir);
    
    bool checkSelfCollision() const;
    bool checkBoundaryCollision(int width, int height) const;
    
    const std::deque<Point>& getBody() const { return body; }
    Point getHead() const { return body.empty() ? Point(0, 0) : body.front(); }
    Direction getDirection() const { return currentDirection; }
    CharacterType getCharacter() const { return character; }
    
    QPixmap getHeadPixmap() const { return headPixmap; }
    QPixmap getBodyPixmap() const { return bodyPixmap; }
    
    int getLength() const { return body.size(); }
    bool canChangeDirection(Direction newDir) const;
    
private:
    void loadCharacterPixmaps();
    
    std::deque<Point> body;
    Direction currentDirection;
    Direction nextDirection;
    CharacterType character;
    
    QPixmap headPixmap;
    QPixmap bodyPixmap;
};

#endif // SNAKE_H