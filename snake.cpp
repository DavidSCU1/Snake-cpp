#include "snake.h"
#include <QPixmap>
#include <QDebug>
#include <QSvgRenderer>
#include <QPainter>

Snake::Snake(QObject *parent)
    : QObject(parent)
    , currentDirection(Direction::RIGHT)
    , nextDirection(Direction::RIGHT)
    , character(CharacterType::SPONGEBOB)
{
    loadCharacterPixmaps();
}

void Snake::setCharacter(CharacterType character)
{
    this->character = character;
    loadCharacterPixmaps();
}

void Snake::reset(const Point& startPos)
{
    body.clear();
    body.push_back(startPos);
    body.push_back(Point(startPos.x - 1, startPos.y));
    body.push_back(Point(startPos.x - 2, startPos.y));
    
    currentDirection = Direction::RIGHT;
    nextDirection = Direction::RIGHT;
}

void Snake::move()
{
    // 检查蛇身是否为空
    if (body.empty()) {
        return;
    }
    
    // 更新当前方向
    if (canChangeDirection(nextDirection)) {
        currentDirection = nextDirection;
    }
    
    Point head = getHead();
    Point newHead = head;
    
    // 根据当前方向计算新的头部位置
    switch (currentDirection) {
    case Direction::UP:
        newHead.y--;
        break;
    case Direction::DOWN:
        newHead.y++;
        break;
    case Direction::LEFT:
        newHead.x--;
        break;
    case Direction::RIGHT:
        newHead.x++;
        break;
    }
    
    // 移动蛇身
    body.push_front(newHead);
    if (!body.empty()) {
        body.pop_back();
    }
}

void Snake::grow()
{
    if (!body.empty()) {
        Point tail = body.back();
        body.push_back(tail);
    }
}

void Snake::setDirection(Direction dir)
{
    // 只有在可以改变方向时才设置新方向
    if (canChangeDirection(dir)) {
        nextDirection = dir;
    }
}

bool Snake::checkSelfCollision() const
{
    if (body.size() < 4) return false;
    
    Point head = getHead();
    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        if (head == *it) {
            return true;
        }
    }
    return false;
}

bool Snake::checkBoundaryCollision(int width, int height) const
{
    Point head = getHead();
    return head.x < 0 || head.x >= width || head.y < 0 || head.y >= height;
}

void Snake::loadCharacterPixmaps()
{
    QString basePath = ":/images/";
    QString characterName;
    
    switch (character) {
    case CharacterType::SPONGEBOB:
        characterName = "spongebob";
        break;
    case CharacterType::PATRICK:
        characterName = "patrick";
        break;
    case CharacterType::SQUIDWARD:
        characterName = "squidward";
        break;
    case CharacterType::SANDY:
        characterName = "sandy";
        break;
    case CharacterType::MR_KRABS:
        characterName = "mrcrabs";
        break;
    case CharacterType::PLANKTON:
        characterName = "plankton";
        break;
    }
    
    // 使用QSvgRenderer加载SVG文件
    QString headPath = basePath + characterName + "_head.svg";
    QString bodyPath = basePath + characterName + "_body.svg";
    
    QSvgRenderer headRenderer;
    QSvgRenderer bodyRenderer;
    headRenderer.load(QString(headPath));
    bodyRenderer.load(QString(bodyPath));
    
    // 使用统一的大小（20x20像素，与cellSize相同）
    const int pixmapSize = 20;
    
    if (headRenderer.isValid()) {
        qDebug() << "Successfully loaded head SVG:" << headPath;
        headPixmap = QPixmap(pixmapSize, pixmapSize);
        headPixmap.fill(Qt::transparent);
        QPainter painter(&headPixmap);
        headRenderer.render(&painter);
    } else {
        qDebug() << "Failed to load head SVG:" << headPath;
        headPixmap = QPixmap(pixmapSize, pixmapSize);
        headPixmap.fill(Qt::darkGreen);
    }

    if (bodyRenderer.isValid()) {
        qDebug() << "Successfully loaded body SVG:" << bodyPath;
        bodyPixmap = QPixmap(pixmapSize, pixmapSize);
        bodyPixmap.fill(Qt::transparent);
        QPainter painter(&bodyPixmap);
        bodyRenderer.render(&painter);
    } else {
        qDebug() << "Failed to load body SVG:" << bodyPath;
        bodyPixmap = QPixmap(pixmapSize, pixmapSize);
        bodyPixmap.fill(Qt::green);
    }
}

bool Snake::canChangeDirection(Direction newDir) const
{
    switch (currentDirection) {
    case Direction::UP:
        return newDir != Direction::DOWN;
    case Direction::DOWN:
        return newDir != Direction::UP;
    case Direction::LEFT:
        return newDir != Direction::RIGHT;
    case Direction::RIGHT:
        return newDir != Direction::LEFT;
    }
    return true;
}