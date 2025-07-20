#include "singleplayergamemanager.h"
#include "gamewidget.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QMessageBox>

SinglePlayerGameManager::SinglePlayerGameManager(QObject *parent)
    : QObject(parent)
    , currentMode(SinglePlayerMode::CLASSIC)
    , gameWidget(nullptr)
    , isGameActive(false)
    , isPaused(false)
    , timeAttackDuration(TIME_ATTACK_DEFAULT_DURATION)
    , survivalWave(1)
    , survivalEnemies(SURVIVAL_BASE_ENEMIES)
    , speedMultiplier(1.0)
    , puzzleLevel(1)
    , aiScore(0)
    , playerScore(0)
    , aiDirection(Direction::UP)
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
    if (currentMode != mode) {
        currentMode = mode;
        emit modeChanged(mode);
        
        // 重置模式特定数据
        switch (mode) {
        case SinglePlayerMode::TIME_ATTACK:
            timeAttackDuration = TIME_ATTACK_DEFAULT_DURATION;
            break;
        case SinglePlayerMode::SURVIVAL:
            survivalWave = 1;
            survivalEnemies = SURVIVAL_BASE_ENEMIES;
            break;
        case SinglePlayerMode::SPEED_RUN:
            speedMultiplier = 1.0;
            break;
        case SinglePlayerMode::PUZZLE:
            puzzleLevel = 1;
            puzzleTargets.clear();
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
}

QString SinglePlayerGameManager::getModeDescription(SinglePlayerMode mode) const
{
    switch (mode) {
    case SinglePlayerMode::CLASSIC:
        return "经典模式：传统的贪吃蛇游戏，吃食物增长身体，避免撞墙和自己。";
    case SinglePlayerMode::TIME_ATTACK:
        return "时间挑战：在限定时间内获得尽可能高的分数！";
    case SinglePlayerMode::SURVIVAL:
        return "生存模式：面对越来越多的障碍物，看你能坚持多久！";
    case SinglePlayerMode::SPEED_RUN:
        return "极速模式：速度会不断增加，考验你的反应能力！";
    case SinglePlayerMode::PUZZLE:
        return "解谜模式：按照指定路径收集食物，挑战你的策略思维！";
    case SinglePlayerMode::AI_BATTLE:
        return "人机对战：与智能AI比拼积分，看谁能获得更高的分数！";
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
        
        // 计算最终效率
        if (gameStats.timeElapsed > 0) {
            gameStats.efficiency = (double)gameStats.foodEaten / gameStats.timeElapsed * 100.0;
        }
        
        // 检查成就
        checkAchievements();
        
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
    case SinglePlayerMode::SURVIVAL:
        survivalWave = 1;
        survivalEnemies = SURVIVAL_BASE_ENEMIES;
        break;
    case SinglePlayerMode::SPEED_RUN:
        speedMultiplier = 1.0;
        break;
    case SinglePlayerMode::PUZZLE:
        puzzleLevel = 1;
        puzzleTargets.clear();
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

int SinglePlayerGameManager::getSurvivalWaves() const
{
    return survivalWave;
}

double SinglePlayerGameManager::getSpeedMultiplier() const
{
    return speedMultiplier;
}

int SinglePlayerGameManager::getPuzzleLevel() const
{
    return puzzleLevel;
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
    case SinglePlayerMode::SURVIVAL:
        updateSurvivalMode();
        break;
    case SinglePlayerMode::SPEED_RUN:
        updateSpeedRunMode();
        break;
    case SinglePlayerMode::PUZZLE:
        updatePuzzleMode();
        break;
    case SinglePlayerMode::AI_BATTLE:
        updateAIBattleMode();
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
    case SinglePlayerMode::SURVIVAL:
        // 每30秒增加一波敌人
        if (gameStats.timeElapsed % 30 == 0) {
            survivalWave++;
            survivalEnemies += 2;
            emit waveCompleted(survivalWave);
        }
        break;
    default:
        break;
    }
}

void SinglePlayerGameManager::onSpeedTimer()
{
    if (currentMode == SinglePlayerMode::SPEED_RUN) {
        speedMultiplier += SPEED_RUN_INCREMENT;
        emit speedIncreased(speedMultiplier);
    }
}

void SinglePlayerGameManager::initializeAchievements()
{
    achievements.clear();
    
    // 分数成就
    achievements.append({"score_100", "初学者", "获得100分", false, 0, 100});
    achievements.append({"score_500", "进步者", "获得500分", false, 0, 500});
    achievements.append({"score_1000", "高手", "获得1000分", false, 0, 1000});
    achievements.append({"score_5000", "大师", "获得5000分", false, 0, 5000});
    
    // 时间成就
    achievements.append({"time_60", "坚持者", "游戏时间超过1分钟", false, 0, 60});
    achievements.append({"time_300", "耐力王", "游戏时间超过5分钟", false, 0, 300});
    achievements.append({"time_600", "马拉松", "游戏时间超过10分钟", false, 0, 600});
    
    // 长度成就
    achievements.append({"length_20", "小蛇", "蛇身长度达到20", false, 0, 20});
    achievements.append({"length_50", "大蛇", "蛇身长度达到50", false, 0, 50});
    achievements.append({"length_100", "巨蛇", "蛇身长度达到100", false, 0, 100});
    
    // 特殊成就
    achievements.append({"perfect_10", "完美主义者", "连续10次完美移动", false, 0, 10});
    achievements.append({"special_food_10", "美食家", "吃掉10个特殊食物", false, 0, 10});
    achievements.append({"efficiency_80", "效率专家", "游戏效率达到80%", false, 0, 80});
    
    // 模式特定成就
    achievements.append({"time_attack_master", "时间大师", "时间挑战模式获得1000分", false, 0, 1000});
    achievements.append({"survival_wave_10", "生存专家", "生存模式坚持到第10波", false, 0, 10});
    achievements.append({"speed_run_5x", "极速之王", "极速模式达到5倍速度", false, 0, 5});
    achievements.append({"puzzle_level_10", "解谜高手", "解谜模式通过第10关", false, 0, 10});
    achievements.append({"ai_battle_win", "人机对战胜利者", "在人机对战中击败AI", false, 0, 1});
    achievements.append({"ai_battle_500", "AI挑战者", "人机对战模式获得500分", false, 0, 500});
    achievements.append({"ai_battle_master", "AI征服者", "人机对战模式获得1000分", false, 0, 1000});
}

void SinglePlayerGameManager::setupModeTimers()
{
    modeTimer->stop();
    speedTimer->stop();
    aiMoveTimer->stop();
    
    switch (currentMode) {
    case SinglePlayerMode::SURVIVAL:
        modeTimer->start(1000); // 每秒检查
        break;
    case SinglePlayerMode::SPEED_RUN:
        speedTimer->start(10000); // 每10秒增加速度
        break;
    case SinglePlayerMode::AI_BATTLE:
        aiMoveTimer->start(200); // AI每200ms移动一次
        initializeAI();
        break;
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

void SinglePlayerGameManager::updateSurvivalMode()
{
    // 生存模式的更新逻辑
    // 这里可以添加障碍物生成等逻辑
}

void SinglePlayerGameManager::updateSpeedRunMode()
{
    // 极速模式的更新逻辑
    // 速度增加在speedTimer中处理
}

void SinglePlayerGameManager::updatePuzzleMode()
{
    // 解谜模式的更新逻辑
    // 检查是否完成当前关卡
    if (puzzleTargets.isEmpty()) {
        puzzleLevel++;
        emit puzzleSolved(puzzleLevel - 1);
        
        // 生成新的解谜目标
        // 这里可以添加更复杂的解谜逻辑
    }
}

void SinglePlayerGameManager::checkAchievements()
{
    checkScoreAchievements();
    checkTimeAchievements();
    checkEfficiencyAchievements();
    checkSpecialAchievements();
}

void SinglePlayerGameManager::checkScoreAchievements()
{
    QStringList scoreAchievements = {"score_100", "score_500", "score_1000", "score_5000"};
    
    for (auto& achievement : achievements) {
        if (scoreAchievements.contains(achievement.id) && !achievement.unlocked) {
            if (gameStats.totalScore >= achievement.target) {
                achievement.unlocked = true;
                achievement.progress = achievement.target;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.totalScore;
            }
        }
    }
}

void SinglePlayerGameManager::checkTimeAchievements()
{
    QStringList timeAchievements = {"time_60", "time_300", "time_600"};
    
    for (auto& achievement : achievements) {
        if (timeAchievements.contains(achievement.id) && !achievement.unlocked) {
            if (gameStats.timeElapsed >= achievement.target) {
                achievement.unlocked = true;
                achievement.progress = achievement.target;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.timeElapsed;
            }
        }
    }
}

void SinglePlayerGameManager::checkEfficiencyAchievements()
{
    for (auto& achievement : achievements) {
        if (achievement.id == "efficiency_80" && !achievement.unlocked) {
            if (gameStats.efficiency >= achievement.target) {
                achievement.unlocked = true;
                achievement.progress = achievement.target;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = (int)gameStats.efficiency;
            }
        }
    }
}

void SinglePlayerGameManager::checkSpecialAchievements()
{
    // 检查长度成就
    QStringList lengthAchievements = {"length_20", "length_50", "length_100"};
    for (auto& achievement : achievements) {
        if (lengthAchievements.contains(achievement.id) && !achievement.unlocked) {
            if (gameStats.maxLength >= achievement.target) {
                achievement.unlocked = true;
                achievement.progress = achievement.target;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.maxLength;
            }
        }
    }
    
    // 检查特殊食物成就
    for (auto& achievement : achievements) {
        if (achievement.id == "special_food_10" && !achievement.unlocked) {
            if (gameStats.specialFoodEaten >= achievement.target) {
                achievement.unlocked = true;
                achievement.progress = achievement.target;
                emit achievementUnlocked(achievement);
            } else {
                achievement.progress = gameStats.specialFoodEaten;
            }
        }
    }
    
    // 检查模式特定成就
    if (currentMode == SinglePlayerMode::TIME_ATTACK) {
        for (auto& achievement : achievements) {
            if (achievement.id == "time_attack_master" && !achievement.unlocked) {
                if (gameStats.totalScore >= achievement.target) {
                    achievement.unlocked = true;
                    achievement.progress = achievement.target;
                    emit achievementUnlocked(achievement);
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
                    emit achievementUnlocked(achievement);
                }
            } else if (achievement.id == "ai_battle_500" && !achievement.unlocked) {
                if (gameStats.totalScore >= 500) {
                    achievement.unlocked = true;
                    achievement.progress = achievement.target;
                    emit achievementUnlocked(achievement);
                }
            } else if (achievement.id == "ai_battle_master" && !achievement.unlocked) {
                if (gameStats.totalScore >= 1000) {
                    achievement.unlocked = true;
                    achievement.progress = achievement.target;
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
                break;
            }
        }
    }
    settings->endArray();
}

// AI对战模式实现
void SinglePlayerGameManager::updateAIBattleMode()
{
    // AI对战模式的更新逻辑
    // 这里可以添加特殊的AI对战规则
    emit aiScoreUpdated(aiScore, playerScore);
}

void SinglePlayerGameManager::initializeAI()
{
    // 初始化AI蛇的位置（在游戏区域的右上角）
    aiSnake.clear();
    Point startPos = {35, 5}; // 假设游戏区域是40x30
    
    // 创建初始AI蛇身（3个段）
    for (int i = 0; i < 3; ++i) {
        Point segment = {startPos.x, startPos.y + i};
        aiSnake.push_back(segment);
    }
    
    aiDirection = Direction::DOWN;
    aiTarget = {20, 15}; // 初始目标位置
}

void SinglePlayerGameManager::updateAIMovement()
{
    if (!isGameActive || isPaused || aiSnake.empty()) {
        return;
    }
    
    // 计算AI的下一步移动
    Direction newDirection = calculateAIDirection();
    
    if (isValidAIMove(newDirection)) {
        aiDirection = newDirection;
    }
    
    // 移动AI蛇
    Point newHead = aiSnake.front();
    switch (aiDirection) {
    case Direction::UP:
        newHead.y--;
        break;
    case Direction::DOWN:
        newHead.y++;
        break;
    case Direction::LEFT:
        newHead.x--;
        break;
    case Direction::RIGHT:
        newHead.x++;
        break;
    }
    
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
    
    aiSnake.push_front(newHead);
    
    // 检查是否吃到食物（这里需要与游戏主逻辑配合）
    // 简化处理：随机增加AI分数
    if (QRandomGenerator::global()->bounded(100) < 5) { // 5%概率
        aiScore += 10;
        emit aiScoreUpdated(aiScore, playerScore);
    } else {
        aiSnake.pop_back();
    }
}

Direction SinglePlayerGameManager::calculateAIDirection()
{
    if (aiSnake.empty()) {
        return Direction::UP;
    }
    
    Point head = aiSnake.front();
    
    // 简单的AI逻辑：朝向目标移动
    Direction bestDirection = aiDirection;
    
    // 计算到目标的距离
    int dx = aiTarget.x - head.x;
    int dy = aiTarget.y - head.y;
    
    // 优先处理距离更大的轴
    if (abs(dx) > abs(dy)) {
        bestDirection = (dx > 0) ? Direction::RIGHT : Direction::LEFT;
    } else {
        bestDirection = (dy > 0) ? Direction::DOWN : Direction::UP;
    }
    
    // 如果计算出的方向与当前方向相反，保持当前方向
    if ((bestDirection == Direction::UP && aiDirection == Direction::DOWN) ||
        (bestDirection == Direction::DOWN && aiDirection == Direction::UP) ||
        (bestDirection == Direction::LEFT && aiDirection == Direction::RIGHT) ||
        (bestDirection == Direction::RIGHT && aiDirection == Direction::LEFT)) {
        bestDirection = aiDirection;
    }
    
    return bestDirection;
}

bool SinglePlayerGameManager::isValidAIMove(Direction direction)
{
    if (aiSnake.empty()) {
        return true;
    }
    
    Point head = aiSnake.front();
    Point newHead = head;
    
    switch (direction) {
    case Direction::UP:
        newHead.y--;
        break;
    case Direction::DOWN:
        newHead.y++;
        break;
    case Direction::LEFT:
        newHead.x--;
        break;
    case Direction::RIGHT:
        newHead.x++;
        break;
    }
    
    // 检查边界
    if (newHead.x < 0 || newHead.x >= 40 || newHead.y < 0 || newHead.y >= 30) {
        return false;
    }
    
    // 检查是否撞到自己的身体（除了尾部）
    for (size_t i = 0; i < aiSnake.size() - 1; ++i) {
        if (aiSnake[i].x == newHead.x && aiSnake[i].y == newHead.y) {
            return false;
        }
    }
    
    return true;
}