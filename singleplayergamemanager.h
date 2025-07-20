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
    SURVIVAL,       // 生存模式
    SPEED_RUN,      // 极速模式
    PUZZLE,         // 解谜模式
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
};

// 成就系统
struct Achievement {
    QString id;
    QString name;
    QString description;
    bool unlocked = false;
    int progress = 0;
    int target = 0;
};

class SinglePlayerGameManager : public QObject
{
    Q_OBJECT

public:
    explicit SinglePlayerGameManager(QObject *parent = nullptr);
    ~SinglePlayerGameManager();
    
    // 游戏模式管理
    void setGameMode(SinglePlayerMode mode);
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
    void recordFoodEaten(bool isSpecial = false);
    void recordPerfectMove();
    
    // 成就系统
    QList<Achievement> getAchievements() const { return achievements; }
    QList<Achievement> getUnlockedAchievements() const;
    void checkAchievements();
    
    // 模式特定功能
    int getTimeRemaining() const;  // 时间挑战模式
    int getSurvivalWaves() const;  // 生存模式
    double getSpeedMultiplier() const;  // 极速模式
    int getPuzzleLevel() const;  // 解谜模式
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
    void waveCompleted(int wave);  // 生存模式波次完成
    void speedIncreased(double multiplier);  // 极速模式速度提升
    void puzzleSolved(int level);  // 解谜模式关卡完成
    void aiScoreUpdated(int aiScore, int playerScore);  // AI对战分数更新
    void gameEnded(SinglePlayerMode mode, const GameStats& finalStats);
    
private slots:
    void onGameTimer();
    void onModeTimer();
    void onSpeedTimer();
    
private:
    void initializeAchievements();
    void setupModeTimers();
    void updateTimeAttackMode();
    void updateSurvivalMode();
    void updateSpeedRunMode();
    void updatePuzzleMode();
    void updateAIBattleMode();
    
    // AI对战相关方法
    void initializeAI();
    void updateAIMovement();
    Direction calculateAIDirection();
    bool isValidAIMove(Direction direction);
    
    // 成就检查函数
    void checkScoreAchievements();
    void checkTimeAchievements();
    void checkEfficiencyAchievements();
    void checkSpecialAchievements();
    
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
    
    // 统计数据
    GameStats gameStats;
    
    // 模式特定数据
    int timeAttackDuration;     // 时间挑战模式持续时间（秒）
    int survivalWave;           // 当前生存波次
    int survivalEnemies;        // 当前波次敌人数量
    double speedMultiplier;     // 速度倍数
    int puzzleLevel;            // 解谜关卡
    QList<QPoint> puzzleTargets; // 解谜目标点
    
    // AI对战模式数据
    int aiScore;                // AI当前分数
    int playerScore;            // 玩家当前分数
    std::deque<Point> aiSnake;  // AI蛇身
    Direction aiDirection;      // AI移动方向
    Point aiTarget;             // AI目标食物位置
    QTimer* aiMoveTimer;        // AI移动计时器
    
    // 成就系统
    QList<Achievement> achievements;
    QSettings* settings;
    
    // 常量
    static constexpr int TIME_ATTACK_DEFAULT_DURATION = 180;  // 3分钟
    static constexpr int SURVIVAL_BASE_ENEMIES = 5;
    static constexpr double SPEED_RUN_INCREMENT = 0.1;
    static constexpr int PUZZLE_MAX_LEVEL = 20;
};

#endif // SINGLEPLAYERGAMEMANAGER_H