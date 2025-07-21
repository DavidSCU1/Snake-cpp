#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QSettings>
#include <QSet>
#include <QMap>
#include "gamestate.h"
#include "snake.h"
#include "food.h"
#include "wall.h"
#include "networkmanager.h"
#include "multiplayergamemanager.h"
#include "singleplayergamemanager.h"


class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();
    
    void setCharacter(CharacterType character);
    void setDifficulty(Difficulty difficulty);
    void setSinglePlayerGameMode(SinglePlayerMode mode);
    void startSinglePlayerGame();
    void startMultiPlayerGame(bool isHost = false);
    void startLocalCoopGame();
    void setLocalCoopMode(CharacterType player1Character, CharacterType player2Character);
    void createRoom(const QString& playerName, int maxPlayers = 4);
    void joinRoom(const QString& roomId, const QString& playerName);
    void leaveRoom();
    
    // 多人游戏管理器访问方法
    MultiPlayerGameManager* getMultiPlayerManager() const { return multiPlayerManager; }
    void setMultiPlayerManager(MultiPlayerGameManager* manager);
    void setCurrentRoomId(const QString& roomId) { currentRoomId = roomId; }
    void setPlayerName(const QString& name) { playerName = name; }
    void pauseGame();
    void resumeGame();
    void resetGame();
    
    GameState getCurrentState() const { return currentState; }
    int getCurrentScore() const { return score; }
    
signals:
    void gameOver(int finalScore);
    void backToMenu();
    void scoreChanged(int newScore);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    
private slots:
    void gameLoop();
    void onFoodExpired();
    void onPlayerConnected(const QString& playerName);
    void onPlayerDisconnected(const QString& playerName);
    void onPlayerInfoReceived(const PlayerInfo& playerInfo);
    void onScoreUpdateReceived(const QString& playerName, int score);
    void onPlayerPositionReceived(const QString& playerName, const std::deque<Point>& snakeBody);
    void onNetworkError(const QString& error);
    
    // 多人游戏管理器槽函数
    void onRoomCreated(const QString& roomId, const GameRoom& room);
    void onPlayerJoinedRoom(const QString& roomId, const QString& playerName);
    void onPlayerLeftRoom(const QString& roomId, const QString& playerName);
    void onGameStarted(const QString& roomId);
    void onGameEnded(const QString& roomId, const QString& winner);
    void onGameStateUpdated(const QString& roomId, const MultiPlayerGameState& gameState);
    void onPlayerCollision(const QString& roomId, const QString& playerName);
    void onFoodEaten(const QString& roomId, const QString& playerName, int points);
    void updateCountdown();  // 更新倒计时
    
private:
    void setupUI();
    void setupGame();
    void updateGameArea();
    void updateButtonPositions();
    void generateFood();
    void generateSpecialFood();
    void generateWalls();
    void checkCollisions();
    void checkLocalCoopCollisions();
    void updateScore(int points);
    void saveHighScore();
    void loadHighScores();
    void updateSpeed();
    void drawGrid(QPainter& painter, const QRect& gameRect);
    void drawSnake(QPainter& painter, const QRect& gameRect);
    void drawLocalCoopSnakes(QPainter& painter, const QRect& gameRect);
    void drawFood(QPainter& painter, const QRect& gameRect);
    void drawWalls(QPainter& painter, const QRect& gameRect);
    void drawMultiplayerSnakes(QPainter& painter, const QRect& gameRect);
    void drawUI(QPainter& painter);
    void drawPauseOverlay(QPainter& painter, const QRect& gameRect);
    void drawGameOverOverlay(QPainter& painter, const QRect& gameRect);

    
    QSet<Point> getOccupiedPositions() const;
    void sendNetworkUpdate();
    
    // UI 组件
    QVBoxLayout* mainLayout;
    QHBoxLayout* gameLayout;
    QWidget* gameArea;
    QWidget* sidePanel;
    QLabel* scoreLabel;
    QLabel* levelLabel;
    QLabel* playersLabel;
    QLabel* timeLabel;  // 时间挑战模式的倒计时标签
    QListWidget* playersList;
    QPushButton* pauseButton;
    QPushButton* menuButton;
    
    // 游戏状态
    GameState currentState;
    Difficulty currentDifficulty;
    bool isMultiplayer;
    bool isLocalCoop;
    bool isHost;
    
    // 游戏对象
    Snake* snake;
    Snake* player2Snake;  // 本地双人游戏的第二个玩家
    Food* food;
    Wall* wall;
    QTimer* gameTimer;
    QTimer* specialFoodTimer;
    QTimer* countdownTimer;  // 时间挑战模式的倒计时器
    
    // 游戏参数
    int gridWidth;
    int gridHeight;
    int cellSize;
    int score;
    int level;
    int baseSpeed;
    int currentSpeed;
    int remainingTime;  // 时间挑战模式的剩余时间（秒）
    const int TIME_CHALLENGE_DURATION = 300;  // 时间挑战模式的总时长（5分钟 = 300秒）
    
    // 多人游戏
    NetworkManager* networkManager;
    MultiPlayerGameManager* multiPlayerManager;
    QString currentRoomId;
    QString playerName;
    QMap<QString, std::deque<Point>> otherPlayers;
    QMap<QString, CharacterType> playerCharacters;
    QMap<QString, int> playerScores;
    QMap<QString, bool> playerAliveStatus;
    
    // 单人游戏管理器
    SinglePlayerGameManager* singlePlayerManager;
    
    // 本地双人游戏相关
    CharacterType player1Character;
    CharacterType player2Character;
    int player1Score;
    int player2Score;
    bool player1Alive;
    bool player2Alive;
    
    // 高分记录
    QList<int> highScores;
    QSettings* settings;
    
    // 特殊食物计时器
    int specialFoodCounter;
    const int SPECIAL_FOOD_INTERVAL = 10; // 每10个普通食物生成一个特殊食物
};

#endif // GAMEWIDGET_H
