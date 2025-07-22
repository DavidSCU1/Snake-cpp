#ifndef WALL_H
#define WALL_H

#include <QSet>
#include <QVector>
#include <QPoint>
#include <QRandomGenerator>
#include "gamestate.h"

class Wall
{
public:
    Wall();
    
    // 生成墙体
    void generateWalls(int gridWidth, int gridHeight, const QSet<Point>& occupiedPositions);
    
    // 生成指定数量的墙块（用于挑战模式）
    void generateChallengeWalls(int count, int gridWidth, int gridHeight, const QSet<Point>& occupiedPositions);
    
    // 检查位置是否有墙
    bool hasWallAt(const Point& position) const;
    
    // 获取所有墙体位置
    const QSet<Point>& getWallPositions() const { return wallPositions; }
    
    // 清空墙体
    void clear();
    
private:
    QSet<Point> wallPositions;
    
    // 生成连续的墙体段
    QVector<Point> generateWallSegment(int gridWidth, int gridHeight, const QSet<Point>& occupiedPositions, const QSet<Point>& forbiddenArea);
    
    // 检查墙体是否会围成封闭区域
    bool wouldCreateEnclosure(const QVector<Point>& newWalls, int gridWidth, int gridHeight) const;
    
    // 使用洪水填充算法检查连通性
    bool isAreaReachable(const Point& start, const Point& end, int gridWidth, int gridHeight, const QSet<Point>& obstacles) const;
    
    // 获取相邻位置
    QVector<Point> getAdjacentPositions(const Point& pos) const;
    
    // 检查在指定位置放置墙体是否会违反密度规则
    // 规则：任何空格子的上下左右四个相邻格子中不能有三个或更多的墙
    bool wouldViolateDensityRule(const Point& newWallPos, int gridWidth, int gridHeight) const;
    
    // 检查整个网格是否存在违反密度规则的空格子
    bool hasViolatingEmptySpaces(int gridWidth, int gridHeight) const;
};

#endif // WALL_H