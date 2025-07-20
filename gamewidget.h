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
#include "networkmanager.h"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = nullptr);
    
    void setCharacter(CharacterType character);
    void setDifficulty(Difficulty difficulty);
    void startSinglePlayerGame();
    void startMultiPlayerGame(bool isHost = false);
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
    
private:
    void setupUI();
    void setupGame();
    void updateGameArea();
    void generateFood();
    void generateSpecialFood();
    void checkCollisions();
    void updateScore(int points);
    void saveHighScore();
    void loadHighScores();
    void updateSpeed();
    void drawGrid(QPainter& painter);
    void drawSnake(QPainter& painter);
    void drawFood(QPainter& painter);
    void drawMultiplayerSnakes(QPainter& painter);
    void drawUI(QPainter& painter);
    void drawPauseOverlay(QPainter& painter);
    void drawGameOverOverlay(QPainter& painter);
    
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
    QListWidget* playersList;
    QPushButton* pauseButton;
    QPushButton* menuButton;
    
    // 游戏状态
    GameState currentState;
    Difficulty currentDifficulty;
    bool isMultiplayer;
    bool isHost;
    
    // 游戏对象
    Snake* snake;
    Food* food;
    QTimer* gameTimer;
    QTimer* specialFoodTimer;
    
    // 游戏参数
    int gridWidth;
    int gridHeight;
    int cellSize;
    int score;
    int level;
    int baseSpeed;
    int currentSpeed;
    
    // 多人游戏
    NetworkManager* networkManager;
    QMap<QString, std::deque<Point>> otherPlayers;
    QMap<QString, CharacterType> playerCharacters;
    QMap<QString, int> playerScores;
    QMap<QString, bool> playerAliveStatus;
    
    // 高分记录
    QList<int> highScores;
    QSettings* settings;
    
    // 特殊食物计时器
    int specialFoodCounter;
    const int SPECIAL_FOOD_INTERVAL = 10; // 每10个普通食物生成一个特殊食物
};

#endif // GAMEWIDGET_H