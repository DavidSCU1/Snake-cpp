#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QtCore>
#include <QtWidgets>
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
#include "gamestate.h"
#include "snake.h"
#include "food.h"
#include "wall.h"

#include "singleplayergamemanager.h"
#include "hotspotgamemanager.h"


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
    void startLocalCoopGame(SinglePlayerMode mode = SinglePlayerMode::CLASSIC);
    void setLocalCoopMode(CharacterType player1Character, CharacterType player2Character);

    

    
    // 热点游戏管理器访问方法
    HotspotGameManager* getHotspotGameManager() const { return hotspotGameManager; }
    void setHotspotGameManager(HotspotGameManager* manager);
    void setCurrentRoomId(const QString& roomId) { currentRoomId = roomId; }
    void setPlayerName(const QString& name) { m_playerName = name; }
    void pauseGame();
    void resumeGame();
    void resetGame();
    void generateFood();  // 公开食物生成函数供SinglePlayerGameManager调用
    void generateWalls(const QSet<Point>& occupiedPositions, int wallCount = 0);  // 支持传入占用位置的墙体生成
    bool hasWallAt(const Point& position) const;  // 检查指定位置是否有墙体
    
    GameState getCurrentState() const { return currentState; }
    int getCurrentScore() const { return score; }
    int getCurrentSpeed() const { return currentSpeed; }
    QList<Point> getSnakeBody() const;
    
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
    void updateCountdown();  // 更新倒计时
    void updateRespawnTimer();  // 更新复活倒计时
    void updateGameTimer();     // 更新游戏总时间
    
private:
    void setupUI();
    void setupGame();
    void updateGameArea();
    void updateButtonPositions();
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
    void drawPlayerStatusPanel(QPainter& painter);  // 绘制玩家状态面板
    void drawPauseOverlay(QPainter& painter, const QRect& gameRect);
    void drawGameOverOverlay(QPainter& painter, const QRect& gameRect);
    void respawnPlayer(int playerNum);  // 复活玩家
    void endTimeAttackGame();           // 结束时间挑战游戏
    void endLocalCoopGame();            // 结束本地双人游戏

    
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
    

    QString currentRoomId;
    QString m_playerName;
    QMap<QString, std::deque<Point>> otherPlayers;
    QMap<QString, CharacterType> playerCharacters;
    QMap<QString, int> playerScores;
    QMap<QString, bool> playerAliveStatus;
    
    // 单人游戏管理器
    SinglePlayerGameManager* singlePlayerManager;
    
    // 热点游戏管理器
    HotspotGameManager* hotspotGameManager;
    
    // 本地双人游戏相关
    CharacterType player1Character;
    CharacterType player2Character;
    SinglePlayerMode localCoopMode;  // 本地双人游戏模式
    int player1Score;
    int player2Score;
    bool player1Alive;
    bool player2Alive;
    
    // 生命机制相关
    int player1Lives;
    int player2Lives;
    const int MAX_LIVES = 3;  // 最大生命数
    
    // 复活机制相关
    bool player1Respawning;
    bool player2Respawning;
    int player1RespawnTime;
    int player2RespawnTime;
    QTimer* respawnTimer;
    QTimer* gameTimeTimer;  // 游戏总时间计时器
    int totalGameTime;      // 游戏总时间（秒）
    const int RESPAWN_TIME = 10;  // 复活时间（秒）
    const int TOTAL_GAME_TIME = 300;  // 游戏总时长（5分钟）
    
    // 高分记录
    QList<int> highScores;
    QSettings* settings;
    
    // 特殊食物计时器
    int specialFoodCounter;
    const int SPECIAL_FOOD_INTERVAL = 10; // 每10个普通食物生成一个特殊食物
};

#endif // GAMEWIDGET_H
