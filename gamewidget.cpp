#include "gamewidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QFont>
#include <QDebug>
#include <QApplication>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , currentState(GameState::MENU)
    , currentDifficulty(Difficulty::NORMAL)
    , isMultiplayer(false)
    , isHost(false)
    , snake(new Snake(this))
    , food(new Food(this))
    , gameTimer(new QTimer(this))
    , specialFoodTimer(new QTimer(this))
    , gridWidth(30)
    , gridHeight(20)
    , cellSize(20)
    , score(0)
    , level(1)
    , baseSpeed(200)
    , currentSpeed(200)
    , networkManager(new NetworkManager(this))
    , multiPlayerManager(new MultiPlayerGameManager(this))
    , singlePlayerManager(new SinglePlayerGameManager(this))
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
    
    // 网络信号
    connect(networkManager, &NetworkManager::playerConnected, this, &GameWidget::onPlayerConnected);
    connect(networkManager, &NetworkManager::playerDisconnected, this, &GameWidget::onPlayerDisconnected);
    connect(networkManager, &NetworkManager::playerInfoReceived, this, &GameWidget::onPlayerInfoReceived);
    connect(networkManager, &NetworkManager::scoreUpdateReceived, this, &GameWidget::onScoreUpdateReceived);
    connect(networkManager, &NetworkManager::playerPositionReceived, this, &GameWidget::onPlayerPositionReceived);
    connect(networkManager, &NetworkManager::connectionError, this, &GameWidget::onNetworkError);
    
    // 多人游戏管理器信号
    connect(multiPlayerManager, &MultiPlayerGameManager::roomCreated, this, &GameWidget::onRoomCreated);
    connect(multiPlayerManager, &MultiPlayerGameManager::playerJoinedRoom, this, &GameWidget::onPlayerJoinedRoom);
    connect(multiPlayerManager, &MultiPlayerGameManager::playerLeftRoom, this, &GameWidget::onPlayerLeftRoom);
    connect(multiPlayerManager, &MultiPlayerGameManager::gameStarted, this, &GameWidget::onGameStarted);
    connect(multiPlayerManager, &MultiPlayerGameManager::gameEnded, this, &GameWidget::onGameEnded);
    connect(multiPlayerManager, &MultiPlayerGameManager::gameStateUpdated, this, &GameWidget::onGameStateUpdated);
    connect(multiPlayerManager, &MultiPlayerGameManager::playerCollision, this, &GameWidget::onPlayerCollision);
    connect(multiPlayerManager, &MultiPlayerGameManager::foodEaten, this, &GameWidget::onFoodEaten);
    
    // 设置网络管理器
    multiPlayerManager->setNetworkManager(networkManager);
    
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(800, 600);
}

void GameWidget::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    gameLayout = new QHBoxLayout();
    
    // 游戏区域
    gameArea = new QWidget(this);
    gameArea->setMinimumSize(gridWidth * cellSize, gridHeight * cellSize);
    gameArea->setStyleSheet("background-color: #87CEEB; border: 2px solid #4682B4;");
    
    // 侧边面板
    sidePanel = new QWidget(this);
    sidePanel->setFixedWidth(200);
    sidePanel->setStyleSheet("background-color: #F0F8FF; border: 2px solid #4682B4; border-radius: 10px;");
    
    QVBoxLayout* sidePanelLayout = new QVBoxLayout(sidePanel);
    sidePanelLayout->setSpacing(15);
    sidePanelLayout->setContentsMargins(15, 15, 15, 15);
    
    // 分数标签
    scoreLabel = new QLabel("分数: 0", sidePanel);
    scoreLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #FF6347;");
    sidePanelLayout->addWidget(scoreLabel);
    
    // 等级标签
    levelLabel = new QLabel("等级: 1", sidePanel);
    levelLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #32CD32;");
    sidePanelLayout->addWidget(levelLabel);
    
    // 玩家标签（多人游戏时显示）
    playersLabel = new QLabel("在线玩家:", sidePanel);
    playersLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #4169E1;");
    playersLabel->setVisible(false);
    sidePanelLayout->addWidget(playersLabel);
    
    // 玩家列表
    playersList = new QListWidget(sidePanel);
    playersList->setMaximumHeight(150);
    playersList->setStyleSheet("background-color: white; border: 1px solid #ccc; border-radius: 5px;");
    playersList->setVisible(false);
    sidePanelLayout->addWidget(playersList);
    
    sidePanelLayout->addStretch();
    
    // 暂停按钮
    pauseButton = new QPushButton("暂停", sidePanel);
    pauseButton->setStyleSheet("QPushButton { background-color: #FFA500; color: white; border: none; border-radius: 5px; padding: 8px; font-size: 12px; }"
                               "QPushButton:hover { background-color: #FF8C00; }"
                               "QPushButton:pressed { background-color: #FF7F00; }");
    connect(pauseButton, &QPushButton::clicked, [this]() {
        if (currentState == GameState::PLAYING) {
            pauseGame();
        } else if (currentState == GameState::PAUSED) {
            resumeGame();
        }
    });
    sidePanelLayout->addWidget(pauseButton);
    
    // 返回菜单按钮
    menuButton = new QPushButton("返回菜单", sidePanel);
    menuButton->setStyleSheet("QPushButton { background-color: #DC143C; color: white; border: none; border-radius: 5px; padding: 8px; font-size: 12px; }"
                              "QPushButton:hover { background-color: #B22222; }"
                              "QPushButton:pressed { background-color: #A0522D; }");
    connect(menuButton, &QPushButton::clicked, [this]() {
        resetGame();
        emit backToMenu();
    });
    sidePanelLayout->addWidget(menuButton);
    
    gameLayout->addWidget(gameArea, 1);
    gameLayout->addWidget(sidePanel);
    
    mainLayout->addLayout(gameLayout);
    
    // 确保游戏区域尺寸正确
    updateGameArea();
}

void GameWidget::setupGame()
{
    // 设置游戏计时器
    specialFoodTimer->setSingleShot(true);
    connect(specialFoodTimer, &QTimer::timeout, [this]() {
        generateSpecialFood();
    });
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
    playersLabel->setVisible(false);
    playersList->setVisible(false);
    
    resetGame();
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
    
    // 启动游戏循环
    gameTimer->start(currentSpeed);
    qDebug() << "Game timer started with speed:" << currentSpeed;
    
    update();
    qDebug() << "Update called";
}

void GameWidget::startMultiPlayerGame(bool isHost)
{
    this->isHost = isHost;
    isMultiplayer = true;
    playersLabel->setVisible(true);
    playersList->setVisible(true);
    
    // 使用新的多人游戏管理器
    if (isHost) {
        // 创建房间
        QString hostName = "Player1"; // 可以从设置中获取
        createRoom(hostName, 4); // 默认4人房间
    } else {
        // 这里应该有UI让用户输入房间ID和玩家名
        // 暂时使用默认值进行测试
        QString guestName = "Player2";
        QString roomId = "123456"; // 应该从UI获取
        joinRoom(roomId, guestName);
    }
    
    resetGame();
    currentState = GameState::MULTIPLAYER_GAME;
    
    // 初始化蛇的位置
    Point startPos(gridWidth / 2, gridHeight / 2);
    snake->reset(startPos);
    
    // 生成第一个食物
    generateFood();
    
    update();
}

void GameWidget::pauseGame()
{
    if (currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) {
        gameTimer->stop();
        specialFoodTimer->stop();
        currentState = GameState::PAUSED;
        pauseButton->setText("继续");
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
        pauseButton->setText("暂停");
        update();
    }
}

void GameWidget::resetGame()
{
    gameTimer->stop();
    specialFoodTimer->stop();
    
    score = 0;
    level = 1;
    currentSpeed = baseSpeed;
    specialFoodCounter = 0;
    
    scoreLabel->setText("分数: 0");
    levelLabel->setText("等级: 1");
    pauseButton->setText("暂停");
    
    // 清理多人游戏数据
    otherPlayers.clear();
    playerCharacters.clear();
    playerScores.clear();
    playerAliveStatus.clear();
    playersList->clear();
    
    if (isMultiplayer) {
        networkManager->stopServer();
        networkManager->disconnectFromServer();
    }
    
    currentState = GameState::MENU;
    update();
}

void GameWidget::gameLoop()
{
    if (currentState != GameState::PLAYING && currentState != GameState::MULTIPLAYER_GAME) {
        return;
    }
    
    if (currentState == GameState::MULTIPLAYER_GAME && !currentRoomId.isEmpty()) {
        // 多人游戏模式：通过游戏管理器处理
        Point head = snake->getHead();
        Direction currentDirection = snake->getDirection();
        
        // 更新玩家方向
        multiPlayerManager->updatePlayerDirection(currentRoomId, playerName, currentDirection);
        
        // 移动蛇
        snake->move();
        
        // 检查碰撞
        Point newHead = snake->getHead();
        if (multiPlayerManager->checkPlayerCollision(currentRoomId, playerName, newHead)) {
            // 玩家碰撞，由管理器处理
            return;
        }
        
        // 检查食物碰撞
        if (multiPlayerManager->checkFoodCollision(currentRoomId, newHead)) {
            snake->grow();
            // 食物碰撞由管理器处理分数更新
        }
        
        // 更新玩家位置
        multiPlayerManager->updatePlayerPosition(currentRoomId, playerName, snake->getBody());
        
        // 发送网络更新
        if (isMultiplayer) {
            sendNetworkUpdate();
        }
    } else {
        // 单人游戏模式：原有逻辑
        snake->move();
        checkCollisions();
    }
    
    update();
}

void GameWidget::checkCollisions()
{
    Point head = snake->getHead();
    
    // 检查边界碰撞
    if (snake->checkBoundaryCollision(gridWidth, gridHeight)) {
        currentState = GameState::GAME_OVER;
        gameTimer->stop();
        specialFoodTimer->stop();
        saveHighScore();
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
        emit gameOver(score);
        update();
        return;
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
        
        if (isMultiplayer && !currentRoomId.isEmpty()) {
            // 多人游戏中通过管理器更新分数
            multiPlayerManager->updatePlayerScore(currentRoomId, playerName, score);
        }
    }
}

void GameWidget::generateFood()
{
    QSet<Point> occupiedPositions = getOccupiedPositions();
    food->generateFood(gridWidth, gridHeight, occupiedPositions);
}

void GameWidget::generateSpecialFood()
{
    QSet<Point> occupiedPositions = getOccupiedPositions();
    food->generateSpecialFood(gridWidth, gridHeight, occupiedPositions);
}

QSet<Point> GameWidget::getOccupiedPositions() const
{
    QSet<Point> positions;
    
    // 添加蛇身位置
    for (const auto& point : snake->getBody()) {
        positions.insert(point);
    }
    
    // 添加其他玩家的蛇身位置（多人游戏）
    for (const auto& playerSnake : otherPlayers) {
        for (const auto& point : playerSnake) {
            positions.insert(point);
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
        
        // 每升一级，速度增加10%
        currentSpeed = qMax(50, static_cast<int>(baseSpeed * qPow(0.9, level - 1)));
        if (gameTimer->isActive()) {
            gameTimer->setInterval(currentSpeed);
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
    if (networkManager && isMultiplayer) {
        networkManager->sendPlayerPosition(snake->getBody());
    }
}

// 网络事件处理
void GameWidget::onPlayerConnected(const QString& playerName)
{
    playersList->addItem(QString("%1 (连接)").arg(playerName));
}

void GameWidget::onPlayerDisconnected(const QString& playerName)
{
    // 从列表中移除玩家
    for (int i = 0; i < playersList->count(); ++i) {
        if (playersList->item(i)->text().contains(playerName)) {
            delete playersList->takeItem(i);
            break;
        }
    }
    
    // 清理玩家数据
    otherPlayers.remove(playerName);
    playerCharacters.remove(playerName);
    playerScores.remove(playerName);
    playerAliveStatus.remove(playerName);
}

void GameWidget::onPlayerInfoReceived(const PlayerInfo& playerInfo)
{
    QString playerName = QString::fromStdString(playerInfo.name);
    playerCharacters[playerName] = playerInfo.character;
    playerScores[playerName] = playerInfo.score;
    playerAliveStatus[playerName] = playerInfo.isAlive;
}

void GameWidget::onScoreUpdateReceived(const QString& playerName, int score)
{
    playerScores[playerName] = score;
    
    // 更新玩家列表显示
    for (int i = 0; i < playersList->count(); ++i) {
        QListWidgetItem* item = playersList->item(i);
        if (item->text().contains(playerName)) {
            item->setText(QString("%1: %2分").arg(playerName).arg(score));
            break;
        }
    }
}

void GameWidget::onPlayerPositionReceived(const QString& playerName, const std::deque<Point>& snakeBody)
{
    otherPlayers[playerName] = snakeBody;
}

void GameWidget::onNetworkError(const QString& error)
{
    QMessageBox::warning(this, "网络错误", error);
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
    
    // 计算游戏区域的实际位置
    QRect gameRect = gameArea->geometry();
    qDebug() << "GameArea geometry:" << gameRect << "Widget size:" << size() << "CellSize:" << cellSize;
    painter.setClipRect(gameRect);
    
    // 绘制背景
    painter.fillRect(gameRect, QColor(135, 206, 235)); // 天蓝色背景
    
    qDebug() << "Current state:" << static_cast<int>(currentState) << "Snake body size:" << snake->getBody().size();
    
    if (currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME || currentState == GameState::PAUSED) {
        // 绘制网格
        drawGrid(painter);
        
        // 绘制食物
        drawFood(painter);
        
        // 绘制蛇
        drawSnake(painter);
        
        // 绘制其他玩家的蛇（多人游戏）
        if (isMultiplayer) {
            drawMultiplayerSnakes(painter);
        }
    }
    
    // 绘制UI覆盖层
    if (currentState == GameState::PAUSED) {
        drawPauseOverlay(painter);
    } else if (currentState == GameState::GAME_OVER) {
        drawGameOverOverlay(painter);
    }
}

void GameWidget::drawGrid(QPainter& painter)
{
    QRect gameRect = gameArea->geometry();
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

void GameWidget::drawSnake(QPainter& painter)
{
    QRect gameRect = gameArea->geometry();
    const auto& body = snake->getBody();
    
    if (body.empty()) return;
    
    // 绘制蛇头
    Point head = body.front();
    QRect headRect(gameRect.x() + head.x * cellSize + 1, 
                   gameRect.y() + head.y * cellSize + 1, 
                   cellSize - 2, cellSize - 2);
    
    QPixmap headPixmap = snake->getHeadPixmap();
    if (!headPixmap.isNull()) {
        painter.drawPixmap(headRect, headPixmap);
    } else {
        painter.fillRect(headRect, Qt::darkGreen);
    }
    
    // 绘制蛇身
    QPixmap bodyPixmap = snake->getBodyPixmap();
    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        QRect bodyRect(gameRect.x() + it->x * cellSize + 1, 
                       gameRect.y() + it->y * cellSize + 1, 
                       cellSize - 2, cellSize - 2);
        
        if (!bodyPixmap.isNull()) {
            painter.drawPixmap(bodyRect, bodyPixmap);
        } else {
            painter.fillRect(bodyRect, Qt::green);
        }
    }
}

void GameWidget::drawFood(QPainter& painter)
{
    QRect gameRect = gameArea->geometry();
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

void GameWidget::drawMultiplayerSnakes(QPainter& painter)
{
    QRect gameRect = gameArea->geometry();
    
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
            QRect rect(gameRect.x() + point.x * cellSize + 1, 
                       gameRect.y() + point.y * cellSize + 1, 
                       cellSize - 2, cellSize - 2);
            
            if (i == 0) {
                // 蛇头，颜色稍深
                painter.fillRect(rect, playerColor.darker(120));
            } else {
                // 蛇身
                painter.fillRect(rect, playerColor);
            }
        }
        
        // 绘制玩家名称
        if (!body.empty()) {
            Point head = body.front();
            QRect nameRect(gameRect.x() + head.x * cellSize, 
                           gameRect.y() + head.y * cellSize - 15, 
                           cellSize * 3, 15);
            painter.setPen(Qt::black);
            painter.setFont(QFont("Arial", 8));
            painter.drawText(nameRect, Qt::AlignCenter, playerName);
        }
    }
}

void GameWidget::drawPauseOverlay(QPainter& painter)
{
    QRect gameRect = gameArea->geometry();
    
    // 半透明背景
    painter.fillRect(gameRect, QColor(0, 0, 0, 128));
    
    // 暂停文字
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(24);
    font.setBold(true);
    painter.setFont(font);
    
    painter.drawText(gameRect, Qt::AlignCenter, "游戏暂停\n按空格键继续");
}

void GameWidget::drawGameOverOverlay(QPainter& painter)
{
    QRect gameRect = gameArea->geometry();
    
    // 半透明背景
    painter.fillRect(gameRect, QColor(0, 0, 0, 150));
    
    // 游戏结束文字
    painter.setPen(Qt::red);
    QFont font = painter.font();
    font.setPointSize(28);
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
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        if (currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) {
            snake->setDirection(Direction::UP);
        }
        break;
        
    case Qt::Key_Down:
    case Qt::Key_S:
        if (currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) {
            snake->setDirection(Direction::DOWN);
        }
        break;
        
    case Qt::Key_Left:
    case Qt::Key_A:
        if (currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) {
            snake->setDirection(Direction::LEFT);
        }
        break;
        
    case Qt::Key_Right:
    case Qt::Key_D:
        if (currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) {
            snake->setDirection(Direction::RIGHT);
        }
        break;
        
    case Qt::Key_Space:
        if (currentState == GameState::PLAYING || currentState == GameState::MULTIPLAYER_GAME) {
            pauseGame();
        } else if (currentState == GameState::PAUSED) {
            resumeGame();
        }
        break;
        
    case Qt::Key_R:
        if (currentState == GameState::GAME_OVER) {
            if (isMultiplayer) {
                startMultiPlayerGame(isHost);
            } else {
                startSinglePlayerGame();
            }
        }
        break;
        
    case Qt::Key_Escape:
        if (currentState == GameState::GAME_OVER) {
            resetGame();
            emit backToMenu();
        } else if (currentState == GameState::PAUSED) {
            resetGame();
            emit backToMenu();
        }
        break;
    }
    
    QWidget::keyPressEvent(event);
}

void GameWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateGameArea();
}

void GameWidget::updateGameArea()
{
    // 根据窗口大小调整游戏区域
    QSize availableSize = size() - QSize(220, 40); // 减去侧边栏和边距
    
    int maxCellSize = qMin(availableSize.width() / gridWidth, availableSize.height() / gridHeight);
    cellSize = qMax(15, qMin(25, maxCellSize)); // 限制格子大小在15-25之间
    
    gameArea->setFixedSize(gridWidth * cellSize, gridHeight * cellSize);
}

// 多人游戏管理器相关函数实现
void GameWidget::createRoom(const QString& playerName, int maxPlayers)
{
    this->playerName = playerName;
    currentRoomId = multiPlayerManager->createRoom(playerName, maxPlayers);
    isHost = true;
    
    if (!networkManager->startServer()) {
        QMessageBox::warning(this, "错误", "无法启动服务器！");
        return;
    }
    
    qDebug() << "Room created:" << currentRoomId << "by" << playerName;
}

void GameWidget::joinRoom(const QString& roomId, const QString& playerName)
{
    this->playerName = playerName;
    this->currentRoomId = roomId;
    isHost = false;
    
    if (multiPlayerManager->joinRoom(roomId, playerName)) {
        qDebug() << "Successfully joined room:" << roomId;
    } else {
        QMessageBox::warning(this, "错误", "无法加入房间！");
    }
}

void GameWidget::leaveRoom()
{
    if (!currentRoomId.isEmpty() && !playerName.isEmpty()) {
        multiPlayerManager->leaveRoom(currentRoomId, playerName);
        currentRoomId.clear();
        playerName.clear();
        
        if (isHost) {
            networkManager->stopServer();
        } else {
            networkManager->disconnectFromServer();
        }
    }
}

void GameWidget::onRoomCreated(const QString& roomId, const GameRoom& room)
{
    qDebug() << "Room created signal received:" << roomId;
    // 更新UI显示房间信息
    playersLabel->setText(QString("房间: %1 (%2/%3)").arg(roomId).arg(room.playerNames.size()).arg(room.maxPlayers));
    
    playersList->clear();
    for (const QString& player : room.playerNames) {
        playersList->addItem(player + (player == room.hostName ? " (房主)" : ""));
    }
}

void GameWidget::onPlayerJoinedRoom(const QString& roomId, const QString& playerName)
{
    if (roomId != currentRoomId) return;
    
    qDebug() << "Player joined:" << playerName;
    
    // 更新玩家列表
    GameRoom room = multiPlayerManager->getRoomInfo(roomId);
    playersLabel->setText(QString("房间: %1 (%2/%3)").arg(roomId).arg(room.playerNames.size()).arg(room.maxPlayers));
    
    playersList->clear();
    for (const QString& player : room.playerNames) {
        playersList->addItem(player + (player == room.hostName ? " (房主)" : ""));
    }
    
    // 如果是房主且人数足够，可以开始游戏
    if (isHost && room.playerNames.size() >= 2) {
        // 这里可以添加自动开始游戏的逻辑，或者显示开始按钮
        QTimer::singleShot(2000, [this, roomId]() {
            multiPlayerManager->startGame(roomId);
        });
    }
}

void GameWidget::onPlayerLeftRoom(const QString& roomId, const QString& playerName)
{
    if (roomId != currentRoomId) return;
    
    qDebug() << "Player left:" << playerName;
    
    // 更新玩家列表
    GameRoom room = multiPlayerManager->getRoomInfo(roomId);
    if (!room.roomId.isEmpty()) {
        playersLabel->setText(QString("房间: %1 (%2/%3)").arg(roomId).arg(room.playerNames.size()).arg(room.maxPlayers));
        
        playersList->clear();
        for (const QString& player : room.playerNames) {
            playersList->addItem(player + (player == room.hostName ? " (房主)" : ""));
        }
    }
    
    // 清理离开玩家的数据
    otherPlayers.remove(playerName);
    playerCharacters.remove(playerName);
    playerScores.remove(playerName);
    playerAliveStatus.remove(playerName);
}

void GameWidget::onGameStarted(const QString& roomId)
{
    if (roomId != currentRoomId) return;
    
    qDebug() << "Game started in room:" << roomId;
    
    // 重置游戏状态
    resetGame();
    currentState = GameState::MULTIPLAYER_GAME;
    
    // 初始化本地玩家的蛇
    Point startPos(gridWidth / 4, gridHeight / 4);
    snake->reset(startPos);
    
    // 启动游戏循环
    gameTimer->start(currentSpeed);
    
    update();
}

void GameWidget::onGameEnded(const QString& roomId, const QString& winner)
{
    if (roomId != currentRoomId) return;
    
    qDebug() << "Game ended. Winner:" << winner;
    
    currentState = GameState::GAME_OVER;
    gameTimer->stop();
    
    QString message = winner.isEmpty() ? "游戏结束！" : QString("游戏结束！\n获胜者: %1").arg(winner);
    QMessageBox::information(this, "游戏结束", message);
    
    update();
}

void GameWidget::onGameStateUpdated(const QString& roomId, const MultiPlayerGameState& gameState)
{
    if (roomId != currentRoomId) return;
    
    // 更新其他玩家的状态
    otherPlayers.clear();
    playerCharacters.clear();
    playerScores.clear();
    playerAliveStatus.clear();
    
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        const QString& playerName = it.key();
        if (playerName != this->playerName) { // 不包括自己
            otherPlayers[playerName] = it.value();
            playerCharacters[playerName] = gameState.playerCharacters.value(playerName, CharacterType::SPONGEBOB);
            playerScores[playerName] = gameState.playerScores.value(playerName, 0);
            playerAliveStatus[playerName] = gameState.playerAliveStatus.value(playerName, true);
        }
    }
    
    // 更新食物位置
    food->setPosition(gameState.foodPosition);
    food->setSpecial(gameState.isSpecialFood);
    
    // 检查游戏是否暂停
    if (gameState.isPaused && currentState != GameState::PAUSED) {
        currentState = GameState::PAUSED;
        gameTimer->stop();
    } else if (!gameState.isPaused && currentState == GameState::PAUSED) {
        currentState = GameState::MULTIPLAYER_GAME;
        gameTimer->start(currentSpeed);
    }
    
    update();
}

void GameWidget::onPlayerCollision(const QString& roomId, const QString& playerName)
{
    if (roomId != currentRoomId) return;
    
    qDebug() << "Player collision:" << playerName;
    
    if (playerName == this->playerName) {
        // 本地玩家碰撞
        currentState = GameState::GAME_OVER;
        gameTimer->stop();
    }
    
    // 标记玩家为死亡状态
    playerAliveStatus[playerName] = false;
    
    update();
}

void GameWidget::onFoodEaten(const QString& roomId, const QString& playerName, int points)
{
    if (roomId != currentRoomId) return;
    
    qDebug() << "Food eaten by" << playerName << "for" << points << "points";
    
    if (playerName == this->playerName) {
        // 本地玩家吃到食物
        updateScore(points);
    } else {
        // 其他玩家吃到食物，更新分数
        playerScores[playerName] = playerScores.value(playerName, 0) + points;
    }
    
    update();
}

void GameWidget::setMultiPlayerManager(MultiPlayerGameManager* manager)
{
    if (multiPlayerManager && multiPlayerManager != manager) {
        // 断开旧的连接
        disconnect(multiPlayerManager, nullptr, this, nullptr);
    }
    
    multiPlayerManager = manager;
    
    if (multiPlayerManager) {
        // 连接新的信号
        connect(multiPlayerManager, &MultiPlayerGameManager::roomCreated, this, &GameWidget::onRoomCreated);
        connect(multiPlayerManager, &MultiPlayerGameManager::playerJoinedRoom, this, &GameWidget::onPlayerJoinedRoom);
        connect(multiPlayerManager, &MultiPlayerGameManager::playerLeftRoom, this, &GameWidget::onPlayerLeftRoom);
        connect(multiPlayerManager, &MultiPlayerGameManager::gameStarted, this, &GameWidget::onGameStarted);
        connect(multiPlayerManager, &MultiPlayerGameManager::gameEnded, this, &GameWidget::onGameEnded);
        connect(multiPlayerManager, &MultiPlayerGameManager::gameStateUpdated, this, &GameWidget::onGameStateUpdated);
        connect(multiPlayerManager, &MultiPlayerGameManager::playerCollision, this, &GameWidget::onPlayerCollision);
        connect(multiPlayerManager, &MultiPlayerGameManager::foodEaten, this, &GameWidget::onFoodEaten);
        
        // 设置网络管理器
        multiPlayerManager->setNetworkManager(networkManager);
    }
}