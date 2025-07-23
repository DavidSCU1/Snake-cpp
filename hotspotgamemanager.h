#ifndef HOTSPOTGAMEMANAGER_H
#define HOTSPOTGAMEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <deque>
#include "gamestate.h"
#include "hotspotnetworkmanager.h"

/**
 * 热点游戏状态结构体
 * 简化的游戏状态管理，专为热点网络优化
 */
struct HotspotGameState {
    QMap<QString, std::deque<Point>> playerSnakes;
    QMap<QString, CharacterType> playerCharacters;
    QMap<QString, int> playerScores;
    QMap<QString, bool> playerAliveStatus;
    QMap<QString, Direction> playerDirections;
    QMap<QString, bool> playerReadyStatus;
    Point foodPosition;
    Point specialFoodPosition;
    bool isSpecialFood;
    int gameSpeed;
    bool isPaused;
    bool isGameStarted;
    QString gameWinner;
    int countdownTimer;
    
    HotspotGameState() 
        : isSpecialFood(false)
        , gameSpeed(200)
        , isPaused(false)
        , isGameStarted(false)
        , countdownTimer(0) {}
};

/**
 * 热点多人游戏管理器
 * 专为热点网络环境设计的简化游戏管理器
 * 特点：
 * 1. 简化的房间管理
 * 2. 优化的游戏同步
 * 3. 更好的网络适应性
 * 4. 减少网络延迟影响
 */
class HotspotGameManager : public QObject
{
    Q_OBJECT

public:
    explicit HotspotGameManager(QObject *parent = nullptr);
    ~HotspotGameManager();
    
    // 游戏房间管理
    bool createRoom(const QString& hostPlayerName, const QString& roomName);
    bool joinRoom(const QString& playerName);
    void leaveRoom(const QString& playerName);
    void destroyRoom();
    
    // 游戏控制
    bool startGame();
    void pauseGame();
    void resumeGame();
    void endGame(const QString& winner = QString());
    void resetGame();
    
    // 玩家管理
    void setPlayerCharacter(const QString& playerName, CharacterType character);
    void setPlayerReady(const QString& playerName, bool ready);
    void updatePlayerDirection(const QString& playerName, Direction direction);
    void removePlayer(const QString& playerName);
    
    // 游戏状态
    HotspotGameState getGameState() const { return gameState; }
    bool isHost() const;
    bool isGameActive() const { return gameState.isGameStarted && !gameState.isPaused; }
    int getPlayerCount() const { return gameState.playerSnakes.size(); }
    QStringList getPlayerNames() const { return gameState.playerSnakes.keys(); }
    
    // 网络管理
    void setNetworkManager(HotspotNetworkManager* manager);
    HotspotNetworkManager* getNetworkManager() const { return networkManager; }
    
    // 游戏配置
    void setGameSpeed(int speed) { gameState.gameSpeed = speed; }
    int getGameSpeed() const { return gameState.gameSpeed; }
    
signals:
    // 房间事件
    void roomCreated(const QString& roomName);
    void playerJoined(const QString& playerName);
    void playerLeft(const QString& playerName);
    void roomDestroyed();
    
    // 游戏事件
    void gameStarted();
    void gameEnded(const QString& winner);
    void gamePaused();
    void gameResumed();
    void gameReset();
    
    // 玩家事件
    void playerCharacterChanged(const QString& playerName, CharacterType character);
    void playerReadyChanged(const QString& playerName, bool ready);
    void playerScoreChanged(const QString& playerName, int score);
    void playerDied(const QString& playerName);
    
    // 游戏状态事件
    void gameStateUpdated(const HotspotGameState& state);
    void foodEaten(const QString& playerName, int points);
    void countdownUpdated(int seconds);
    
private slots:
    void onGameTick();
    void onCountdownTick();
    void onNetworkPlayerData(const QString& playerName, const QJsonObject& playerData);
    void onNetworkGameState(const QJsonObject& gameState);
    void onNetworkPlayerConnected(const QString& playerName);
    void onNetworkPlayerDisconnected(const QString& playerName);
    
private:
    void initializeGame();
    void updateGameLogic();
    void checkCollisions();
    void updatePlayerPositions();
    void generateFood();
    void checkWinCondition();
    void broadcastGameState();
    void syncPlayerData(const QString& playerName);
    
    // 碰撞检测
    bool checkSelfCollision(const QString& playerName);
    bool checkWallCollision(const QString& playerName);
    bool checkPlayerCollision(const QString& playerName);
    bool checkFoodCollision(const QString& playerName);
    
    // 游戏逻辑辅助
    Point getNextHeadPosition(const QString& playerName);
    void growSnake(const QString& playerName);
    void killPlayer(const QString& playerName);
    QSet<Point> getAllOccupiedPositions() const;
    Point generateRandomFoodPosition();
    
    // 数据序列化
    QJsonObject gameStateToJson() const;
    void gameStateFromJson(const QJsonObject& json);
    QJsonObject playerDataToJson(const QString& playerName) const;
    void playerDataFromJson(const QString& playerName, const QJsonObject& json);
    
    // 成员变量
    HotspotGameState gameState;
    HotspotNetworkManager* networkManager;
    QTimer* gameTimer;
    QTimer* countdownTimer;
    QString hostPlayerName;
    QString roomName;
    
    // 游戏配置
    static const int GRID_WIDTH = 40;
    static const int GRID_HEIGHT = 30;
    static const int INITIAL_SNAKE_LENGTH = 3;
    static const int COUNTDOWN_SECONDS = 3;
    static const int FOOD_POINTS = 10;
    static const int SPECIAL_FOOD_POINTS = 50;
};

#endif // HOTSPOTGAMEMANAGER_H