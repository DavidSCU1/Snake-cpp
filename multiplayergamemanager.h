#ifndef MULTIPLAYERGAMEMANAGER_H
#define MULTIPLAYERGAMEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <deque>
#include "gamestate.h"
#include "networkmanager.h"

// 多人游戏状态结构体
struct MultiPlayerGameState {
    QMap<QString, std::deque<Point>> playerSnakes;
    QMap<QString, CharacterType> playerCharacters;
    QMap<QString, int> playerScores;
    QMap<QString, bool> playerAliveStatus;
    QMap<QString, Direction> playerDirections;
    Point foodPosition;
    Point specialFoodPosition;
    bool isSpecialFood;
    int gameSpeed;
    bool isPaused;
    QString currentTurn;
    int turnTimeLeft;
    
    MultiPlayerGameState() : isSpecialFood(false), gameSpeed(200), isPaused(false) {}
};

class MultiPlayerGameManager : public QObject
{
    Q_OBJECT

public:
    explicit MultiPlayerGameManager(QObject *parent = nullptr);
    
    // 房间管理
    QString createRoom(const QString& hostName, int maxPlayers = 4);
    bool joinRoom(const QString& roomId, const QString& playerName);
    bool leaveRoom(const QString& roomId, const QString& playerName);
    void destroyRoom(const QString& roomId);
    QStringList getAvailableRooms() const;
    GameRoom getRoomInfo(const QString& roomId) const;
    
    // 游戏控制
    bool startGame(const QString& roomId);
    void pauseGame(const QString& roomId);
    void resumeGame(const QString& roomId);
    void endGame(const QString& roomId);
    
    // 玩家操作
    void updatePlayerDirection(const QString& roomId, const QString& playerName, Direction direction);
    void updatePlayerPosition(const QString& roomId, const QString& playerName, const std::deque<Point>& snakeBody);
    void updatePlayerScore(const QString& roomId, const QString& playerName, int score);
    void setPlayerCharacter(const QString& roomId, const QString& playerName, CharacterType character);
    
    // 游戏状态
    void syncGameState(const QString& roomId);
    MultiPlayerGameState getGameState(const QString& roomId) const;
    
    // 碰撞检测
    bool checkPlayerCollision(const QString& roomId, const QString& playerName, const Point& newHead);
    bool checkFoodCollision(const QString& roomId, const Point& position);
    void generateNewFood(const QString& roomId, int gridWidth, int gridHeight);
    
    // 网络管理
    void setNetworkManager(NetworkManager* manager);
    NetworkManager* getNetworkManager() const; // 新增：获取NetworkManager实例
    void broadcastGameState(const QString& roomId);
    void sendPlayerUpdate(const QString& roomId, const QString& playerName);
    
signals:
    void roomCreated(const QString& roomId, const GameRoom& room);
    void playerJoinedRoom(const QString& roomId, const QString& playerName);
    void playerLeftRoom(const QString& roomId, const QString& playerName);
    void gameStarted(const QString& roomId);
    void gameEnded(const QString& roomId, const QString& winner);
    void gameStateUpdated(const QString& roomId, const MultiPlayerGameState& state);
    void playerCollision(const QString& roomId, const QString& playerName);
    void foodEaten(const QString& roomId, const QString& playerName, int points);
    void roomDestroyed(const QString& roomId);
    void roomListUpdated(); // 新增：房间列表更新信号
    
private slots:
    void onGameTick();
    void onPlayerInfoReceived(const QJsonObject& data, QTcpSocket* sender = nullptr);
    void onPlayerPositionReceived(const QJsonObject& data);
    void onNetworkPlayerInfoReceived(const PlayerInfo& playerInfo);
    void onNetworkPlayerPositionReceived(const QString& playerName, const std::deque<Point>& snakeBody);
    void onNetworkPlayerDisconnected(const QString& playerName);
    void onRoomListRequested(QTcpSocket* requester);
    void onRoomListReceived(const QJsonArray& roomArray); // 新增：处理收到的房间列表
    
private:
    void initializeGameState(const QString& roomId);
    void updateGameLogic(const QString& roomId);
    void checkAllCollisions(const QString& roomId);
    void updateScores(const QString& roomId);
    void checkWinCondition(const QString& roomId);
    QSet<Point> getAllOccupiedPositions(const QString& roomId) const;
    QString generateRoomId() const;
    
    // 房间列表相关
    QJsonArray getRoomListJson() const;
    void syncRoomList();
    
    QMap<QString, GameRoom> rooms;
    QMap<QString, MultiPlayerGameState> gameStates;
    NetworkManager* networkManager;
    QTimer* gameTimer;
    
    static const int GRID_WIDTH = 40;
    static const int GRID_HEIGHT = 30;
};

#endif // MULTIPLAYERGAMEMANAGER_H