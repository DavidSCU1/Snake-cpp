#include "singleplayergamemanager.h"
#include "gamewidget.h"
#include <QDebug>
#include <QSettings>
#include <QRandomGenerator>
#include <QMessageBox>

// 游戏网格大小常量
const int GRID_WIDTH = 40;
const int GRID_HEIGHT = 30;

SinglePlayerGameManager::SinglePlayerGameManager(QObject *parent)
    : QObject(parent)
    , currentMode(SinglePlayerMode::CLASSIC)
    , gameWidget(nullptr)
    , isGameActive(false)
    , isPaused(false)
    , timeAttackDuration(TIME_ATTACK_DEFAULT_DURATION)
    , speedMultiplier(1.0)
    , aiScore(0)
    , playerScore(0)
    , aiDirection(Direction::UP)
    , aiSnakeCharacter(CharacterType::PATRICK)
    , foodPosition(20, 15)
    , settings(new QSettings("SnakeGame", "SinglePlayer", this))
{
    // 初始化计时器
    gameTimer = new QTimer(this);
    modeTimer = new QTimer(this);
    speedTimer = new QTimer(this);
    aiMoveTimer = new QTimer(this);
    
    connect(gameTimer, &QTimer::timeout, this, &SinglePlayerGameManager::onGameTimer);
    connect(modeTimer, &QTimer::timeout, this, &SinglePlayerGameManager::onModeTimer);
    connect(speedTimer, &QTimer::timeout, this, &SinglePlayerGameManager::onSpeedTimer);
    connect(aiMoveTimer, &QTimer::timeout, this, &SinglePlayerGameManager::updateAIMovement);
    
    // 初始化成就系统
    initializeAchievements();
    loadProgress();
}

SinglePlayerGameManager::~SinglePlayerGameManager()
{
    saveProgress();
}

void SinglePlayerGameManager::setGameMode(SinglePlayerMode mode)
{
    qDebug() << "Setting game mode to:" << (int)mode;
    if (currentMode != mode) {
        currentMode = mode;
        emit modeChanged(mode);
        
        // 重置模式特定数据
        switch (mode) {
        case SinglePlayerMode::TIME_ATTACK:
            timeAttackDuration = TIME_ATTACK_DEFAULT_DURATION;
            break;
        case SinglePlayerMode::CHALLENGE:
            qDebug() << "CHALLENGE mode set";
            // 挑战模式无需特殊初始化，墙体生成在食物被吃时处理
            break;
        case SinglePlayerMode::SPEED_RUN:
            speedMultiplier = 1.0;
            break;
        case SinglePlayerMode::AI_BATTLE:
            qDebug() << "AI_BATTLE mode set, resetting AI data";
            aiScore = 0;
            playerScore = 0;
            aiSnake.clear();
            aiDirection = Direction::UP;
            break;
        default:
            break;
        }
    }
}

QString SinglePlayerGameManager::getModeDescription(SinglePlayerMode mode) const
{
    switch (mode) {
    case SinglePlayerMode::CLASSIC:
        return "经典模式：传统的贪吃蛇游戏，吃食物增长身体，避免撞墙和自己。";
    case SinglePlayerMode::TIME_ATTACK:
        return "时间挑战：在限定时间内获得尽可能高的分数！";
    case SinglePlayerMode::CHALLENGE:
        return "挑战模式：每吃一个食物会生成5块墙，难度不断增加！";
    case SinglePlayerMode::SPEED_RUN:
        return "极速模式：速度会不断增加，考验你的反应能力！";
    case SinglePlayerMode::AI_BATTLE:
        return "人机对战：与智能AI比拼积分，看谁能获得更高的分数！\n\n• 玩家与AI同时游戏\n• 领先对手10个食物即可获胜\n• 随机生成障碍物增加挑战性\n• 考验策略和反应能力";
    default:
        return "未知模式";
    }
}

void SinglePlayerGameManager::startGame(GameWidget* widget)
{
    gameWidget = widget;
    isGameActive = true;
    isPaused = false;
    
    // 重置统计数据
    gameStats = GameStats();
    gameStartTime = QTime::currentTime();
    
    // 启动主计时器
    gameTimer->start(1000); // 每秒更新一次
    
    // 设置模式特定计时器
    setupModeTimers();
    
    emit statsUpdated(gameStats);
}

void SinglePlayerGameManager::pauseGame()
{
    if (isGameActive && !isPaused) {
        isPaused = true;
        gameTimer->stop();
        modeTimer->stop();
        speedTimer->stop();
        aiMoveTimer->stop();
    }
}

void SinglePlayerGameManager::resumeGame()
{
    if (isGameActive && isPaused) {
        isPaused = false;
        gameTimer->start(1000);
        
        // 恢复模式特定计时器
        setupModeTimers();
    }
}

void SinglePlayerGameManager::endGame()
{
    if (isGameActive) {
        isGameActive = false;
        isPaused = false;
        
        gameTimer->stop();
        modeTimer->stop();
        speedTimer->stop();
        aiMoveTimer->stop();
        
        // 计算最终效率
        if (gameStats.timeElapsed > 0) {
            gameStats.efficiency = (double)gameStats.foodEaten / gameStats.timeElapsed * 100.0;
        }
        
        // 检查成就
        checkAchievements();
        
        // 显示未显示的成就
        showPendingAchievements();
        
        // 保存进度
        saveProgress();
        
        emit gameEnded(currentMode, gameStats);
    }
}

void SinglePlayerGameManager::resetGame()
{
    endGame();
    gameStats = GameStats();
    
    // 重置模式特定数据
    switch (currentMode) {
    case SinglePlayerMode::TIME_ATTACK:
        timeAttackDuration = TIME_ATTACK_DEFAULT_DURATION;
        break;
    case SinglePlayerMode::CHALLENGE:
        // 挑战模式无需特殊重置
        break;
    case SinglePlayerMode::SPEED_RUN:
        speedMultiplier = 1.0;
        break;
    case SinglePlayerMode::AI_BATTLE:
        aiScore = 0;
        playerScore = 0;
        aiSnake.clear();
        aiDirection = Direction::UP;
        break;
    default:
        break;
    }
}

void SinglePlayerGameManager::updateStats(int score, int foodCount, int snakeLength)
{
    gameStats.totalScore = score;
    gameStats.foodEaten = foodCount;
    gameStats.maxLength = qMax(gameStats.maxLength, snakeLength);
    
    // 在AI对战模式中，同步玩家分数
    if (currentMode == SinglePlayerMode::AI_BATTLE) {
        playerScore = score;
    }
    
    emit statsUpdated(gameStats);
}

void SinglePlayerGameManager::recordFoodEaten(bool isSpecial)
{
    if (isSpecial) {
        gameStats.specialFoodEaten++;
    }
    
    // 检查是否为完美移动（直接路径到食物）
    // 这里可以添加更复杂的逻辑来判断移动效率
}

void SinglePlayerGameManager::recordPerfectMove()
{
    gameStats.perfectMoves++;
}

int SinglePlayerGameManager::getTimeRemaining() const
{
    if (currentMode == SinglePlayerMode::TIME_ATTACK) {
        return qMax(0, timeAttackDuration - gameStats.timeElapsed);
    }
    return 0;
}



double SinglePlayerGameManager::getSpeedMultiplier() const
{
    return speedMultiplier;
}

void SinglePlayerGameManager::setSpeedMultiplier(double multiplier)
{
    speedMultiplier = multiplier;
    emit speedIncreased(speedMultiplier);
}

int SinglePlayerGameManager::getAIScore() const
{
    return aiScore;
}

int SinglePlayerGameManager::getPlayerScore() const
{
    return playerScore;
}

QList<Achievement> SinglePlayerGameManager::getUnlockedAchievements() const
{
    QList<Achievement> unlocked;
    for (const auto& achievement : achievements) {
        if (achievement.unlocked) {
            unlocked.append(achievement);
        }
    }
    return unlocked;
}

void SinglePlayerGameManager::onGameTimer()
{
    if (!isGameActive || isPaused) return;
    
    gameStats.timeElapsed++;
    
    // 更新模式特定逻辑
    switch (currentMode) {
    case SinglePlayerMode::TIME_ATTACK:
        updateTimeAttackMode();
        break;
    case SinglePlayerMode::CHALLENGE:
        // 挑战模式的逻辑在食物被吃时处理
        break;
    case SinglePlayerMode::SPEED_RUN:
        updateSpeedRunMode();
        break;
    case SinglePlayerMode::AI_BATTLE:
        // AI移动逻辑由aiMoveTimer单独处理，这里不需要调用
        break;
    default:
        break;
    }
    
    emit statsUpdated(gameStats);
}

void SinglePlayerGameManager::onModeTimer()
{
    // 模式特定的定时事件
    switch (currentMode) {
    default:
        break;
    }
}

void SinglePlayerGameManager::onSpeedTimer()
{
    // 极速模式下不再使用定时器自动加速
    // 速度增加现在完全由吃食物触发
}

void SinglePlayerGameManager::initializeAchievements()
{
    achievements.clear();
    
    // 分数成就
    achievements.append({"score_100", "初学者", "获得100分", false, 0, 100, true});
    achievements.append({"score_500", "进步者", "获得500分", false, 0, 500, true});
    achievements.append({"score_1000", "高手", "获得1000分", false, 0, 1000, true});
    achievements.append({"score_5000", "大师", "获得5000分", false, 0, 5000, true});
    achievements.append({"score_10000", "传奇", "获得10000分", false, 0, 10000, true});
    achievements.append({"score_25000", "神话", "获得25000分", false, 0, 25000, true});
    
    // 时间成就
    achievements.append({"time_60", "坚持者", "游戏时间超过1分钟", false, 0, 60, true});
    achievements.append({"time_300", "耐力王", "游戏时间超过5分钟", false, 0, 300, true});
    achievements.append({"time_600", "马拉松", "游戏时间超过10分钟", false, 0, 600, true});
    achievements.append({"time_1200", "超级马拉松", "游戏时间超过20分钟", false, 0, 1200, true});
    achievements.append({"time_1800", "铁人", "游戏时间超过30分钟", false, 0, 1800, true});
    
    // 长度成就
    achievements.append({"length_20", "小蛇", "蛇身长度达到20", false, 0, 20, true});
    achievements.append({"length_50", "大蛇", "蛇身长度达到50", false, 0, 50, true});
    achievements.append({"length_100", "巨蛇", "蛇身长度达到100", false, 0, 100, true});
    achievements.append({"length_200", "超级巨蛇", "蛇身长度达到200", false, 0, 200, true});
    achievements.append({"length_500", "蛇王", "蛇身长度达到500", false, 0, 500, true});
    
    // 连击成就
    achievements.append({"combo_5", "连击新手", "连续吃掉5个食物", false, 0, 5, true});
    achievements.append({"combo_10", "连击高手", "连续吃掉10个食物", false, 0, 10, true});
    achievements.append({"combo_20", "连击大师", "连续吃掉20个食物", false, 0, 20, true});
    achievements.append({"combo_50", "连击之王", "连续吃掉50个食物", false, 0, 50, true});
    
    // 速度成就
    achievements.append({"speed_fast", "速度恶魔", "游戏速度达到最快级别", false, 0, 1, true});
    achievements.append({"speed_maintain_60", "稳定高速", "在高速下维持60秒", false, 0, 60, true});
    achievements.append({"speed_no_slow", "永不减速", "整局游戏从不减速", false, 0, 1, true});
    
    // 精准成就
    achievements.append({"perfect_10", "完美主义者", "连续10次完美移动", false, 0, 10, true});
    achievements.append({"perfect_50", "精准大师", "连续50次完美移动", false, 0, 50, true});
    achievements.append({"no_waste_move", "零浪费", "整局游戏无无效移动", false, 0, 1, true});
    achievements.append({"efficiency_80", "效率专家", "游戏效率达到80%", false, 0, 80, true});
    achievements.append({"efficiency_95", "完美效率", "游戏效率达到95%", false, 0, 95, true});
    
    // 生存成就
    achievements.append({"near_death_5", "死里逃生", "5次险些撞墙但成功避开", false, 0, 5, true});
    achievements.append({"corner_escape", "绝境求生", "在角落中成功脱困", false, 0, 1, true});
    achievements.append({"wall_collision", "硬！", "在游戏中撞墙而死", false, 0, 1, true});
    achievements.append({"self_collision", "自食其果", "撞到自己的身体而死", false, 0, 1, true});
    
    // 收集成就
    achievements.append({"special_food_10", "美食家", "吃掉10个特殊食物", false, 0, 10, true});
    achievements.append({"special_food_50", "特殊收集家", "吃掉50个特殊食物", false, 0, 50, true});
    achievements.append({"food_variety", "营养均衡", "吃掉所有类型的食物", false, 0, 5, true});
    achievements.append({"golden_food", "黄金猎手", "吃掉金色特殊食物", false, 0, 1, true});
    
    // 探索成就
    achievements.append({"map_explorer", "地图探索者", "访问游戏区域的80%", false, 0, 80, true});
    achievements.append({"corner_visitor", "角落访客", "访问所有四个角落", false, 0, 4, true});
    achievements.append({"border_runner", "边界行者", "沿着边界移动100步", false, 0, 100, true});
    
    // 技巧成就
    achievements.append({"quick_turn", "急转弯大师", "连续进行20次急转弯", false, 0, 20, true});
    achievements.append({"spiral_master", "螺旋大师", "完成一个完整的螺旋移动", false, 0, 1, true});
    achievements.append({"zigzag_king", "之字形之王", "连续进行50次之字形移动", false, 0, 50, true});
    
    // 挑战成就
    achievements.append({"no_pause", "一气呵成", "整局游戏从不暂停", false, 0, 1, true});
    achievements.append({"blindfold", "盲打高手", "在高速下不看屏幕移动20步", false, 0, 20, true});
    achievements.append({"minimalist", "极简主义", "用最少的移动次数达到50分", false, 0, 50, true});
    
    // 模式特定成就
    achievements.append({"time_attack_master", "时间大师", "时间挑战模式获得1000分", false, 0, 1000, true});
    achievements.append({"time_attack_speed", "时间竞速者", "时间挑战模式在2分钟内获得500分", false, 0, 500, true});
    achievements.append({"challenge_walls_50", "墙体大师", "挑战模式生成50块墙体", false, 0, 50, true});
    achievements.append({"challenge_survivor", "挑战生存者", "挑战模式下生存10分钟", false, 0, 600, true});
    achievements.append({"speed_run_5x", "极速之王", "极速模式达到5倍速度", false, 0, 5, true});
    achievements.append({"speed_run_master", "极速大师", "极速模式获得2000分", false, 0, 2000, true});
    achievements.append({"ai_battle_win", "人机对战胜利者", "在人机对战中击败AI", false, 0, 1, true});
    achievements.append({"ai_battle_500", "AI挑战者", "人机对战模式获得500分", false, 0, 500, true});
    achievements.append({"ai_battle_master", "AI征服者", "人机对战模式获得1000分", false, 0, 1000, true});
    achievements.append({"ai_dominator", "AI统治者", "连续3局击败AI", false, 0, 3, true});
    
    // 特殊里程碑成就
    achievements.append({"first_game", "初次体验", "完成第一局游戏", false, 0, 1, true});
    achievements.append({"comeback_king", "逆转之王", "从濒死状态逆转获胜", false, 0, 1, true});
    achievements.append({"perfectionist", "完美主义", "一局游戏中解锁5个成就", false, 0, 5, true});
    achievements.append({"achievement_hunter", "成就猎人", "解锁30个成就", false, 0, 30, true});
    achievements.append({"completionist", "完美收集家", "解锁所有成就", false, 0, 50, true});
    achievements.append({"genius_youth", "天才少年", "在时间模式下吃到10个特殊食物。如果那是你的真实实力,那么所有质疑都是对你的认可", false, 0, 10, true});
}

void SinglePlayerGameManager::setupModeTimers()
{
    qDebug() << "Setting up mode timers for mode:" << (int)currentMode;
    modeTimer->stop();
    speedTimer->stop();
    aiMoveTimer->stop();
    
    switch (currentMode) {
    case SinglePlayerMode::CHALLENGE:
        // 挑战模式不需要定时器，墙体生成在食物被吃时处理
        break;
    case SinglePlayerMode::SPEED_RUN:
        speedTimer->start(10000); // 每10秒增加速度
        break;
    case SinglePlayerMode::AI_BATTLE:
        {
            qDebug() << "Starting AI move timer and initializing AI";
            // AI移动速度与玩家一致
            int playerSpeed = gameWidget ? gameWidget->getCurrentSpeed() : 200;
            int aiSpeed = playerSpeed; // AI蛇移动速度与玩家相同
            aiMoveTimer->start(aiSpeed);
            initializeAI();
            
            // 在AI对战模式中生成障碍物
            if (gameWidget) {
                QSet<Point> occupiedPositions;
                // 添加玩家蛇的位置
                QList<Point> playerBody = gameWidget->getSnakeBody();
                for (const Point& segment : playerBody) {
                    occupiedPositions.insert(segment);
                }
                // 添加AI蛇的位置
                for (const Point& segment : aiSnake) {
                    occupiedPositions.insert(segment);
                }
                // 添加食物位置
                occupiedPositions.insert(foodPosition);
                
                // 生成30-50个障碍物
                int wallCount = QRandomGenerator::global()->bounded(30, 51);
                gameWidget->generateWalls(occupiedPositions, wallCount);
                qDebug() << "Generated" << wallCount << "walls for AI battle mode";
            }
            break;
        }
    default:
        break;
    }
}

void SinglePlayerGameManager::updateTimeAttackMode()
{
    int remaining = getTimeRemaining();
    if (remaining <= 0) {
        // 时间到，结束游戏
        endGame();
    } else if (remaining <= 10) {
        // 最后10秒警告
        emit timeWarning(remaining);
    }
}



void SinglePlayerGameManager::updateSpeedRunMode()
{
    // 极速模式的更新逻辑
    // 速度增加在speedTimer中处理
}


void SinglePlayerGameManager::checkAchievements()
{
    checkScoreAchievements();
    checkTimeAchievements();
    checkLengthAchievements();
    checkComboAchievements();
    checkSpeedAchievements();
    checkEfficiencyAchievements();
    checkSurvivalAchievements();
    checkCollectionAchievements();
    checkExplorationAchievements();
    checkSkillAchievements();
    checkChallengeAchievements();
    checkSpecialAchievements();
    checkWallCollisionAchievement();
    checkMilestoneAchievements();
}

void SinglePlayerGameManager::updateAchievements(const QList<Achievement>& newAchievements)
{
    achievements = newAchievements;
    saveProgress(); // 保存更新后的成就进度
}

QList<Achievement> SinglePlayerGameManager::getUnDisplayedAchievements() const
{
    QList<Achievement> unDisplayedAchievements;
    for (const auto& achievement : achievements) {
        if (achievement.unlocked && !achievement.displayed) {
            unDisplayedAchievements.append(achievement);
        }
    }
    return unDisplayedAchievements;
}

void SinglePlayerGameManager::markAchievementsAsDisplayed()
{
    bool updated = false;
    for (auto& achievement : achievements) {
        if (achievement.unlocked && !achievement.displayed) {
            achievement.displayed = true;
            updated = true;
        }
    }
    
    if (updated) {
        saveProgress(); // 保存更新后的成就进度
    }
}

void SinglePlayerGameManager::showPendingAchievements()
{
    QList<Achievement> unDisplayedAchievements = getUnDisplayedAchievements();
    if (!unDisplayedAchievements.isEmpty()) {
        QString message = "恭喜！你解锁了以下成就：\n\n";
        
        for (const auto& achievement : unDisplayedAchievements) {
            message += QString("• %1: %2\n")
                      .arg(achievement.name)
                      .arg(achievement.description);
        }
        
        QMessageBox::information(nullptr, "成就解锁", message);
        markAchievementsAsDisplayed();
    }
}

void SinglePlayerGameManager::checkWallCollisionAchievement()
{
    // 检查撞墙而死成就
    for (auto& achievement : achievements) {
        if (achievement.id == "wall_collision" && !achievement.unlocked) {
            if (gameStats.diedByWallCollision) {
                achievement.unlocked = true;
                achievement.progress = 1;
                achievement.displayed = false; // 标记为未显示
                emit achievementUnlocked(achievement);
                // 不再立即显示成就解锁提示，而是在游戏结束时统一显示
            }
        }
    }
}

void SinglePlayerGameManager::checkScoreAchievements()
{
    QStringList scoreAchievements = {"score_100", "score_500", "score_1000", "score_5000", "score_10000", "score_25000"};
    
    for (auto& achievement : achievements) {
        if (scoreAchievements.contains(achievement.id) && !achievement.unlocked) {
            if (gameStats.totalScore >= achievement.target) {
                achievement.unlocked = true;
                achievement.progress = achievement.target;
                achievement.displayed = false; // 标记为未显示
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.totalScore;
            }
        }
    }
}

void SinglePlayerGameManager::checkTimeAchievements()
{
    QStringList timeAchievements = {"time_60", "time_300", "time_600", "time_1200", "time_1800"};
    
    for (auto& achievement : achievements) {
        if (timeAchievements.contains(achievement.id) && !achievement.unlocked) {
            if (gameStats.timeElapsed >= achievement.target) {
                achievement.unlocked = true;
                achievement.progress = achievement.target;
                achievement.displayed = false; // 标记为未显示
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.timeElapsed;
            }
        }
    }
}

void SinglePlayerGameManager::checkEfficiencyAchievements()
{
    QStringList efficiencyAchievements = {"efficiency_80", "efficiency_95", "perfect_10", "perfect_50", "no_waste_move"};
    
    for (auto& achievement : achievements) {
        if (efficiencyAchievements.contains(achievement.id) && !achievement.unlocked) {
            if (achievement.id == "efficiency_80" || achievement.id == "efficiency_95") {
                if (gameStats.efficiency >= achievement.target) {
                    achievement.unlocked = true;
                    achievement.progress = achievement.target;
                    achievement.displayed = false;
                    emit achievementUnlocked(achievement);
                } else {
                    achievement.progress = (int)gameStats.efficiency;
                }
            } else if (achievement.id == "perfect_10" || achievement.id == "perfect_50") {
                if (gameStats.perfectMoves >= achievement.target) {
                    achievement.unlocked = true;
                    achievement.progress = achievement.target;
                    achievement.displayed = false;
                    emit achievementUnlocked(achievement);
                } else {
                    achievement.progress = gameStats.perfectMoves;
                }
            } else if (achievement.id == "no_waste_move") {
                if (gameStats.wastedMoves == 0 && gameStats.totalScore > 100) {
                    achievement.unlocked = true;
                    achievement.progress = 1;
                    achievement.displayed = false;
                    emit achievementUnlocked(achievement);
                }
            }
        }
    }
}

void SinglePlayerGameManager::checkLengthAchievements()
{
    QStringList lengthAchievements = {"length_20", "length_50", "length_100", "length_200", "length_500"};
    for (auto& achievement : achievements) {
        if (lengthAchievements.contains(achievement.id) && !achievement.unlocked) {
            if (gameStats.maxLength >= achievement.target) {
                achievement.unlocked = true;
                achievement.progress = achievement.target;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.maxLength;
            }
        }
    }
}

void SinglePlayerGameManager::checkComboAchievements()
{
    QStringList comboAchievements = {"combo_5", "combo_10", "combo_20", "combo_50"};
    for (auto& achievement : achievements) {
        if (comboAchievements.contains(achievement.id) && !achievement.unlocked) {
            if (gameStats.currentCombo >= achievement.target) {
                achievement.unlocked = true;
                achievement.progress = achievement.target;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.currentCombo;
            }
        }
    }
}

void SinglePlayerGameManager::checkSpeedAchievements()
{
    for (auto& achievement : achievements) {
        if (achievement.id == "speed_fast" && !achievement.unlocked) {
            if (gameWidget && gameWidget->getCurrentSpeed() <= 50) { // 最快速度
                achievement.unlocked = true;
                achievement.progress = 1;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            }
        } else if (achievement.id == "speed_maintain_60" && !achievement.unlocked) {
            if (gameStats.highSpeedTime >= 60) {
                achievement.unlocked = true;
                achievement.progress = 60;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.highSpeedTime;
            }
        } else if (achievement.id == "speed_no_slow" && !achievement.unlocked) {
            if (gameStats.neverSlowed && gameStats.totalScore > 200) {
                achievement.unlocked = true;
                achievement.progress = 1;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            }
        }
    }
}

void SinglePlayerGameManager::checkSurvivalAchievements()
{
    for (auto& achievement : achievements) {
        if (achievement.id == "near_death_5" && !achievement.unlocked) {
            if (gameStats.nearDeathEscapes >= 5) {
                achievement.unlocked = true;
                achievement.progress = 5;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.nearDeathEscapes;
            }
        } else if (achievement.id == "corner_escape" && !achievement.unlocked) {
            if (gameStats.cornerEscapes >= 1) {
                achievement.unlocked = true;
                achievement.progress = 1;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            }
        } else if (achievement.id == "self_collision" && !achievement.unlocked) {
            if (gameStats.diedBySelfCollision) {
                achievement.unlocked = true;
                achievement.progress = 1;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            }
        }
    }
}

void SinglePlayerGameManager::checkCollectionAchievements()
{
    QStringList collectionAchievements = {"special_food_10", "special_food_50", "food_variety", "golden_food"};
    for (auto& achievement : achievements) {
        if (collectionAchievements.contains(achievement.id) && !achievement.unlocked) {
            if (achievement.id == "special_food_10" || achievement.id == "special_food_50") {
                if (gameStats.specialFoodEaten >= achievement.target) {
                    achievement.unlocked = true;
                    achievement.progress = achievement.target;
                    achievement.displayed = false;
                    emit achievementUnlocked(achievement);
                } else {
                    achievement.progress = gameStats.specialFoodEaten;
                }
            } else if (achievement.id == "food_variety") {
                if (gameStats.foodTypesEaten >= 5) {
                    achievement.unlocked = true;
                    achievement.progress = 5;
                    achievement.displayed = false;
                    emit achievementUnlocked(achievement);
                } else {
                    achievement.progress = gameStats.foodTypesEaten;
                }
            } else if (achievement.id == "golden_food") {
                if (gameStats.goldenFoodEaten >= 1) {
                    achievement.unlocked = true;
                    achievement.progress = 1;
                    achievement.displayed = false;
                    emit achievementUnlocked(achievement);
                }
            }
        }
    }
}

void SinglePlayerGameManager::checkExplorationAchievements()
{
    for (auto& achievement : achievements) {
        if (achievement.id == "map_explorer" && !achievement.unlocked) {
            if (gameStats.mapCoverage >= 80) {
                achievement.unlocked = true;
                achievement.progress = 80;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.mapCoverage;
            }
        } else if (achievement.id == "corner_visitor" && !achievement.unlocked) {
            if (gameStats.cornersVisited >= 4) {
                achievement.unlocked = true;
                achievement.progress = 4;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.cornersVisited;
            }
        } else if (achievement.id == "border_runner" && !achievement.unlocked) {
            if (gameStats.borderSteps >= 100) {
                achievement.unlocked = true;
                achievement.progress = 100;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.borderSteps;
            }
        }
    }
}

void SinglePlayerGameManager::checkSkillAchievements()
{
    for (auto& achievement : achievements) {
        if (achievement.id == "quick_turn" && !achievement.unlocked) {
            if (gameStats.quickTurns >= 20) {
                achievement.unlocked = true;
                achievement.progress = 20;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.quickTurns;
            }
        } else if (achievement.id == "spiral_master" && !achievement.unlocked) {
            if (gameStats.spiralCompleted) {
                achievement.unlocked = true;
                achievement.progress = 1;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            }
        } else if (achievement.id == "zigzag_king" && !achievement.unlocked) {
            if (gameStats.zigzagMoves >= 50) {
                achievement.unlocked = true;
                achievement.progress = 50;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.zigzagMoves;
            }
        }
    }
}

void SinglePlayerGameManager::checkChallengeAchievements()
{
    for (auto& achievement : achievements) {
        if (achievement.id == "no_pause" && !achievement.unlocked) {
            if (gameStats.neverPaused && gameStats.totalScore > 100) {
                achievement.unlocked = true;
                achievement.progress = 1;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            }
        } else if (achievement.id == "blindfold" && !achievement.unlocked) {
            if (gameStats.blindMoves >= 20) {
                achievement.unlocked = true;
                achievement.progress = 20;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.blindMoves;
            }
        } else if (achievement.id == "minimalist" && !achievement.unlocked) {
            if (gameStats.totalScore >= 50 && gameStats.totalMoves <= gameStats.totalScore / 2) {
                achievement.unlocked = true;
                achievement.progress = 50;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            }
        }
    }
}

void SinglePlayerGameManager::checkMilestoneAchievements()
{
    for (auto& achievement : achievements) {
        if (achievement.id == "first_game" && !achievement.unlocked) {
            if (gameStats.gamesPlayed >= 1) {
                achievement.unlocked = true;
                achievement.progress = 1;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            }
        } else if (achievement.id == "comeback_king" && !achievement.unlocked) {
            if (gameStats.comebackWins >= 1) {
                achievement.unlocked = true;
                achievement.progress = 1;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            }
        } else if (achievement.id == "perfectionist" && !achievement.unlocked) {
            if (gameStats.achievementsThisGame >= 5) {
                achievement.unlocked = true;
                achievement.progress = 5;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.achievementsThisGame;
            }
        } else if (achievement.id == "achievement_hunter" && !achievement.unlocked) {
            int unlockedCount = 0;
            for (const auto& ach : achievements) {
                if (ach.unlocked) unlockedCount++;
            }
            if (unlockedCount >= 30) {
                achievement.unlocked = true;
                achievement.progress = 30;
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = unlockedCount;
            }
        } else if (achievement.id == "completionist" && !achievement.unlocked) {
            int unlockedCount = 0;
            for (const auto& ach : achievements) {
                if (ach.unlocked) unlockedCount++;
            }
            if (unlockedCount >= achievements.size() - 1) { // 除了自己
                achievement.unlocked = true;
                achievement.progress = achievements.size();
                achievement.displayed = false;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = unlockedCount;
            }
        }
    }
}

void SinglePlayerGameManager::checkSpecialAchievements()
{
    
    // 检查模式特定成就
    if (currentMode == SinglePlayerMode::TIME_ATTACK) {
        for (auto& achievement : achievements) {
            if (achievement.id == "time_attack_master" && !achievement.unlocked) {
                if (gameStats.totalScore >= achievement.target) {
                    achievement.unlocked = true;
                    achievement.progress = achievement.target;
                    achievement.displayed = false; // 标记为未显示
                    emit achievementUnlocked(achievement);
                }
            } else if (achievement.id == "genius_youth" && !achievement.unlocked) {
                if (gameStats.specialFoodEaten >= 10) {
                    achievement.unlocked = true;
                    achievement.progress = 10;
                    achievement.displayed = false; // 标记为未显示
                    emit achievementUnlocked(achievement);
                } else {
                    achievement.progress = gameStats.specialFoodEaten;
                }
            }
        }
    } else if (currentMode == SinglePlayerMode::AI_BATTLE) {
        // 检查AI对战成就
        for (auto& achievement : achievements) {
            if (achievement.id == "ai_battle_win" && !achievement.unlocked) {
                if (playerScore > aiScore) {
                    achievement.unlocked = true;
                    achievement.progress = 1;
                    achievement.displayed = false; // 标记为未显示
                    emit achievementUnlocked(achievement);
                }
            } else if (achievement.id == "ai_battle_500" && !achievement.unlocked) {
                if (gameStats.totalScore >= 500) {
                    achievement.unlocked = true;
                    achievement.progress = achievement.target;
                    achievement.displayed = false; // 标记为未显示
                    emit achievementUnlocked(achievement);
                }
            } else if (achievement.id == "ai_battle_master" && !achievement.unlocked) {
                if (gameStats.totalScore >= 1000) {
                    achievement.unlocked = true;
                    achievement.progress = achievement.target;
                    achievement.displayed = false; // 标记为未显示
                    emit achievementUnlocked(achievement);
                }
            }
        }
    }
}

void SinglePlayerGameManager::saveProgress()
{
    // 保存成就进度
    settings->beginWriteArray("achievements");
    for (int i = 0; i < achievements.size(); ++i) {
        settings->setArrayIndex(i);
        settings->setValue("id", achievements[i].id);
        settings->setValue("unlocked", achievements[i].unlocked);
        settings->setValue("progress", achievements[i].progress);
        settings->setValue("displayed", achievements[i].displayed);
    }
    settings->endArray();
    
    // 保存最佳记录
    settings->setValue("bestScore", settings->value("bestScore", 0).toInt() < gameStats.totalScore ? gameStats.totalScore : settings->value("bestScore", 0).toInt());
    settings->setValue("longestTime", settings->value("longestTime", 0).toInt() < gameStats.timeElapsed ? gameStats.timeElapsed : settings->value("longestTime", 0).toInt());
    settings->setValue("maxLength", settings->value("maxLength", 0).toInt() < gameStats.maxLength ? gameStats.maxLength : settings->value("maxLength", 0).toInt());
}

void SinglePlayerGameManager::loadProgress()
{
    // 加载成就进度
    int size = settings->beginReadArray("achievements");
    for (int i = 0; i < size && i < achievements.size(); ++i) {
        settings->setArrayIndex(i);
        QString id = settings->value("id").toString();
        
        // 找到对应的成就并更新
        for (auto& achievement : achievements) {
            if (achievement.id == id) {
                achievement.unlocked = settings->value("unlocked", false).toBool();
                achievement.progress = settings->value("progress", 0).toInt();
                achievement.displayed = settings->value("displayed", true).toBool();
                break;
            }
        }
    }
    settings->endArray();
}

// AI对战模式实现
// updateAIBattleMode函数已被移除，AI移动逻辑统一使用updateAIMovement函数

void SinglePlayerGameManager::initializeAI()
{
    qDebug() << "Initializing AI snake";
    aiSnake.clear();
    
    // 在游戏区域的右半部分选择一个安全的起始位置
    int startX = 35;
    int startY = 10;
    Point startPos = {startX, startY};
    
    // 创建初始AI蛇身（3个段）
    aiSnake.push_back(startPos);
    aiSnake.push_back({startPos.x - 1, startPos.y});
    aiSnake.push_back({startPos.x - 2, startPos.y});
    
    // 初始方向设置为向左
    aiDirection = Direction::LEFT;
    
    // 设置AI蛇的角色
    aiSnakeCharacter = CharacterType::PATRICK;
    
    // 初始化目标位置为食物位置
    aiTarget = foodPosition;
    
    // 重置AI分数
    aiScore = 0;
    
    qDebug() << "AI snake initialized with" << aiSnake.size() << "segments at position" << startX << "," << startY;
    qDebug() << "AI target set to:" << aiTarget.x << "," << aiTarget.y;
}


Point SinglePlayerGameManager::getNextPosition(const Point& currentPos, Direction direction)
{
    Point nextPos = currentPos;
    switch (direction) {
    case Direction::UP:
        nextPos.y--;
        break;
    case Direction::DOWN:
        nextPos.y++;
        break;
    case Direction::LEFT:
        nextPos.x--;
        break;
    case Direction::RIGHT:
        nextPos.x++;
        break;
    }
    return nextPos;
}

void SinglePlayerGameManager::updateAISpeed(int newSpeed)
{
    if (currentMode == SinglePlayerMode::AI_BATTLE && aiMoveTimer->isActive()) {
        // AI蛇速度与玩家一致
        int aiSpeed = newSpeed;
        qDebug() << "Updating AI speed from" << aiMoveTimer->interval() << "to" << aiSpeed << "(player speed:" << newSpeed << ")";
        aiMoveTimer->setInterval(aiSpeed);
    }
}

void SinglePlayerGameManager::pauseAI()
{
    if (currentMode == SinglePlayerMode::AI_BATTLE && aiMoveTimer->isActive()) {
        qDebug() << "Pausing AI movement timer";
        aiMoveTimer->stop();
    }
}

void SinglePlayerGameManager::resumeAI()
{
    if (currentMode == SinglePlayerMode::AI_BATTLE && !aiMoveTimer->isActive() && isGameActive && !isPaused) {
        qDebug() << "Resuming AI movement timer";
        // AI蛇速度与玩家一致
        int playerSpeed = gameWidget ? gameWidget->getCurrentSpeed() : 200;
        int aiSpeed = playerSpeed;
        aiMoveTimer->start(aiSpeed);
    }
}

void SinglePlayerGameManager::updateAIMovement()
{
    if (!isGameActive || isPaused || aiSnake.empty()) {
        qDebug() << "AI movement blocked: isGameActive=" << isGameActive << ", isPaused=" << isPaused << ", aiSnake.empty()=" << aiSnake.empty();
        return;
    }
    
    qDebug() << "AI movement update called, aiSnake size:" << aiSnake.size() << ", foodPosition:" << foodPosition.x << "," << foodPosition.y;
    
    // 使用食物位置作为目标
    aiTarget = foodPosition;
    
    // 计算AI的下一步移动方向
    Point aiHead = aiSnake.front();
    Direction newDirection = calculateAIDirection(aiHead, foodPosition);
    
    // 如果计算出的方向不安全，尝试其他方向
     if (!isValidAIMove(aiHead, newDirection)) {
        QList<Direction> safeDirections;
         for (Direction dir : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
             // 避免180度转向
             if ((dir == Direction::UP && aiDirection == Direction::DOWN) ||
                 (dir == Direction::DOWN && aiDirection == Direction::UP) ||
                 (dir == Direction::LEFT && aiDirection == Direction::RIGHT) ||
                 (dir == Direction::RIGHT && aiDirection == Direction::LEFT)) {
                 continue;
             }
             
             if (isValidAIMove(aiHead, dir)) {
                 safeDirections.append(dir);
             }
         }
        
        if (!safeDirections.isEmpty()) {
            // 随机选择一个安全的方向
            newDirection = safeDirections[QRandomGenerator::global()->bounded(safeDirections.size())];
        } else {
            // 如果没有安全的方向，保持当前方向
            newDirection = aiDirection;
        }
    }
    
    aiDirection = newDirection;
    
    // 移动AI蛇
    Point newHead = getNextPosition(aiSnake.front(), aiDirection);
    
    // 检查边界碰撞
    if (newHead.x < 0 || newHead.x >= 40 || newHead.y < 0 || newHead.y >= 30) {
        // AI撞墙，重新初始化
        initializeAI();
        return;
    }
    
    // 检查自身碰撞
    for (const Point& segment : aiSnake) {
        if (segment.x == newHead.x && segment.y == newHead.y) {
            // AI撞到自己，重新初始化
            initializeAI();
            return;
        }
    }
    
    // 检查障碍物碰撞
    if (gameWidget && gameWidget->hasWallAt(newHead)) {
        // AI撞到障碍物，重新初始化
        initializeAI();
        return;
    }
    
    aiSnake.push_front(newHead);
    
    // 检查是否吃到食物
    if (newHead.x == foodPosition.x && newHead.y == foodPosition.y) {
        aiScore += 10;
        qDebug() << "AI snake ate food! Score:" << aiScore;
        
        // 通知GameWidget重新生成食物
        if (gameWidget) {
            gameWidget->generateFood();
        }
        
        emit aiScoreUpdated(aiScore, playerScore);
        
        // 检查分数差距是否达到结束条件（AI领先100分）
        if (aiScore - playerScore >= 100) {
            qDebug() << "AI wins! Score difference reached 100 points. AI:" << aiScore << ", Player:" << playerScore;
            endGame();
            emit gameEnded("AI获胜！领先10个食物的分数！");
            return;
        }
    } else {
        aiSnake.pop_back();
    }
    
    // 检查玩家分数是否领先AI 100分
    if (playerScore - aiScore >= 100) {
        qDebug() << "Player wins! Score difference reached 100 points. Player:" << playerScore << ", AI:" << aiScore;
        endGame();
        emit gameEnded("玩家获胜！领先10个食物的分数！");
        return;
    }
}

Direction SinglePlayerGameManager::calculateAIDirection(const Point& aiHead, const Point& target)
{
    // 计算到目标的距离
    int dx = target.x - aiHead.x;
    int dy = target.y - aiHead.y;
    
    // 优先选择距离目标更近的单一方向
    Direction preferredDirection;
    
    // 选择距离更远的轴作为主要移动方向
    if (abs(dx) > abs(dy)) {
        // 水平移动优先
        preferredDirection = (dx > 0) ? Direction::RIGHT : Direction::LEFT;
    } else if (abs(dy) > abs(dx)) {
        // 垂直移动优先
        preferredDirection = (dy > 0) ? Direction::DOWN : Direction::UP;
    } else {
        // 距离相等时，优先选择水平移动
        if (dx != 0) {
            preferredDirection = (dx > 0) ? Direction::RIGHT : Direction::LEFT;
        } else if (dy != 0) {
            preferredDirection = (dy > 0) ? Direction::DOWN : Direction::UP;
        } else {
            // 已经到达目标，保持当前方向
            preferredDirection = aiDirection;
        }
    }
    
    // 检查首选方向是否安全且不是180度转向
    if (preferredDirection != aiDirection || 
        !((preferredDirection == Direction::UP && aiDirection == Direction::DOWN) ||
          (preferredDirection == Direction::DOWN && aiDirection == Direction::UP) ||
          (preferredDirection == Direction::LEFT && aiDirection == Direction::RIGHT) ||
          (preferredDirection == Direction::RIGHT && aiDirection == Direction::LEFT))) {
        
        if (isValidAIMove(aiHead, preferredDirection)) {
            return preferredDirection;
        }
    }
    
    // 如果首选方向不安全，尝试其他方向
    QList<Direction> alternativeDirections;
    
    // 根据目标位置创建备选方向列表
    if (abs(dx) > abs(dy)) {
        // 主要是水平移动，备选垂直方向
        if (dy > 0) {
            alternativeDirections << Direction::DOWN << Direction::UP;
        } else if (dy < 0) {
            alternativeDirections << Direction::UP << Direction::DOWN;
        } else {
            alternativeDirections << Direction::UP << Direction::DOWN;
        }
        // 最后考虑反向水平移动
        alternativeDirections << ((dx > 0) ? Direction::LEFT : Direction::RIGHT);
    } else {
        // 主要是垂直移动，备选水平方向
        if (dx > 0) {
            alternativeDirections << Direction::RIGHT << Direction::LEFT;
        } else if (dx < 0) {
            alternativeDirections << Direction::LEFT << Direction::RIGHT;
        } else {
            alternativeDirections << Direction::LEFT << Direction::RIGHT;
        }
        // 最后考虑反向垂直移动
        alternativeDirections << ((dy > 0) ? Direction::UP : Direction::DOWN);
    }
    
    // 检查备选方向
    for (Direction dir : alternativeDirections) {
        // 避免180度转向
        if ((dir == Direction::UP && aiDirection == Direction::DOWN) ||
            (dir == Direction::DOWN && aiDirection == Direction::UP) ||
            (dir == Direction::LEFT && aiDirection == Direction::RIGHT) ||
            (dir == Direction::RIGHT && aiDirection == Direction::LEFT)) {
            continue;
        }
        
        if (isValidAIMove(aiHead, dir)) {
            return dir;
        }
    }
    
    // 如果所有方向都不安全，尝试任何可用的方向（避免死锁）
    for (Direction dir : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
        if ((dir == Direction::UP && aiDirection == Direction::DOWN) ||
            (dir == Direction::DOWN && aiDirection == Direction::UP) ||
            (dir == Direction::LEFT && aiDirection == Direction::RIGHT) ||
            (dir == Direction::RIGHT && aiDirection == Direction::LEFT)) {
            continue;
        }
        
        if (isValidAIMove(aiHead, dir)) {
            return dir;
        }
    }
    
    // 如果没有找到安全的方向，返回当前方向
    return aiDirection;
}

bool SinglePlayerGameManager::isValidAIMove(const Point& aiHead, Direction direction)
{
    Point newHead = getNextPosition(aiHead, direction);
    
    // 检查边界碰撞
    if (newHead.x < 0 || newHead.x >= 40 || newHead.y < 0 || newHead.y >= 30) {
        return false;
    }
    
    // 检查是否撞到自己的身体
    for (const Point& segment : aiSnake) {
        if (segment.x == newHead.x && segment.y == newHead.y) {
            return false;
        }
    }
    
    // 检查是否撞到玩家蛇的身体
    if (gameWidget) {
        QList<Point> playerBody = gameWidget->getSnakeBody();
        for (const Point& segment : playerBody) {
            if (segment.x == newHead.x && segment.y == newHead.y) {
                return false;
            }
        }
        
        // 检查是否撞到障碍物（墙体）
        if (gameWidget->hasWallAt(newHead)) {
            return false;
        }
    }
    
    // 检查前方两格是否有障碍物（用于提前规避）
    Point twoStepsAhead = getNextPosition(newHead, direction);
    
    // 如果前方两格是边界，这个方向可能不太安全，但不完全禁止
    if (twoStepsAhead.x < 0 || twoStepsAhead.x >= 40 || 
        twoStepsAhead.y < 0 || twoStepsAhead.y >= 30) {
        // 边界情况下，只要当前步骤安全就允许移动
        return true;
    }
    
    for (const Point& segment : aiSnake) {
        if (segment.x == twoStepsAhead.x && segment.y == twoStepsAhead.y) {
            return false;
        }
    }
    
    // 检查前方两格是否撞到玩家蛇
    if (gameWidget) {
        QList<Point> playerBody = gameWidget->getSnakeBody();
        for (const Point& segment : playerBody) {
            if (segment.x == twoStepsAhead.x && segment.y == twoStepsAhead.y) {
                return false;
            }
        }
        
        // 检查前方两格是否有障碍物
        if (gameWidget->hasWallAt(twoStepsAhead)) {
            return false;
        }
    }
    
    return true;
}