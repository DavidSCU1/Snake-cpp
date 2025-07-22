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
    
    // 使用更严格的逐个放置策略
    int targetWallCount = QRandomGenerator::global()->bounded(80, 121); // 降低目标数量以确保质量
    int maxAttempts = 2000; // 增加尝试次数
    
    for (int attempt = 0; attempt < maxAttempts && wallPositions.size() < targetWallCount; ++attempt) {
        // 随机选择一个位置
        Point candidate;
        candidate.x = QRandomGenerator::global()->bounded(2, gridWidth - 2);
        candidate.y = QRandomGenerator::global()->bounded(2, gridHeight - 2);
        
        // 检查位置是否可用
        if (occupiedPositions.contains(candidate) || 
            wallPositions.contains(candidate) || 
            forbiddenArea.contains(candidate)) {
            continue;
        }
        
        // 检查是否会违反密度规则
        if (wouldViolateDensityRule(candidate, gridWidth, gridHeight)) {
            continue;
        }
        
        // 临时添加这个墙体，检查是否会创建封闭区域
        QVector<Point> singleWall;
        singleWall.append(candidate);
        if (wouldCreateEnclosure(singleWall, gridWidth, gridHeight)) {
            continue;
        }
        
        // 添加墙体
        wallPositions.insert(candidate);
    }
    
    // 最终验证：检查是否存在违反密度规则的空格子
    if (hasViolatingEmptySpaces(gridWidth, gridHeight)) {
        qDebug() << "Warning: Still found violating empty spaces after generation!";
    } else {
        qDebug() << "Wall generation completed successfully - no density violations found.";
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
    
    // 随机选择起始位置（避开边界、已占用位置、禁区和违反密度规则的位置）
    Point start;
    int attempts = 0;
    do {
        start.x = QRandomGenerator::global()->bounded(2, gridWidth - 2);
        start.y = QRandomGenerator::global()->bounded(2, gridHeight - 2);
        attempts++;
    } while ((occupiedPositions.contains(start) || wallPositions.contains(start) || forbiddenArea.contains(start) || wouldViolateDensityRule(start, gridWidth, gridHeight)) && attempts < 100);
    
    if (attempts >= 100) {
        return segment; // 无法找到合适位置
    }
    
    segment.append(start);
    
    // 生成3-8个连续的墙体块，确保墙体不单独出现
    int segmentLength = QRandomGenerator::global()->bounded(3, 9);
    Point current = start;
    
    for (int i = 1; i < segmentLength; ++i) {
        QVector<Point> adjacentPositions = getAdjacentPositions(current);
        
        // 过滤掉无效位置（包括禁区和违反密度规则的位置）
        QVector<Point> validPositions;
        for (const Point& pos : adjacentPositions) {
            if (pos.x >= 1 && pos.x < gridWidth - 1 && 
                pos.y >= 1 && pos.y < gridHeight - 1 &&
                !occupiedPositions.contains(pos) && 
                !wallPositions.contains(pos) &&
                !forbiddenArea.contains(pos) &&
                !segment.contains(pos) &&
                !wouldViolateDensityRule(pos, gridWidth, gridHeight)) {
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

bool Wall::hasViolatingEmptySpaces(int gridWidth, int gridHeight) const
{
    // 检查网格中的每个空格子
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            Point currentPos(x, y);
            
            // 如果当前位置不是墙体，检查其周围的墙体密度
            if (!wallPositions.contains(currentPos)) {
                QVector<Point> neighbors = getAdjacentPositions(currentPos);
                int wallCount = 0;
                
                for (const Point& neighbor : neighbors) {
                    // 检查邻居是否在网格范围内且是墙体
                    if (neighbor.x >= 0 && neighbor.x < gridWidth && 
                        neighbor.y >= 0 && neighbor.y < gridHeight && 
                        wallPositions.contains(neighbor)) {
                        wallCount++;
                    }
                }
                
                // 如果周围有3个或更多墙体，违反规则
                if (wallCount >= 3) {
                    qDebug() << "Found violating empty space at (" << x << "," << y << ") with" << wallCount << "surrounding walls";
                    return true;
                }
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

bool Wall::wouldViolateDensityRule(const Point& newWallPos, int gridWidth, int gridHeight) const
{
    // 创建临时墙体集合，包含新墙体位置
    QSet<Point> tempWalls = wallPositions;
    tempWalls.insert(newWallPos);
    
    // 需要检查的空格子范围：新墙体周围2格范围内的所有空格子
    QSet<Point> spacesToCheck;
    
    // 检查新墙体周围2格范围内的所有位置
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dy = -2; dy <= 2; ++dy) {
            Point checkPos(newWallPos.x + dx, newWallPos.y + dy);
            
            // 确保位置在网格范围内且不是墙体
            if (checkPos.x >= 0 && checkPos.x < gridWidth && 
                checkPos.y >= 0 && checkPos.y < gridHeight && 
                !tempWalls.contains(checkPos)) {
                spacesToCheck.insert(checkPos);
            }
        }
    }
    
    // 检查每个空格子周围的墙体密度
    for (const Point& emptySpace : spacesToCheck) {
        QVector<Point> neighbors = getAdjacentPositions(emptySpace);
        int wallCount = 0;
        
        for (const Point& neighbor : neighbors) {
            // 检查邻居是否在网格范围内且是墙体
            if (neighbor.x >= 0 && neighbor.x < gridWidth && 
                neighbor.y >= 0 && neighbor.y < gridHeight && 
                tempWalls.contains(neighbor)) {
                wallCount++;
            }
        }
        
        // 如果周围有3个或更多墙体，违反规则
        if (wallCount >= 3) {
            return true;
        }
    }
    
    return false;
}