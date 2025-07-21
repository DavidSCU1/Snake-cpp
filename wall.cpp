#include "wall.h"
#include <QQueue>
#include <QDebug>

Wall::Wall()
{
}

void Wall::generateWalls(int gridWidth, int gridHeight, const QSet<Point>& occupiedPositions)
{
    clear();
    
    // 创建蛇前方禁区（蛇初始位置在中心，面向右）
    QSet<Point> forbiddenArea;
    Point snakeStart(gridWidth / 2, gridHeight / 2);
    for (int i = 1; i <= 10; ++i) {
        Point forbiddenPos(snakeStart.x + i, snakeStart.y);
        if (forbiddenPos.x < gridWidth) {
            forbiddenArea.insert(forbiddenPos);
        }
    }
    
    // 目标墙块数量100-150，通过生成墙体段来实现
    int targetWallCount = QRandomGenerator::global()->bounded(100, 151);
    int generatedWalls = 0;
    int maxSegments = 50; // 最大段数
    int segmentAttempts = 0;
    
    while (generatedWalls < targetWallCount && segmentAttempts < maxSegments) {
        QVector<Point> segment = generateWallSegment(gridWidth, gridHeight, occupiedPositions, forbiddenArea);
        
        // 检查新墙体段是否会创建封闭区域
        if (!segment.isEmpty() && !wouldCreateEnclosure(segment, gridWidth, gridHeight)) {
            for (const Point& pos : segment) {
                wallPositions.insert(pos);
                generatedWalls++;
                if (generatedWalls >= targetWallCount) break;
            }
        }
        
        segmentAttempts++;
    }
    
    qDebug() << "Generated" << wallPositions.size() << "wall blocks (target:" << targetWallCount << ")";
}

bool Wall::hasWallAt(const Point& position) const
{
    return wallPositions.contains(position);
}

void Wall::clear()
{
    wallPositions.clear();
}

QVector<Point> Wall::generateWallSegment(int gridWidth, int gridHeight, const QSet<Point>& occupiedPositions, const QSet<Point>& forbiddenArea)
{
    QVector<Point> segment;
    
    // 随机选择起始位置（避开边界、已占用位置和禁区）
    Point start;
    int attempts = 0;
    do {
        start.x = QRandomGenerator::global()->bounded(2, gridWidth - 2);
        start.y = QRandomGenerator::global()->bounded(2, gridHeight - 2);
        attempts++;
    } while ((occupiedPositions.contains(start) || wallPositions.contains(start) || forbiddenArea.contains(start)) && attempts < 100);
    
    if (attempts >= 100) {
        return segment; // 无法找到合适位置
    }
    
    segment.append(start);
    
    // 生成3-8个连续的墙体块，确保墙体不单独出现
    int segmentLength = QRandomGenerator::global()->bounded(3, 9);
    Point current = start;
    
    for (int i = 1; i < segmentLength; ++i) {
        QVector<Point> adjacentPositions = getAdjacentPositions(current);
        
        // 过滤掉无效位置（包括禁区）
        QVector<Point> validPositions;
        for (const Point& pos : adjacentPositions) {
            if (pos.x >= 1 && pos.x < gridWidth - 1 && 
                pos.y >= 1 && pos.y < gridHeight - 1 &&
                !occupiedPositions.contains(pos) && 
                !wallPositions.contains(pos) &&
                !forbiddenArea.contains(pos) &&
                !segment.contains(pos)) {
                validPositions.append(pos);
            }
        }
        
        if (validPositions.isEmpty()) {
            break; // 无法继续扩展
        }
        
        // 随机选择一个有效位置
        int index = QRandomGenerator::global()->bounded(validPositions.size());
        current = validPositions[index];
        segment.append(current);
    }
    
    return segment;
}

bool Wall::wouldCreateEnclosure(const QVector<Point>& newWalls, int gridWidth, int gridHeight) const
{
    // 创建临时障碍物集合
    QSet<Point> allObstacles = wallPositions;
    for (const Point& wall : newWalls) {
        allObstacles.insert(wall);
    }
    
    // 检查从左上角到右下角是否仍然连通
    Point start(0, 0);
    Point end(gridWidth - 1, gridHeight - 1);
    
    return !isAreaReachable(start, end, gridWidth, gridHeight, allObstacles);
}

bool Wall::isAreaReachable(const Point& start, const Point& end, int gridWidth, int gridHeight, const QSet<Point>& obstacles) const
{
    if (obstacles.contains(start) || obstacles.contains(end)) {
        return false;
    }
    
    QSet<Point> visited;
    QQueue<Point> queue;
    queue.enqueue(start);
    visited.insert(start);
    
    while (!queue.isEmpty()) {
        Point current = queue.dequeue();
        
        if (current == end) {
            return true;
        }
        
        QVector<Point> neighbors = getAdjacentPositions(current);
        for (const Point& neighbor : neighbors) {
            if (neighbor.x >= 0 && neighbor.x < gridWidth &&
                neighbor.y >= 0 && neighbor.y < gridHeight &&
                !obstacles.contains(neighbor) &&
                !visited.contains(neighbor)) {
                visited.insert(neighbor);
                queue.enqueue(neighbor);
            }
        }
    }
    
    return false;
}

QVector<Point> Wall::getAdjacentPositions(const Point& pos) const
{
    QVector<Point> adjacent;
    adjacent.append(Point(pos.x + 1, pos.y));     // 右
    adjacent.append(Point(pos.x - 1, pos.y));     // 左
    adjacent.append(Point(pos.x, pos.y + 1));     // 下
    adjacent.append(Point(pos.x, pos.y - 1));     // 上
    return adjacent;
}