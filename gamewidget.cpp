#include "gamewidget.h"
#include "singleplayergamemanager.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QFont>
#include <QDebug>
#include <QApplication>
#include <QMetaObject>
#include <QCoreApplication>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , currentState(GameState::MENU)
    , currentDifficulty(Difficulty::NORMAL)
    , isMultiplayer(false)
    , isLocalCoop(false)
    , isHost(false)
    , snake(new Snake(this))
    , player2Snake(new Snake(this))
    , food(new Food(this))
    , wall(new Wall())
    , gameTimer(new QTimer(this))
    , specialFoodTimer(new QTimer(this))
    , countdownTimer(new QTimer(this))
    , gridWidth(40)
    , gridHeight(25)
    , cellSize(20)
    , score(0)
    , level(1)
    , baseSpeed(200)
    , currentSpeed(200)
    , remainingTime(TIME_CHALLENGE_DURATION)

    , singlePlayerManager(new SinglePlayerGameManager(this))
    , hotspotGameManager(nullptr)
    , player1Character(CharacterType::SPONGEBOB)
    , player2Character(CharacterType::PATRICK)
    , localCoopMode(SinglePlayerMode::CLASSIC)
    , player1Score(0)
    , player2Score(0)
    , player1Alive(true)
    , player2Alive(true)
    , player1Lives(MAX_LIVES)
    , player2Lives(MAX_LIVES)
    , player1Respawning(false)
    , player2Respawning(false)
    , player1RespawnTime(0)
    , player2RespawnTime(0)
    , respawnTimer(new QTimer(this))
    , gameTimeTimer(new QTimer(this))
    , totalGameTime(0)
    , settings(new QSettings("SnakeGame", "SpongeBobSnake", this))
    , specialFoodCounter(0)
{
    qDebug() << "GameWidget constructor called";
    setupUI();
    setupGame();
    loadHighScores();

    // 连接信号
    connect(gameTimer, &QTimer::timeout, this, &GameWidget::gameLoop);
    connect(food, &Food::foodExpired, this, &GameWidget::onFoodExpired);
    connect(countdownTimer, &QTimer::timeout, this, &GameWidget::updateCountdown);
    

    
    // 单人游戏管理器信号连接
    connect(singlePlayerManager, QOverload<SinglePlayerMode, const GameStats&>::of(&SinglePlayerGameManager::gameEnded), this, [this](SinglePlayerMode mode, const GameStats& stats) {
        qDebug() << "Single player game ended, mode:" << (int)mode;
        currentState = GameState::GAME_OVER;
        gameTimer->stop();
        update();
    });
    
    // AI对战模式游戏结束信号连接
    connect(singlePlayerManager, QOverload<const QString&>::of(&SinglePlayerGameManager::gameEnded), this, [this](const QString& message) {
        qDebug() << "AI Battle game ended:" << message;
        currentState = GameState::GAME_OVER;
        gameTimer->stop();
        // 显示游戏结束消息
        QMessageBox::information(this, "游戏结束", message);
        update();
    });
    
    // 连接成就解锁信号
    connect(singlePlayerManager, &SinglePlayerGameManager::achievementUnlocked, this, [this](const Achievement& achievement) {
        qDebug() << "Achievement unlocked:" << achievement.name;
        // 成就解锁后的处理逻辑已经在 QMetaObject::invokeMethod 中实现
    });
    
    // 连接极速模式速度增加信号
    connect(singlePlayerManager, &SinglePlayerGameManager::speedIncreased, this, [this](double speedMultiplier) {
        qDebug() << "Speed increased to multiplier:" << speedMultiplier;
        // 更新游戏速度
        int newSpeed = static_cast<int>(baseSpeed / speedMultiplier);
        currentSpeed = newSpeed;
        if (gameTimer->isActive()) {
            gameTimer->setInterval(newSpeed);
        }
    });
    
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(800, 600);
}

GameWidget::~GameWidget()
{
    qDebug() << "GameWidget destructor called";
    delete wall;
}

void GameWidget::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // 游戏区域占据整个窗口
    gameArea = new QWidget(this);
    gameArea->setMinimumSize(gridWidth * cellSize, gridHeight * cellSize);
    gameArea->setStyleSheet("");
    
    // 分数标签 - 左上角
    scoreLabel = new QLabel("分数: 0", this);
    scoreLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #FF6347; font-family: '华文彩云'; background-color: rgba(255, 255, 255, 200); padding: 5px; border-radius: 5px;");
    scoreLabel->move(10, 10);
    scoreLabel->raise();
    
    // 等级标签 - 左上角，分数下方
    levelLabel = new QLabel("等级: 1", this);
    levelLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #32CD32; font-family: '华文彩云'; background-color: rgba(255, 255, 255, 200); padding: 5px; border-radius: 5px;");
    levelLabel->move(10, 40);
    levelLabel->raise();
    
    // 时间标签（时间挑战模式时显示）- 游戏界面正上方中央
    timeLabel = new QLabel("Time: 05:00", this);
    timeLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #FF0000; font-family: Arial, sans-serif; background-color: rgba(255, 255, 255, 220); padding: 8px 15px; border-radius: 8px; border: 2px solid #FF0000;");
    timeLabel->setVisible(false);
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setFixedSize(150, 40);  // 设置固定大小确保显示完整
    timeLabel->raise();
    
    // 玩家标签（多人游戏时显示）
    playersLabel = new QLabel("在线玩家:", this);
    playersLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #4169E1; font-family: '华文彩云'; background-color: rgba(255, 255, 255, 200); padding: 5px; border-radius: 5px;");
    playersLabel->setVisible(false);
    playersLabel->move(10, 70);
    playersLabel->raise();
    
    // 玩家列表
    playersList = new QListWidget(this);
    playersList->setMaximumHeight(150);
    playersList->setMaximumWidth(200);
    playersList->setStyleSheet("background-color: rgba(255, 255, 255, 200); border: 1px solid #ccc; border-radius: 5px;");
    playersList->setVisible(false);
    playersList->move(10, 100);
    playersList->raise();
    
    // 暂停按钮 - 右上角
    pauseButton = new QPushButton("暂停", this);
    pauseButton->setStyleSheet("QPushButton { background-color: #FFA500; color: white; border: none; border-radius: 5px; padding: 8px; font-size: 12px; font-family: '华文彩云'; }"
                               "QPushButton:hover { background-color: #FF8C00; }"
                               "QPushButton:pressed { background-color: #FF7F00; }");
    pauseButton->setFixedSize(100, 45);
    connect(pauseButton, &QPushButton::clicked, [this]() {
        if (currentState == GameState::PLAYING) {
            pauseGame();
        } else if (currentState == GameState::PAUSED) {
            resumeGame();
        }
    });
    pauseButton->raise();
    
    // 返回菜单按钮 - 右下角
    menuButton = new QPushButton("返回菜单", this);
    menuButton->setStyleSheet("QPushButton { background-color: #DC143C; color: white; border: none; border-radius: 5px; padding: 8px; font-size: 12px; font-family: '华文彩云'; }"
                              "QPushButton:hover { background-color: #B22222; }"
                              "QPushButton:pressed { background-color: #A0522D; }");
    menuButton->setFixedSize(120, 45);
    connect(menuButton, &QPushButton::clicked, [this]() {
        resetGame();
        emit backToMenu();
    });
    menuButton->raise();
    
    mainLayout->addWidget(gameArea);
    
    // 确保游戏区域尺寸正确
    updateGameArea();
    
    // 初始化按钮位置
    updateButtonPositions();
}

void GameWidget::setupGame()
{
    // 设置游戏计时器
    specialFoodTimer->setSingleShot(true);
    connect(specialFoodTimer, &QTimer::timeout, [this]() {
        generateSpecialFood();
    });
    
    // 设置复活计时器
    connect(respawnTimer, &QTimer::timeout, this, &GameWidget::updateRespawnTimer);
    
    // 设置游戏总时间计时器
    connect(gameTimeTimer, &QTimer::timeout, this, &GameWidget::updateGameTimer);
}

void GameWidget::setCharacter(CharacterType character)
{
    snake->setCharacter(character);
}

void GameWidget::setDifficulty(Difficulty difficulty)
{
    currentDifficulty = difficulty;
    
    switch (difficulty) {
    case Difficulty::EASY:
        baseSpeed = 300;
        break;
    case Difficulty::NORMAL:
        baseSpeed = 200;
        break;
    case Difficulty::HARD:
        baseSpeed = 100;
        break;
    }
    
    currentSpeed = baseSpeed;
}

void GameWidget::setSinglePlayerGameMode(SinglePlayerMode mode)
{
    if (singlePlayerManager) {
        singlePlayerManager->setGameMode(mode);
    }
}

void GameWidget::startSinglePlayerGame()
{
    qDebug() << "startSinglePlayerGame called";
    isMultiplayer = false;
    
    // 确保UI组件已初始化后再使用
    if (playersLabel) playersLabel->setVisible(false);
    if (playersList) playersList->setVisible(false);
    
    // 根据游戏模式显示相应的UI组件
    
    // 重置游戏数据但不改变状态
    gameTimer->stop();
    specialFoodTimer->stop();
    
    score = 0;
    level = 1;
    currentSpeed = baseSpeed;
    specialFoodCounter = 0;
    
    if (scoreLabel) scoreLabel->setText("分数: 0");
    if (levelLabel) levelLabel->setText("等级: 1");
    if (pauseButton) pauseButton->setText("暂停");
    
    // 清理多人游戏数据
    otherPlayers.clear();
    playerCharacters.clear();
    playerScores.clear();
    playerAliveStatus.clear();
    playersList->clear();
    
    // 清空墙体 - 确保每次开始新游戏时墙体都被清理
    if (wall) wall->clear();
    
    // 设置正确的游戏状态
    currentState = GameState::PLAYING;
    
    // 确保窗口可见
    show();
    setFocus();
    qDebug() << "GameWidget shown and focused. Size:" << size() << "Visible:" << isVisible();
    
    // 初始化蛇的位置
    Point startPos(gridWidth / 2, gridHeight / 2);
    qDebug() << "Snake reset to position:" << startPos.x << "," << startPos.y;
    snake->reset(startPos);
    
    // 生成第一个食物
    generateFood();
    qDebug() << "Food generated at:" << food->getPosition().x << "," << food->getPosition().y;
    
    // 启动单人游戏管理器
    if (singlePlayerManager) {
        singlePlayerManager->startGame(this);
        
        // 在AI对战模式下初始化AI蛇
        if (singlePlayerManager->getCurrentMode() == SinglePlayerMode::AI_BATTLE) {
            singlePlayerManager->initializeAI();
            singlePlayerManager->setFoodPosition(food->getPosition());
        }
    }
    
    // 生成墙体（在经典模式、时间挑战模式和极速模式下）
    if (singlePlayerManager && 
        (singlePlayerManager->getCurrentMode() == SinglePlayerMode::CLASSIC ||
         singlePlayerManager->getCurrentMode() == SinglePlayerMode::TIME_ATTACK ||
         singlePlayerManager->getCurrentMode() == SinglePlayerMode::SPEED_RUN) &&
        (currentDifficulty == Difficulty::NORMAL || currentDifficulty == Difficulty::HARD)) {
        generateWalls();
        qDebug() << "Walls generated, count:" << wall->getWallPositions().size();
    }
    
    // 检查是否为时间挑战模式，启动倒计时
    if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::TIME_ATTACK) {
        // 重置倒计时时间
        remainingTime = TIME_CHALLENGE_DURATION;
        
        // 显示时间标签
        if (timeLabel) {
            timeLabel->setVisible(true);
            timeLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #00FF00; font-family: Arial, sans-serif; background-color: rgba(0, 0, 0, 180); padding: 8px 15px; border-radius: 8px; border: 2px solid #00FF00;");
            
            // 初始化时间显示
            int minutes = remainingTime / 60;
            int seconds = remainingTime % 60;
            QString timeText = QString("Time: %1:%2")
                              .arg(minutes, 2, 10, QChar('0'))
                              .arg(seconds, 2, 10, QChar('0'));
            timeLabel->setText(timeText);
        }
        
        // 启动倒计时器
        countdownTimer->start(1000); // 每秒更新一次
        qDebug() << "Time challenge mode: countdown timer started";
    } else {
        // 非时间挑战模式，隐藏时间标签
        if (timeLabel) {
            timeLabel->setVisible(false);
        }
        countdownTimer->stop();
    }
    
    // 启动游戏循环
    gameTimer->start(currentSpeed);
    qDebug() << "Game timer started with speed:" << currentSpeed;
    
    qDebug() << "Game started, initial update will be triggered by paint event";
}

void GameWidget::startMultiPlayerGame(bool isHost)
{
    this->isHost = isHost;
    isMultiplayer = true;
    
    // 确保UI组件已初始化后再使用
    if (playersLabel) playersLabel->setVisible(true);
    if (playersList) playersList->setVisible(true);
    
    // 多人游戏模式已移除，此函数暂时保留但不执行任何操作
    
    // 重置游戏数据但不改变状态
    gameTimer->stop();
    specialFoodTimer->stop();
    
    score = 0;
    level = 1;
    currentSpeed = baseSpeed;
    specialFoodCounter = 0;
    
    if (scoreLabel) scoreLabel->setText("分数: 0");
    if (levelLabel) levelLabel->setText("等级: 1");
    if (pauseButton) pauseButton->setText("暂停");
    
    // 设置正确的游戏状态
    currentState = GameState::MULTIPLAYER_GAME;
    
    // 初始化蛇的位置
    Point startPos(gridWidth / 2, gridHeight / 2);
    snake->reset(startPos);
    
    // 生成第一个食物
    generateFood();
    
    // 生成墙体（多人模式使用经典模式的墙机制）
    if (currentDifficulty == Difficulty::NORMAL || currentDifficulty == Difficulty::HARD) {
        generateWalls();
        qDebug() << "Multiplayer walls generated, count:" << wall->getWallPositions().size();
    }
    
    update();
}

void GameWidget::pauseGame()
{
    if (currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) {
        gameTimer->stop();
        specialFoodTimer->stop();
        countdownTimer->stop();  // 暂停倒计时器
        
        // 暂停AI移动定时器（如果在AI对战模式）
        if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::AI_BATTLE) {
            singlePlayerManager->pauseAI();
        }
        
        currentState = GameState::PAUSED;
        if (pauseButton) pauseButton->setText("继续");
        update();
    }
}

void GameWidget::resumeGame()
{
    if (currentState == GameState::PAUSED) {
        if (isMultiplayer) {
            currentState = GameState::MULTIPLAYER_GAME;
        } else {
            currentState = GameState::PLAYING;
        }
        gameTimer->start(currentSpeed);
        
        // 如果是时间挑战模式，重新启动倒计时器
        if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::TIME_ATTACK && remainingTime > 0) {
            countdownTimer->start(1000);
        }
        
        // 恢复AI移动定时器（如果在AI对战模式）
        if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::AI_BATTLE) {
            singlePlayerManager->resumeAI();
        }
        
        if (pauseButton) pauseButton->setText("暂停");
        update();
    }
}

void GameWidget::resetGame()
{
    gameTimer->stop();
    specialFoodTimer->stop();
    countdownTimer->stop();  // 停止倒计时器
    
    // 隐藏时间标签
    if (timeLabel) {
        timeLabel->setVisible(false);
    }
    
    score = 0;
    level = 1;
    currentSpeed = baseSpeed;
    specialFoodCounter = 0;
    
    if (scoreLabel) scoreLabel->setText("分数: 0");
    if (levelLabel) levelLabel->setText("等级: 1");
    if (pauseButton) pauseButton->setText("暂停");
    
    // 清理多人游戏数据
    otherPlayers.clear();
    playerCharacters.clear();
    playerScores.clear();
    playerAliveStatus.clear();
    if (playersList) playersList->clear();
    
    // 清空墙体
    if (wall) wall->clear();
    
    // 重置本地双人游戏状态
    isLocalCoop = false;
    player1Score = 0;
    player2Score = 0;
    player1Alive = true;
    player2Alive = true;
    player1Character = CharacterType::SPONGEBOB;
    player2Character = CharacterType::PATRICK;
    
    // 旧联机模式相关代码已移除
    
    isMultiplayer = false;
    currentState = GameState::MENU;
    update();
}

void GameWidget::gameLoop()
{
    if (!snake) {
        qCritical() << "Snake object is null in gameLoop";
        return;
    }
    
    if (currentState != GameState::PLAYING && currentState != GameState::MULTIPLAYER_GAME) {
        return;
    }
    
    if (isLocalCoop) {
        // 本地双人游戏模式
        if (player1Alive) {
            snake->move();
        }
        if (player2Alive && player2Snake) {
            player2Snake->move();
        }
        
        checkLocalCoopCollisions();
    } else if (currentState == GameState::MULTIPLAYER_GAME) {
        // 旧多人游戏模式已移除，暂时保留状态检查
        snake->move();
        checkCollisions();
    } else {
        // 单人游戏模式
        snake->move();
        
        // AI蛇通过独立的定时器控制移动，不在这里调用
        
        checkCollisions();
    }
    
    update();
}

void GameWidget::checkCollisions()
{
    if (!snake || !food) {
        qCritical() << "Collision check with null pointers! snake:" << snake << "food:" << food;
        return;
    }
    if (!snake || !food) {
        qDebug() << "Snake or Food object is null in checkCollisions";
        return;
    }
    
    Point head = snake->getHead();
    
    // 检查边界碰撞
    if (snake->checkBoundaryCollision(gridWidth, gridHeight)) {
        currentState = GameState::GAME_OVER;
        gameTimer->stop();
        specialFoodTimer->stop();
        saveHighScore();
        
        // 结束游戏并显示成就
        if (singlePlayerManager) {
            singlePlayerManager->endGame();
        }
        
        emit gameOver(score);
        update();
        return;
    }
    
    // 检查自身碰撞
    if (snake->checkSelfCollision()) {
        currentState = GameState::GAME_OVER;
        gameTimer->stop();
        specialFoodTimer->stop();
        saveHighScore();
        
        // 结束游戏并显示成就
        if (singlePlayerManager) {
            singlePlayerManager->endGame();
        }
        
        emit gameOver(score);
        update();
        return;
    }
    
    // 检查墙体碰撞
    if (wall && wall->hasWallAt(head)) {
        currentState = GameState::GAME_OVER;
        gameTimer->stop();
        specialFoodTimer->stop();
        
        // 设置撞墙而死的标志
        if (singlePlayerManager) {
            GameStats stats = singlePlayerManager->getGameStats();
            stats.diedByWallCollision = true;
            singlePlayerManager->updateGameStats(stats);
            
            // 结束游戏并显示成就
            singlePlayerManager->endGame();
        }
        
        saveHighScore();
        emit gameOver(score);
        update();
        return;
    }
    
    // 检查与AI蛇的碰撞（AI对战模式）
    if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::AI_BATTLE) {
        const auto& aiSnakeBody = singlePlayerManager->getAISnake();
        for (const auto& aiSegment : aiSnakeBody) {
            if (head.x == aiSegment.x && head.y == aiSegment.y) {
                currentState = GameState::GAME_OVER;
                gameTimer->stop();
                specialFoodTimer->stop();
                saveHighScore();
                
                // 结束游戏并显示成就
                if (singlePlayerManager) {
                    singlePlayerManager->endGame();
                }
                
                emit gameOver(score);
                update();
                return;
            }
        }
    }
    
    // 检查食物碰撞
    if (head == food->getPosition()) {
        snake->grow();
        int points = food->getValue();
        updateScore(points);
        
        specialFoodCounter++;
        if (specialFoodCounter >= SPECIAL_FOOD_INTERVAL) {
            specialFoodCounter = 0;
            specialFoodTimer->start(QRandomGenerator::global()->bounded(3000, 8000)); // 3-8秒后生成特殊食物
        }
        
        generateFood();
        updateSpeed();
        
        // 挑战模式：每吃一个食物生成5块墙
        if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::CHALLENGE) {
            QSet<Point> occupiedPositions = getOccupiedPositions();
            wall->generateChallengeWalls(5, gridWidth, gridHeight, occupiedPositions);
            
            // 检查墙体成就
            int totalWalls = wall->getWallPositions().size();
            qDebug() << "Challenge mode: Total walls now:" << totalWalls;
            
            // 检查是否达成墙体成就 - 使用 QMetaObject::invokeMethod 在主线程中显示消息框
            QList<Achievement> achievements = singlePlayerManager->getAchievements();
            bool achievementUpdated = false;
            
            for (int i = 0; i < achievements.size(); i++) {
                if (achievements[i].id == "challenge_walls_50") {
                    if (!achievements[i].unlocked && totalWalls >= achievements[i].target) {
                        // 解锁成就
                        achievements[i].unlocked = true;
                        achievements[i].progress = totalWalls;
                        achievementUpdated = true;
                        
                        // 创建一个副本用于信号发送，但不立即显示
                        Achievement achievementCopy = achievements[i];
                        achievementCopy.displayed = false; // 标记为未显示
                        achievements[i].displayed = false; // 标记为未显示
                        
                        // 发送成就解锁信号
                        emit singlePlayerManager->achievementUnlocked(achievementCopy);
                    } else if (!achievements[i].unlocked) {
                        // 更新进度
                        achievements[i].progress = totalWalls;
                        achievementUpdated = true;
                    }
                    break;
                }
            }
            
            // 如果成就有更新，保存回 singlePlayerManager
            if (achievementUpdated) {
                singlePlayerManager->updateAchievements(achievements);
            }
        }
        
        // 更新AI蛇的目标食物位置
        if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::AI_BATTLE) {
            singlePlayerManager->setFoodPosition(food->getPosition());
        }
        
        // 旧多人游戏模式相关代码已移除
    }
}

void GameWidget::generateFood()
{
    QSet<Point> occupiedPositions = getOccupiedPositions();
    
    // 在本地双人模式中，有20%的概率生成特殊食物
    if (isLocalCoop && QRandomGenerator::global()->bounded(100) < 20) {
        food->generateSpecialFood(gridWidth, gridHeight, occupiedPositions);
    } else {
        food->generateFood(gridWidth, gridHeight, occupiedPositions);
    }

    if (isMultiplayer) {
        // In multiplayer, schedule the next food to appear faster
        QTimer::singleShot(3000, this, &GameWidget::generateFood); 
    }
    
    // 更新AI蛇的目标食物位置
    if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::AI_BATTLE) {
        singlePlayerManager->setFoodPosition(food->getPosition());
    }
}

void GameWidget::generateSpecialFood()
{
    QSet<Point> occupiedPositions = getOccupiedPositions();
    food->generateSpecialFood(gridWidth, gridHeight, occupiedPositions);
}

void GameWidget::generateWalls()
{
    QSet<Point> occupiedPositions = getOccupiedPositions();
    
    // 添加食物位置到占用位置集合
    if (food) {
        Point foodPos = food->getPosition();
        if (foodPos.x >= 0 && foodPos.y >= 0) {
            occupiedPositions.insert(foodPos);
        }
    }

    int wallCount = isMultiplayer ? 20 : 0; // 多人模式使用固定数量，单人模式使用随机范围(100-150)
    
    wall->generateWalls(gridWidth, gridHeight, occupiedPositions, wallCount);
}

void GameWidget::generateWalls(const QSet<Point>& occupiedPositions, int wallCount)
{
    if (wall) {
        wall->generateWalls(gridWidth, gridHeight, occupiedPositions, wallCount);
    }
}

bool GameWidget::hasWallAt(const Point& position) const
{
    return wall ? wall->hasWallAt(position) : false;
}

QSet<Point> GameWidget::getOccupiedPositions() const
{
    QSet<Point> positions;
    
    // 添加蛇身位置
    if (snake) {
        for (const auto& point : snake->getBody()) {
            positions.insert(point);
        }
    }
    
    // 添加其他玩家的蛇身位置（多人游戏）
    for (const auto& playerSnake : otherPlayers) {
        for (const auto& point : playerSnake) {
            positions.insert(point);
        }
    }
    
    // 添加本地双人游戏第二个玩家的蛇身位置
    if (isLocalCoop && player2Snake) {
        for (const auto& point : player2Snake->getBody()) {
            positions.insert(point);
        }
    }
    
    // 添加AI蛇的位置（AI对战模式）
    if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::AI_BATTLE) {
        const auto& aiSnakeBody = singlePlayerManager->getAISnake();
        for (const auto& point : aiSnakeBody) {
            positions.insert(point);
        }
    }
    
    // 添加墙体位置
    if (wall) {
        for (const Point& wallPos : wall->getWallPositions()) {
            positions.insert(wallPos);
        }
    }
    
    return positions;
}

void GameWidget::updateScore(int points)
{
    score += points;
    scoreLabel->setText(QString("分数: %1").arg(score));
    emit scoreChanged(score);
}

void GameWidget::updateSpeed()
{
    int newLevel = (score / 100) + 1;
    if (newLevel != level) {
        level = newLevel;
        levelLabel->setText(QString("等级: %1").arg(level));
        
        // 极速模式下使用不同的速度增加机制
        if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::SPEED_RUN) {
            // 极速模式：每吃食物增加少量速度倍增器
            double currentMultiplier = singlePlayerManager->getSpeedMultiplier();
            double newMultiplier = currentMultiplier + 0.05; // 每吃食物增加5%速度
            singlePlayerManager->setSpeedMultiplier(newMultiplier);
            
            // 更新游戏速度
            int newSpeed = static_cast<int>(baseSpeed / newMultiplier);
            currentSpeed = qMax(30, newSpeed); // 最快不超过30ms间隔
            if (gameTimer->isActive()) {
                gameTimer->setInterval(currentSpeed);
            }
            return;
        }
        
        // 每升一级，速度增加10%
        currentSpeed = qMax(50, static_cast<int>(baseSpeed * qPow(0.9, level - 1)));
        if (gameTimer->isActive()) {
            gameTimer->setInterval(currentSpeed);
        }
        
        // 如果是AI对战模式，同步更新AI蛇的移动速度
        if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::AI_BATTLE) {
            singlePlayerManager->updateAISpeed(currentSpeed);
        }
    }
}

void GameWidget::saveHighScore()
{
    highScores.append(score);
    std::sort(highScores.begin(), highScores.end(), std::greater<int>());
    
    // 只保留前10名
    if (highScores.size() > 10) {
        highScores = highScores.mid(0, 10);
    }
    
    // 保存到设置
    settings->beginWriteArray("highScores");
    for (int i = 0; i < highScores.size(); ++i) {
        settings->setArrayIndex(i);
        settings->setValue("score", highScores[i]);
    }
    settings->endArray();
}

void GameWidget::loadHighScores()
{
    highScores.clear();
    int size = settings->beginReadArray("highScores");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        highScores.append(settings->value("score", 0).toInt());
    }
    settings->endArray();
}

void GameWidget::sendNetworkUpdate()
{
    // 旧联机模式相关代码已移除
}

QList<Point> GameWidget::getSnakeBody() const
{
    QList<Point> body;
    if (snake) {
        const auto& snakeBody = snake->getBody();
        for (const Point& point : snakeBody) {
            body.append(point);
        }
    }
    return body;
}



void GameWidget::onFoodExpired()
{
    generateFood(); // 特殊食物过期后生成普通食物
}

// 绘制函数
void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 计算游戏区域的实际位置 - 使用gameArea的位置和大小
    QRect gameRect = gameArea->geometry();
    
    if (currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME || currentState == GameState::PAUSED) {
        // 保存当前状态
        painter.save();
        
        // 设置绘制区域
        painter.setClipRect(gameRect);
        
        // 绘制网格（确保在背景之上）
        drawGrid(painter, gameRect);
        
        // 绘制食物
        drawFood(painter, gameRect);
        
        // 绘制墙体
        drawWalls(painter, gameRect);
        
        // 绘制蛇
        if (isLocalCoop) {
            drawLocalCoopSnakes(painter, gameRect);
        } else {
            drawSnake(painter, gameRect);
        }
        
        // 绘制其他玩家的蛇（多人游戏）或AI蛇（AI对战模式）
        if (isMultiplayer || (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::AI_BATTLE)) {
            drawMultiplayerSnakes(painter, gameRect);
        }
        
        // 恢复状态
        painter.restore();
        
        // 绘制游戏边框（在最上层）
        painter.setPen(QPen(Qt::black, 3)); // 黑色边框，3像素宽度
        painter.drawRect(gameRect);
        
        // 绘制本地双人游戏的分数和状态
        if (isLocalCoop) {
            painter.setClipRect(rect()); // 重置裁剪区域以绘制UI
            
            // 绘制玩家状态面板
            drawPlayerStatusPanel(painter);
        }
        

    }
    
    // 绘制UI覆盖层
    if (currentState == GameState::PAUSED) {
        drawPauseOverlay(painter, gameRect);
    } else if (currentState == GameState::GAME_OVER) {
        drawGameOverOverlay(painter, gameRect);
    }
}

void GameWidget::drawGrid(QPainter& painter, const QRect& gameRect)
{
    painter.setPen(QPen(QColor(100, 149, 237, 100), 1)); // 半透明网格线
    
    // 绘制垂直线
    for (int x = 0; x <= gridWidth; ++x) {
        int pixelX = gameRect.x() + x * cellSize;
        painter.drawLine(pixelX, gameRect.y(), pixelX, gameRect.y() + gridHeight * cellSize);
    }
    
    // 绘制水平线
    for (int y = 0; y <= gridHeight; ++y) {
        int pixelY = gameRect.y() + y * cellSize;
        painter.drawLine(gameRect.x(), pixelY, gameRect.x() + gridWidth * cellSize, pixelY);
    }
}

void GameWidget::drawSnake(QPainter& painter, const QRect& gameRect)
{
    const auto& body = snake->getBody();
    
    if (body.empty()) {
        qDebug() << "Snake body is empty, cannot draw";
        return;
    }
    
    qDebug() << "Drawing snake with" << body.size() << "segments";
    
    // 绘制蛇头
    Point head = body.front();
    QRect headRect(gameRect.x() + head.x * cellSize, 
                   gameRect.y() + head.y * cellSize, 
                   cellSize, cellSize);

    QPixmap headPixmap = snake->getHeadPixmap();
    if (!headPixmap.isNull()) {
        painter.drawPixmap(headRect, headPixmap);
        qDebug() << "Drew snake head with pixmap at" << headRect;
    } else {
        painter.fillRect(headRect, Qt::darkGreen);
        qDebug() << "Drew snake head with color at" << headRect;
    }

    // 绘制蛇身
    QPixmap bodyPixmap = snake->getBodyPixmap();
    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        // 根据角色动态计算身体大小：海绵宝宝100像素，其他角色50像素
        int maxBodySize = (snake->getCharacter() == CharacterType::SPONGEBOB) ? 100 : 50;
        int bodySize = qMin(maxBodySize, cellSize);
        int bodyOffset = (cellSize - bodySize) / 2;
        QRect bodyRect(gameRect.x() + it->x * cellSize + bodyOffset, 
                       gameRect.y() + it->y * cellSize + bodyOffset, 
                       bodySize, bodySize);
        
        if (!bodyPixmap.isNull()) {
            painter.drawPixmap(bodyRect, bodyPixmap);
        } else {
            painter.fillRect(bodyRect, Qt::green);
        }
    }
}

void GameWidget::drawFood(QPainter& painter, const QRect& gameRect)
{
    Point foodPos = food->getPosition();
    
    QRect foodRect(gameRect.x() + foodPos.x * cellSize + 2, 
                   gameRect.y() + foodPos.y * cellSize + 2, 
                   cellSize - 4, cellSize - 4);
    
    QPixmap foodPixmap = food->getPixmap();
    if (!foodPixmap.isNull()) {
        painter.drawPixmap(foodRect, foodPixmap);
    } else {
        if (food->isSpecial()) {
            painter.fillRect(foodRect, Qt::yellow);
            painter.setPen(Qt::red);
            painter.drawText(foodRect, Qt::AlignCenter, "★");
        } else {
            painter.fillRect(foodRect, Qt::red);
        }
    }
}

void GameWidget::drawWalls(QPainter& painter, const QRect& gameRect)
{
    if (!wall) return;
    
    const QSet<Point>& wallPositions = wall->getWallPositions();
    
    painter.setBrush(QBrush(Qt::darkGray));
    painter.setPen(QPen(Qt::black, 2));
    
    for (const Point& wallPos : wallPositions) {
        QRect wallRect(gameRect.x() + wallPos.x * cellSize + 1,
                       gameRect.y() + wallPos.y * cellSize + 1,
                       cellSize - 2, cellSize - 2);
        
        // 绘制墙体，使用深灰色填充，黑色边框
        painter.fillRect(wallRect, Qt::darkGray);
        painter.drawRect(wallRect);
        
        // 添加一些纹理效果
        painter.setPen(QPen(Qt::lightGray, 1));
        painter.drawLine(wallRect.topLeft() + QPoint(2, 2), 
                        wallRect.bottomRight() - QPoint(2, 2));
        painter.drawLine(wallRect.topRight() + QPoint(-2, 2), 
                        wallRect.bottomLeft() + QPoint(2, -2));
        painter.setPen(QPen(Qt::black, 2));
    }
}

void GameWidget::drawMultiplayerSnakes(QPainter& painter, const QRect& gameRect)
{
    // 绘制AI蛇（如果在AI对战模式中）
    if (singlePlayerManager && singlePlayerManager->getCurrentMode() == SinglePlayerMode::AI_BATTLE) {
        const auto& aiSnakeBody = singlePlayerManager->getAISnake();
        if (!aiSnakeBody.empty()) {
            // 使用Snake类来渲染AI蛇
            Snake aiSnakeRenderer(this);
            aiSnakeRenderer.setCharacter(singlePlayerManager->getAISnakeCharacter()); // 使用AI专属角色
            aiSnakeRenderer.setBody(aiSnakeBody);
            // 设置AI蛇的当前方向，避免身体斜着显示
            aiSnakeRenderer.setCurrentDirection(singlePlayerManager->getAIDirection());
            
            // 绘制AI蛇
            Point aiHead = aiSnakeBody.front();
            QRect headRect(gameRect.x() + aiHead.x * cellSize,
                          gameRect.y() + aiHead.y * cellSize,
                          cellSize, cellSize);
            
            // 使用Snake类的绘制方法
            QPixmap headPixmap = aiSnakeRenderer.getHeadPixmap();
            QPixmap bodyPixmap = aiSnakeRenderer.getBodyPixmap();
            
            // 绘制头部
            if (!headPixmap.isNull()) {
                painter.drawPixmap(headRect, headPixmap);
            } else {
                // 备用绘制：使用颜色
                painter.fillRect(headRect, QColor(Qt::magenta).darker(120));
            }
            
            // 绘制身体
            for (size_t i = 1; i < aiSnakeBody.size(); ++i) {
                const Point& segment = aiSnakeBody[i];
                int bodySize = (singlePlayerManager->getAISnakeCharacter() == CharacterType::SPONGEBOB) ? 100 : 50;
                bodySize = qMin(bodySize, cellSize);
                int bodyOffset = (cellSize - bodySize) / 2;
                QRect bodyRect(gameRect.x() + segment.x * cellSize + bodyOffset,
                              gameRect.y() + segment.y * cellSize + bodyOffset,
                              bodySize, bodySize);
                
                if (!bodyPixmap.isNull()) {
                    painter.drawPixmap(bodyRect, bodyPixmap);
                } else {
                    // 备用绘制：使用颜色
                    painter.fillRect(bodyRect, QColor(Qt::magenta));
                }
            }
            
            // 绘制AI标签
            QRect nameRect(gameRect.x() + aiHead.x * cellSize,
                          gameRect.y() + aiHead.y * cellSize - 15,
                          cellSize * 3, 15);
            painter.setPen(Qt::black);
            painter.setFont(QFont("华文彩云", 8));
            painter.drawText(nameRect, Qt::AlignCenter, "AI");
        }
    }
    
    // 绘制其他多人游戏玩家的蛇
    for (auto it = otherPlayers.begin(); it != otherPlayers.end(); ++it) {
        const QString& playerName = it.key();
        const std::deque<Point>& body = it.value();
        
        if (body.empty() || !playerAliveStatus.value(playerName, true)) continue;
        
        // 获取玩家角色类型，设置不同颜色
        CharacterType character = playerCharacters.value(playerName, CharacterType::PATRICK);
        QColor playerColor;
        
        switch (character) {
        case CharacterType::SPONGEBOB: playerColor = Qt::yellow; break;
        case CharacterType::PATRICK: playerColor = Qt::magenta; break;
        case CharacterType::SQUIDWARD: playerColor = Qt::cyan; break;
        case CharacterType::SANDY: playerColor = QColor(139, 69, 19); break;
        case CharacterType::MR_KRABS: playerColor = Qt::red; break;
        case CharacterType::PLANKTON: playerColor = Qt::green; break;
        }
        
        // 绘制其他玩家的蛇
        for (size_t i = 0; i < body.size(); ++i) {
            const Point& point = body[i];
            
            if (i == 0) {
                // 蛇头，使用完整的cellSize
                QRect headRect(gameRect.x() + point.x * cellSize, 
                               gameRect.y() + point.y * cellSize, 
                               cellSize, cellSize);
                painter.fillRect(headRect, playerColor.darker(120));
            } else {
                // 蛇身，根据角色动态计算身体大小：海绵宝宝100像素，其他角色50像素
                int maxBodySize = (character == CharacterType::SPONGEBOB) ? 100 : 50;
                int bodySize = qMin(maxBodySize, cellSize);
                int bodyOffset = (cellSize - bodySize) / 2;
                QRect bodyRect(gameRect.x() + point.x * cellSize + bodyOffset, 
                               gameRect.y() + point.y * cellSize + bodyOffset, 
                               bodySize, bodySize);
                painter.fillRect(bodyRect, playerColor);
            }
        }
        
        // 绘制玩家名称
        if (!body.empty()) {
            Point head = body.front();
            QRect nameRect(gameRect.x() + head.x * cellSize, 
                           gameRect.y() + head.y * cellSize - 15, 
                           cellSize * 3, 15);
            painter.setPen(Qt::black);
            painter.setFont(QFont("华文彩云", 8));
            painter.drawText(nameRect, Qt::AlignCenter, playerName);
        }
    }
}

void GameWidget::drawPauseOverlay(QPainter& painter, const QRect& gameRect)
{
    // 半透明背景
    painter.fillRect(gameRect, QColor(0, 0, 0, 128));
    
    // 暂停文字
    painter.setPen(Qt::white);
    QFont font("华文彩云", 24);
    font.setBold(true);
    painter.setFont(font);
    
    painter.drawText(gameRect, Qt::AlignCenter, "游戏暂停\n按空格键继续");
}

void GameWidget::drawGameOverOverlay(QPainter& painter, const QRect& gameRect)
{
    // 半透明背景
    painter.fillRect(gameRect, QColor(0, 0, 0, 150));
    
    // 游戏结束文字
    painter.setPen(Qt::red);
    QFont font("华文彩云", 28);
    font.setBold(true);
    painter.setFont(font);
    
    QString gameOverText = "游戏结束!\n";
    gameOverText += QString("最终分数: %1\n").arg(score);
    gameOverText += QString("等级: %1\n\n").arg(level);
    gameOverText += "按 R 键重新开始\n按 ESC 键返回菜单";
    
    painter.drawText(gameRect, Qt::AlignCenter, gameOverText);
}


void GameWidget::keyPressEvent(QKeyEvent *event)
{
    // 首先检查事件是否有效
    if (!event) {
        QWidget::keyPressEvent(event);
        return;
    }
    
    // 处理方向键输入
    bool handled = false;
    
    switch (event->key()) {
    case Qt::Key_Up:
        if (isLocalCoop && player2Alive && player2Snake) {
            // 本地双人模式：方向键控制玩家2
            if (player2Snake->canChangeDirection(Direction::UP)) {
                player2Snake->setDirection(Direction::UP);
                handled = true;
            }
        } else if ((currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) && snake) {
            if (snake->canChangeDirection(Direction::UP)) {
                snake->setDirection(Direction::UP);
                handled = true;
            }
        }
        break;
        
    case Qt::Key_Down:
        if (isLocalCoop && player2Alive && player2Snake) {
            // 本地双人模式：方向键控制玩家2
            if (player2Snake->canChangeDirection(Direction::DOWN)) {
                player2Snake->setDirection(Direction::DOWN);
                handled = true;
            }
        } else if ((currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) && snake) {
            if (snake->canChangeDirection(Direction::DOWN)) {
                snake->setDirection(Direction::DOWN);
                handled = true;
            }
        }
        break;
        
    case Qt::Key_Left:
        if (isLocalCoop && player2Alive && player2Snake) {
            // 本地双人模式：方向键控制玩家2
            if (player2Snake->canChangeDirection(Direction::LEFT)) {
                player2Snake->setDirection(Direction::LEFT);
                handled = true;
            }
        } else if ((currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) && snake) {
            if (snake->canChangeDirection(Direction::LEFT)) {
                snake->setDirection(Direction::LEFT);
                handled = true;
            }
        }
        break;
        
    case Qt::Key_Right:
        if (isLocalCoop && player2Alive && player2Snake) {
            // 本地双人模式：方向键控制玩家2
            if (player2Snake->canChangeDirection(Direction::RIGHT)) {
                player2Snake->setDirection(Direction::RIGHT);
                handled = true;
            }
        } else if ((currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) && snake) {
            if (snake->canChangeDirection(Direction::RIGHT)) {
                snake->setDirection(Direction::RIGHT);
                handled = true;
            }
        }
        break;
        
    case Qt::Key_W:
        if (isLocalCoop && player1Alive && snake) {
            // 本地双人模式：WASD控制玩家1
            if (snake->canChangeDirection(Direction::UP)) {
                snake->setDirection(Direction::UP);
                handled = true;
            }
        } else if ((currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) && snake) {
            if (snake->canChangeDirection(Direction::UP)) {
                snake->setDirection(Direction::UP);
                handled = true;
            }
        }
        break;
        
    case Qt::Key_S:
        if (isLocalCoop && player1Alive && snake) {
            // 本地双人模式：WASD控制玩家1
            if (snake->canChangeDirection(Direction::DOWN)) {
                snake->setDirection(Direction::DOWN);
                handled = true;
            }
        } else if ((currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) && snake) {
            if (snake->canChangeDirection(Direction::DOWN)) {
                snake->setDirection(Direction::DOWN);
                handled = true;
            }
        }
        break;
        
    case Qt::Key_A:
        if (isLocalCoop && player1Alive && snake) {
            // 本地双人模式：WASD控制玩家1
            if (snake->canChangeDirection(Direction::LEFT)) {
                snake->setDirection(Direction::LEFT);
                handled = true;
            }
        } else if ((currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) && snake) {
            if (snake->canChangeDirection(Direction::LEFT)) {
                snake->setDirection(Direction::LEFT);
                handled = true;
            }
        }
        break;
        
    case Qt::Key_D:
        if (isLocalCoop && player1Alive && snake) {
            // 本地双人模式：WASD控制玩家1
            if (snake->canChangeDirection(Direction::RIGHT)) {
                snake->setDirection(Direction::RIGHT);
                handled = true;
            }
        } else if ((currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) && snake) {
            if (snake->canChangeDirection(Direction::RIGHT)) {
                snake->setDirection(Direction::RIGHT);
                handled = true;
            }
        }
        break;
        
    case Qt::Key_Space:
        if (currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) {
            pauseGame();
            handled = true;
        } else if (currentState == GameState::PAUSED) {
            resumeGame();
            handled = true;
        }
        break;
        
    case Qt::Key_R:
        if (currentState == GameState::GAME_OVER) {
            if (isLocalCoop) {
                startLocalCoopGame();
            } else if (isMultiplayer) {
                startMultiPlayerGame(isHost);
            } else {
                startSinglePlayerGame();
            }
            handled = true;
        }
        break;
        
    case Qt::Key_Escape:
        if (currentState == GameState::GAME_OVER) {
            resetGame();
            emit backToMenu();
            handled = true;
        } else if (currentState == GameState::PAUSED) {
            resetGame();
            emit backToMenu();
            handled = true;
        }
        break;
    }
    
    // 如果事件被处理，标记为已接受
    if (handled) {
        event->accept();
    } else {
        // 如果事件未被处理，调用基类实现
        QWidget::keyPressEvent(event);
    }
}

void GameWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateGameArea();
    updateButtonPositions();
}

void GameWidget::updateGameArea()
{
    // 根据窗口大小调整游戏区域，现在游戏区域占据整个窗口
    QSize availableSize = size() - QSize(20, 20); // 减去边距
    
    int maxCellSize = qMin(availableSize.width() / gridWidth, availableSize.height() / gridHeight);
    cellSize = qMax(15, qMin(25, maxCellSize)); // 限制格子大小在15-25之间
    
    gameArea->setFixedSize(gridWidth * cellSize, gridHeight * cellSize);
}

void GameWidget::updateButtonPositions()
{
    if (pauseButton) {
        // 暂停按钮位置 - 右上角
        pauseButton->move(width() - pauseButton->width() - 10, 10);
    }
    
    if (menuButton) {
        // 返回菜单按钮位置 - 右下角
        menuButton->move(width() - menuButton->width() - 10, height() - menuButton->height() - 10);
    }
    
    if (timeLabel) {
        // 时间标签位置 - 游戏界面正上方中央
        int labelWidth = timeLabel->width();
        timeLabel->move((width() - labelWidth) / 2, 10);
    }
}





void GameWidget::setLocalCoopMode(CharacterType player1Character, CharacterType player2Character)
{
    this->player1Character = player1Character;
    this->player2Character = player2Character;
    isLocalCoop = true;
    isMultiplayer = false;
}

void GameWidget::startLocalCoopGame(SinglePlayerMode mode)
{
    qDebug() << "Starting local coop game with mode:" << static_cast<int>(mode);
    
    currentState = GameState::PLAYING;
    isLocalCoop = true;
    isMultiplayer = false;
    localCoopMode = mode;  // 保存游戏模式
    
    // 重置游戏状态
    player1Score = 0;
    player2Score = 0;
    player1Alive = true;
    player2Alive = true;
    player1Lives = MAX_LIVES;
    player2Lives = MAX_LIVES;
    player1Respawning = false;
    player2Respawning = false;
    player1RespawnTime = 0;
    player2RespawnTime = 0;
    totalGameTime = 0;
    level = 1;
    currentSpeed = baseSpeed;
    
    // 设置玩家角色
    snake->setCharacter(player1Character);
    player2Snake->setCharacter(player2Character);
    
    // 初始化蛇的位置
    snake->reset(Point(5, gridHeight / 2));
    snake->setDirection(Direction::RIGHT);
    
    player2Snake->reset(Point(gridWidth - 6, gridHeight / 2));
    player2Snake->setDirection(Direction::LEFT);
    
    // 生成食物
    generateFood();
    
    // 生成墙体（仅在经典模式下）
    if (currentDifficulty == Difficulty::NORMAL || currentDifficulty == Difficulty::HARD) {
        generateWalls();
        qDebug() << "Walls generated for local coop, count:" << wall->getWallPositions().size();
    }
    
    // 启动游戏循环
    gameTimer->start(currentSpeed);
    
    // 根据游戏模式决定是否启动倒计时器
    if (mode == SinglePlayerMode::TIME_ATTACK) {
        // 时间挑战模式：启动倒计时器
        remainingTime = TIME_CHALLENGE_DURATION;
        countdownTimer->start(1000);
        if (timeLabel) {
            timeLabel->setVisible(true);
        }
    } else {
        // 经典模式和挑战模式：不启动倒计时器
        countdownTimer->stop();
        if (timeLabel) {
            timeLabel->setVisible(false);
        }
    }
    
    // 启动游戏时间计时器（每秒更新一次）
    gameTimeTimer->start(1000);
    
    // 启动复活计时器（每秒更新一次）
    respawnTimer->start(1000);
    
    update();
}

void GameWidget::checkLocalCoopCollisions()
{
    // 检查玩家1的碰撞
    if (player1Alive) {
        Point head1 = snake->getHead();
        
        // 检查边界碰撞
        if (head1.x < 0 || head1.x >= gridWidth || head1.y < 0 || head1.y >= gridHeight) {
            player1Alive = false;
            player1Lives--;
            if (player1Lives > 0) {
                player1Respawning = true;
                player1RespawnTime = RESPAWN_TIME;
            } else {
                // 玩家1生命耗尽，游戏结束
                endLocalCoopGame(); // 游戏结束
                return;
            }
        }
        
        // 检查自身碰撞
        auto body1 = snake->getBody();
        for (size_t i = 1; i < body1.size(); ++i) {
            if (head1 == body1[i]) {
                player1Alive = false;
                player1Lives--;
                if (player1Lives > 0) {
                    player1Respawning = true;
                    player1RespawnTime = RESPAWN_TIME;
                } else {
                    // 玩家1生命耗尽，游戏结束
                    endLocalCoopGame(); // 游戏结束
                    return;
                }
                break;
            }
        }
        
        // 检查与玩家2蛇身的碰撞
        if (player2Alive && player2Snake) {
            auto body2 = player2Snake->getBody();
            for (const auto& segment : body2) {
                if (head1 == segment) {
                    player1Alive = false;
                    player1Lives--;
                    if (player1Lives > 0) {
                        player1Respawning = true;
                        player1RespawnTime = RESPAWN_TIME;
                    } else {
                        // 玩家1生命耗尽，游戏结束
                        endLocalCoopGame(); // 游戏结束
                        return;
                    }
                    break;
                }
            }
        }
        
        // 检查墙体碰撞
        if (wall && wall->hasWallAt(head1)) {
            player1Alive = false;
            player1Lives--;
            if (player1Lives > 0) {
                player1Respawning = true;
                player1RespawnTime = RESPAWN_TIME;
            } else {
                // 玩家1生命耗尽，游戏结束
                currentState = GameState::GAME_OVER;
                gameTimer->stop();
                respawnTimer->stop();
                gameTimeTimer->stop();
                emit gameOver(player2Score); // 玩家2获胜
                return;
            }
        }
        
        // 检查食物碰撞
        if (head1 == food->getPosition()) {
            snake->grow();
            if (food->isSpecial()) {
                player1Score += 20;
                // 特殊食物回复生命
                if (player1Lives < MAX_LIVES) {
                    player1Lives++;
                }
            } else {
                player1Score += 10;
            }
            generateFood();
        }
    }
    
    // 检查玩家2的碰撞
    if (player2Alive && player2Snake) {
        Point head2 = player2Snake->getHead();
        
        // 检查边界碰撞
        if (head2.x < 0 || head2.x >= gridWidth || head2.y < 0 || head2.y >= gridHeight) {
            player2Alive = false;
            player2Lives--;
            if (player2Lives > 0) {
                player2Respawning = true;
                player2RespawnTime = RESPAWN_TIME;
            } else {
                // 玩家2生命耗尽，游戏结束
                endLocalCoopGame(); // 游戏结束
                return;
            }
        }
        
        // 检查自身碰撞
        auto body2 = player2Snake->getBody();
        for (size_t i = 1; i < body2.size(); ++i) {
            if (head2 == body2[i]) {
                player2Alive = false;
                player2Lives--;
                if (player2Lives > 0) {
                    player2Respawning = true;
                    player2RespawnTime = RESPAWN_TIME;
                } else {
                    // 玩家2生命耗尽，游戏结束
                    endLocalCoopGame(); // 游戏结束
                    return;
                }
                break;
            }
        }
        
        // 检查与玩家1蛇身的碰撞
        if (player1Alive) {
            auto body1 = snake->getBody();
            for (const auto& segment : body1) {
                if (head2 == segment) {
                    player2Alive = false;
                    player2Lives--;
                    if (player2Lives > 0) {
                        player2Respawning = true;
                        player2RespawnTime = RESPAWN_TIME;
                    } else {
                        // 玩家2生命耗尽，游戏结束
                        endLocalCoopGame(); // 游戏结束
                        return;
                    }
                    break;
                }
            }
        }
        
        // 检查墙体碰撞
        if (wall && wall->hasWallAt(head2)) {
            player2Alive = false;
            player2Lives--;
            if (player2Lives > 0) {
                player2Respawning = true;
                player2RespawnTime = RESPAWN_TIME;
            } else {
                // 玩家2生命耗尽，游戏结束
                currentState = GameState::GAME_OVER;
                gameTimer->stop();
                respawnTimer->stop();
                gameTimeTimer->stop();
                emit gameOver(player1Score); // 玩家1获胜
                return;
            }
        }
        
        // 检查食物碰撞
        if (head2 == food->getPosition()) {
            player2Snake->grow();
            if (food->isSpecial()) {
                player2Score += 20;
                // 特殊食物回复生命
                if (player2Lives < MAX_LIVES) {
                    player2Lives++;
                }
            } else {
                player2Score += 10;
            }
            generateFood();
        }
    }
    
    // 时间挑战模式下不会因为玩家死亡而结束游戏
    // 游戏只会在5分钟倒计时结束后结束
 }

void GameWidget::drawLocalCoopSnakes(QPainter& painter, const QRect& gameRect)
{
    // 绘制玩家1的蛇
    if (player1Alive && snake) {
        const auto& body1 = snake->getBody();
        if (!body1.empty()) {
            // 绘制蛇头
            Point head1 = body1.front();
            QRect headRect1(gameRect.x() + head1.x * cellSize, 
                           gameRect.y() + head1.y * cellSize, 
                           cellSize, cellSize);
            
            QPixmap headPixmap1 = snake->getHeadPixmap();
            if (!headPixmap1.isNull()) {
                painter.drawPixmap(headRect1, headPixmap1);
            } else {
                painter.fillRect(headRect1, QColor(255, 215, 0)); // 金色备用
            }
            
            // 绘制蛇身
            QPixmap bodyPixmap1 = snake->getBodyPixmap();
            for (auto it = body1.begin() + 1; it != body1.end(); ++it) {
                int maxBodySize = (snake->getCharacter() == CharacterType::SPONGEBOB) ? 100 : 50;
                int bodySize = qMin(maxBodySize, cellSize);
                int bodyOffset = (cellSize - bodySize) / 2;
                QRect bodyRect1(gameRect.x() + it->x * cellSize + bodyOffset, 
                               gameRect.y() + it->y * cellSize + bodyOffset, 
                               bodySize, bodySize);
                
                if (!bodyPixmap1.isNull()) {
                    painter.drawPixmap(bodyRect1, bodyPixmap1);
                } else {
                    painter.fillRect(bodyRect1, QColor(255, 215, 0)); // 金色备用
                }
            }
        }
    }
    
    // 绘制玩家2的蛇
    if (player2Alive && player2Snake) {
        const auto& body2 = player2Snake->getBody();
        if (!body2.empty()) {
            // 绘制蛇头
            Point head2 = body2.front();
            QRect headRect2(gameRect.x() + head2.x * cellSize, 
                           gameRect.y() + head2.y * cellSize, 
                           cellSize, cellSize);
            
            QPixmap headPixmap2 = player2Snake->getHeadPixmap();
            if (!headPixmap2.isNull()) {
                painter.drawPixmap(headRect2, headPixmap2);
            } else {
                painter.fillRect(headRect2, QColor(255, 20, 147)); // 深粉色备用
            }
            
            // 绘制蛇身
            QPixmap bodyPixmap2 = player2Snake->getBodyPixmap();
            for (auto it = body2.begin() + 1; it != body2.end(); ++it) {
                int maxBodySize = (player2Snake->getCharacter() == CharacterType::SPONGEBOB) ? 100 : 50;
                int bodySize = qMin(maxBodySize, cellSize);
                int bodyOffset = (cellSize - bodySize) / 2;
                QRect bodyRect2(gameRect.x() + it->x * cellSize + bodyOffset, 
                               gameRect.y() + it->y * cellSize + bodyOffset, 
                               bodySize, bodySize);
                
                if (!bodyPixmap2.isNull()) {
                    painter.drawPixmap(bodyRect2, bodyPixmap2);
                } else {
                    painter.fillRect(bodyRect2, QColor(255, 20, 147)); // 深粉色备用
                }
            }
        }
    }
}

void GameWidget::updateCountdown()
{
    if (remainingTime > 0) {
        remainingTime--;
        
        // 更新时间显示
        int minutes = remainingTime / 60;
        int seconds = remainingTime % 60;
        QString timeText = QString("Time: %1:%2")
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(seconds, 2, 10, QChar('0'));
        timeLabel->setText(timeText);
        
        // 时间不足30秒时改变颜色警告
        if (remainingTime <= 30) {
            timeLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF; font-family: Arial, sans-serif; background-color: rgba(255, 0, 0, 220); padding: 8px 15px; border-radius: 8px; border: 2px solid #FF0000;");
        } else if (remainingTime <= 60) {
            timeLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFF00; font-family: Arial, sans-serif; background-color: rgba(255, 165, 0, 220); padding: 8px 15px; border-radius: 8px; border: 2px solid #FFA500;");
        }
        
        // 更新标签位置
        updateButtonPositions();
    } else {
        // 时间到，结束游戏
        countdownTimer->stop();
        gameTimer->stop();
        currentState = GameState::GAME_OVER;
        
        QMessageBox::information(this, "Time's Up!", QString("Time Challenge Ended!\nFinal Score: %1").arg(score));
        
        // 重置游戏
        resetGame();
        emit backToMenu();
    }
}

void GameWidget::updateRespawnTimer()
{
    // 更新玩家1复活倒计时
    if (player1Respawning && player1RespawnTime > 0) {
        player1RespawnTime--;
        if (player1RespawnTime <= 0) {
            respawnPlayer(1);
        }
    }
    
    // 更新玩家2复活倒计时
    if (player2Respawning && player2RespawnTime > 0) {
        player2RespawnTime--;
        if (player2RespawnTime <= 0) {
            respawnPlayer(2);
        }
    }
    
    update();
}

void GameWidget::updateGameTimer()
{
    totalGameTime++;
    
    // 检查是否达到游戏总时长（5分钟）
    if (totalGameTime >= TOTAL_GAME_TIME) {
        endTimeAttackGame();
    }
    
    update();
}

void GameWidget::respawnPlayer(int playerNum)
{
    if (playerNum == 1) {
        player1Alive = true;
        player1Respawning = false;
        player1RespawnTime = 0;
        
        // 重置玩家1蛇的位置
        snake->reset(Point(5, gridHeight / 2));
        snake->setDirection(Direction::RIGHT);
        snake->setCharacter(player1Character);
    } else if (playerNum == 2) {
        player2Alive = true;
        player2Respawning = false;
        player2RespawnTime = 0;
        
        // 重置玩家2蛇的位置
        player2Snake->reset(Point(gridWidth - 6, gridHeight / 2));
        player2Snake->setDirection(Direction::LEFT);
        player2Snake->setCharacter(player2Character);
    }
}

void GameWidget::endTimeAttackGame()
{
    // 停止所有计时器
    gameTimer->stop();
    gameTimeTimer->stop();
    respawnTimer->stop();
    
    currentState = GameState::GAME_OVER;
    
    // 比较得分并显示结果
    QString result;
    if (player1Score > player2Score) {
        result = QString("玩家1胜利！\n玩家1得分: %1\n玩家2得分: %2").arg(player1Score).arg(player2Score);
    } else if (player2Score > player1Score) {
        result = QString("玩家2胜利！\n玩家1得分: %1\n玩家2得分: %2").arg(player1Score).arg(player2Score);
    } else {
        result = QString("平局！\n玩家1得分: %1\n玩家2得分: %2").arg(player1Score).arg(player2Score);
    }
    
    QMessageBox::information(this, "时间挑战结束", result);
}

void GameWidget::endLocalCoopGame()
{
    // 停止所有计时器
    gameTimer->stop();
    gameTimeTimer->stop();
    respawnTimer->stop();
    countdownTimer->stop();
    
    currentState = GameState::GAME_OVER;
    
    // 根据游戏模式显示结果
    QString modeText;
    switch (localCoopMode) {
    case SinglePlayerMode::CLASSIC:
        modeText = "经典模式";
        break;
    case SinglePlayerMode::CHALLENGE:
        modeText = "挑战模式";
        break;
    case SinglePlayerMode::TIME_ATTACK:
        modeText = "时间挑战";
        break;
    default:
        modeText = "未知模式";
        break;
    }
    
    // 自动判断获胜者
    QString result;
    if (player1Score > player2Score) {
        result = QString("%1 - 玩家1胜利！\n\n最终得分:\n玩家1: %2 分\n玩家2: %3 分\n\n游戏时长: %4 秒")
                .arg(modeText)
                .arg(player1Score)
                .arg(player2Score)
                .arg(totalGameTime);
    } else if (player2Score > player1Score) {
        result = QString("%1 - 玩家2胜利！\n\n最终得分:\n玩家1: %2 分\n玩家2: %3 分\n\n游戏时长: %4 秒")
                .arg(modeText)
                .arg(player1Score)
                .arg(player2Score)
                .arg(totalGameTime);
    } else {
        result = QString("%1 - 平局！\n\n最终得分:\n玩家1: %2 分\n玩家2: %3 分\n\n游戏时长: %4 秒")
                .arg(modeText)
                .arg(player1Score)
                .arg(player2Score)
                .arg(totalGameTime);
    }
    
    QMessageBox::information(this, "游戏结束", result);
}

void GameWidget::drawPlayerStatusPanel(QPainter& painter)
{
    // 在游戏界面右侧绘制玩家状态面板，调整大小避免遮挡
    int panelX = width() - 160;
    int panelY = 80;
    int panelWidth = 150;
    int panelHeight = 180;
    
    // 绘制面板背景
    painter.fillRect(panelX, panelY, panelWidth, panelHeight, QColor(0, 0, 0, 180));
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(panelX, panelY, panelWidth, panelHeight);
    
    // 绘制标题
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setPen(Qt::white);
    painter.drawText(panelX + 8, panelY + 18, "玩家状态");
    
    painter.setFont(QFont("Arial", 10));
    
    if (localCoopMode == SinglePlayerMode::TIME_ATTACK) {
        // 时间模式：显示玩家状态和死亡倒计时
        // 绘制玩家1状态
        painter.setPen(Qt::white);
        painter.drawText(panelX + 8, panelY + 38, "P1(WASD):");
        if (player1Alive) {
            painter.setPen(Qt::green);
            painter.drawText(panelX + 8, panelY + 52, "存活");
        } else if (player1Respawning) {
            painter.setPen(Qt::yellow);
            painter.drawText(panelX + 8, panelY + 52, QString("复活%1s").arg(player1RespawnTime));
        } else {
            painter.setPen(Qt::red);
            painter.drawText(panelX + 8, panelY + 52, "死亡");
        }
        
        // 绘制玩家2状态
        painter.setPen(Qt::white);
        painter.drawText(panelX + 8, panelY + 80, "P2(方向键):");
        if (player2Alive) {
            painter.setPen(Qt::green);
            painter.drawText(panelX + 8, panelY + 94, "存活");
        } else if (player2Respawning) {
            painter.setPen(Qt::yellow);
            painter.drawText(panelX + 8, panelY + 94, QString("复活%1s").arg(player2RespawnTime));
        } else {
            painter.setPen(Qt::red);
            painter.drawText(panelX + 8, panelY + 94, "死亡");
        }
        
        // 绘制游戏剩余时间
        int remainingTime = TOTAL_GAME_TIME - totalGameTime;
        int minutes = remainingTime / 60;
        int seconds = remainingTime % 60;
        painter.setPen(Qt::cyan);
        painter.setFont(QFont("Arial", 11, QFont::Bold));
        painter.drawText(panelX + 8, panelY + 120, "剩余时间:");
        painter.drawText(panelX + 8, panelY + 137, QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
        
        // 绘制分隔线
        painter.setPen(QPen(Qt::white, 1));
        painter.drawLine(panelX + 8, panelY + 70, panelX + panelWidth - 8, panelY + 70);
        painter.drawLine(panelX + 8, panelY + 110, panelX + panelWidth - 8, panelY + 110);
    } else {
        // 经典模式和挑战模式：显示剩余生命和复活倒计时
        // 绘制玩家1状态
        painter.setPen(Qt::white);
        painter.drawText(panelX + 8, panelY + 38, "P1(WASD):");
        painter.drawText(panelX + 8, panelY + 52, QString("生命:%1").arg(player1Lives));
        if (player1Respawning) {
            painter.setPen(Qt::yellow);
            painter.drawText(panelX + 8, panelY + 66, QString("复活%1s").arg(player1RespawnTime));
        } else if (!player1Alive) {
            painter.setPen(Qt::red);
            painter.drawText(panelX + 8, panelY + 66, "死亡");
        }
        
        // 绘制玩家2状态
        painter.setPen(Qt::white);
        painter.drawText(panelX + 8, panelY + 90, "P2(方向键):");
        painter.drawText(panelX + 8, panelY + 104, QString("生命:%1").arg(player2Lives));
        if (player2Respawning) {
            painter.setPen(Qt::yellow);
            painter.drawText(panelX + 8, panelY + 118, QString("复活%1s").arg(player2RespawnTime));
        } else if (!player2Alive) {
            painter.setPen(Qt::red);
            painter.drawText(panelX + 8, panelY + 118, "死亡");
        }
        
        // 绘制分隔线
        painter.setPen(QPen(Qt::white, 1));
        painter.drawLine(panelX + 8, panelY + 80, panelX + panelWidth - 8, panelY + 80);
    }
}

void GameWidget::setHotspotGameManager(HotspotGameManager* manager)
{
    hotspotGameManager = manager;
}