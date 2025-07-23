#ifndef SINGLEPLAYERGAMEMANAGER_H
#define SINGLEPLAYERGAMEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QSettings>
#include <deque>
#include "gamestate.h"

class GameWidget;

// 单人游戏模式枚举
enum class SinglePlayerMode {
    CLASSIC,        // 经典模式
    TIME_ATTACK,    // 时间挑战模式
    CHALLENGE,      // 挑战模式
    SPEED_RUN,      // 极速模式
    AI_BATTLE       // 人机对战模式
};

// 游戏统计数据
struct GameStats {
    int totalScore = 0;
    int foodEaten = 0;
    int specialFoodEaten = 0;
    int timeElapsed = 0;  // 秒
    int maxLength = 0;
    int perfectMoves = 0;  // 完美移动次数
    double efficiency = 0.0;  // 效率百分比
    bool diedByWallCollision = false;  // 是否撞墙而死
};

// 成就系统
struct Achievement {
    QString id;
    QString name;
    QString description;
    bool unlocked = false;
    int progress = 0;
    int target = 0;
    bool displayed = true; // 是否已经显示过，默认为true表示不需要显示
};

class SinglePlayerGameManager : public QObject
{
    Q_OBJECT

public:
    explicit SinglePlayerGameManager(QObject *parent = nullptr);
    ~SinglePlayerGameManager();
    
    // 游戏模式管理
    void setGameMode(SinglePlayerMode mode);
    SinglePlayerMode getGameMode() const { return currentMode; }
    SinglePlayerMode getCurrentMode() const { return currentMode; }
    QString getModeDescription(SinglePlayerMode mode) const;
    
    // 游戏控制
    void startGame(GameWidget* gameWidget);
    void pauseGame();
    void resumeGame();
    void endGame();
    void resetGame();
    
    // 统计数据
    const GameStats& getGameStats() const { return gameStats; }
    void updateStats(int score, int foodCount, int snakeLength);
    void updateGameStats(const GameStats& stats) { gameStats = stats; }
    void recordFoodEaten(bool isSpecial = false);
    void recordPerfectMove();
    
    // 获取AI蛇的身体
    const std::deque<Point>& getAISnake() const { return aiSnake; }
    CharacterType getAISnakeCharacter() const { return aiSnakeCharacter; }
    Direction getAIDirection() const { return aiDirection; }
    
    // 设置食物位置
    void setFoodPosition(const Point& foodPos) { foodPosition = foodPos; }
    
    // 成就系统
    QList<Achievement> getAchievements() const { return achievements; }
    QList<Achievement> getUnlockedAchievements() const;
    QList<Achievement> getUnDisplayedAchievements() const;
    void checkAchievements();
    void updateAchievements(const QList<Achievement>& newAchievements);
    void markAchievementsAsDisplayed();
    void showPendingAchievements();
    
    // 获取游戏状态
    int getTimeRemaining() const;  // 时间挑战模式
    double getSpeedMultiplier() const;  // 极速模式
    void setSpeedMultiplier(double multiplier);  // 设置速度倍数
    int getAIScore() const;  // 人机对战模式 - AI分数
    int getPlayerScore() const;  // 人机对战模式 - 玩家分数
    
    // 保存/加载
    void saveProgress();
    void loadProgress();
    
signals:
    void modeChanged(SinglePlayerMode mode);
    void statsUpdated(const GameStats& stats);
    void achievementUnlocked(const Achievement& achievement);
    void timeWarning(int secondsLeft);  // 时间挑战模式警告
    void speedIncreased(double multiplier);  // 极速模式速度提升
    void aiScoreUpdated(int aiScore, int playerScore);  // AI对战分数更新
    void gameEnded(SinglePlayerMode mode, const GameStats& finalStats);
    
public:
    // AI对战相关方法
    void initializeAI();
    void updateAIMovement();
    void updateAISpeed(int newSpeed);
    void pauseAI();
    void resumeAI();

private slots:
    void onGameTimer();
    void onModeTimer();
    void onSpeedTimer();
    
private:
    void initializeAchievements();
    void setupModeTimers();
    void updateTimeAttackMode();
    void updateSpeedRunMode();
    
    Direction calculateAIDirection(const Point& aiHead, const Point& target);
    bool isValidAIMove(const Point& aiHead, Direction direction);
    Point getNextPosition(const Point& currentPos, Direction direction);
    
    // 成就检查函数
    void checkScoreAchievements();
    void checkTimeAchievements();
    void checkEfficiencyAchievements();
    void checkSpecialAchievements();
    void checkWallCollisionAchievement();
    
    // 游戏状态
    SinglePlayerMode currentMode;
    GameWidget* gameWidget;
    bool isGameActive;
    bool isPaused;
    
    // 计时器
    QTimer* gameTimer;      // 主游戏计时器
    QTimer* modeTimer;      // 模式特定计时器
    QTimer* speedTimer;     // 速度变化计时器
    QTime gameStartTime;
    
    // AI蛇的目标食物位置
    Point foodPosition;
    
    // 统计数据
    GameStats gameStats;
    
    // 模式特定数据
    int timeAttackDuration;     // 时间挑战模式持续时间（秒）
    double speedMultiplier;     // 速度倍数
    // AI对战模式数据
    int aiScore;                // AI当前分数
    int playerScore;            // 玩家当前分数
    std::deque<Point> aiSnake;  // AI蛇身
    CharacterType aiSnakeCharacter;  // AI蛇的角色
    Direction aiDirection;      // AI移动方向
    Point aiTarget;             // AI目标食物位置
    QTimer* aiMoveTimer;        // AI移动计时器
    
    // 成就系统
    QList<Achievement> achievements;
    QSettings* settings;
    
    // 常量
    static constexpr int TIME_ATTACK_DEFAULT_DURATION = 180;  // 3分钟
    static constexpr double SPEED_RUN_INCREMENT = 0.1;
};

#endif // SINGLEPLAYERGAMEMANAGER_H