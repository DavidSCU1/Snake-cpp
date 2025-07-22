#include "singleplayergamemanager.h"
#include "gamewidget.h"
#include <QDebug>
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
    achievements.append({"challenge_walls_50", "墙体大师", "挑战模式生成50块墙体", false, 0, 50});
    achievements.append({"speed_run_5x", "极速之王", "极速模式达到5倍速度", false, 0, 5});
    achievements.append({"ai_battle_win", "人机对战胜利者", "在人机对战中击败AI", false, 0, 1});
    achievements.append({"ai_battle_500", "AI挑战者", "人机对战模式获得500分", false, 0, 500});
    achievements.append({"ai_battle_master", "AI征服者", "人机对战模式获得1000分", false, 0, 1000});
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
    } else {
        aiSnake.pop_back();
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
    }
    
    return true;
}