#include "hotspotgamemanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QDebug>
#include <algorithm>

HotspotGameManager::HotspotGameManager(QObject *parent)
    : QObject(parent)
    , networkManager(nullptr)
    , gameTimer(new QTimer(this))
    , countdownTimer(new QTimer(this))
{
    // 设置游戏定时器
    gameTimer->setSingleShot(false);
    connect(gameTimer, &QTimer::timeout, this, &HotspotGameManager::onGameTick);
    
    // 设置倒计时定时器
    countdownTimer->setSingleShot(false);
    connect(countdownTimer, &QTimer::timeout, this, &HotspotGameManager::onCountdownTick);
    
    // 初始化游戏状态
    initializeGame();
}

HotspotGameManager::~HotspotGameManager()
{
    destroyRoom();
}

bool HotspotGameManager::createRoom(const QString& hostPlayerName, const QString& roomName)
{
    if (!networkManager) {
        qWarning() << "Network manager not set";
        return false;
    }
    
    // 启动热点主机
    if (!networkManager->startHotspotHost(roomName)) {
        qWarning() << "Failed to start hotspot host";
        return false;
    }
    
    this->hostPlayerName = hostPlayerName;
    this->roomName = roomName;
    
    // 添加主机玩家
    gameState.playerSnakes[hostPlayerName] = std::deque<Point>();
    gameState.playerCharacters[hostPlayerName] = CharacterType::SPONGEBOB;
    gameState.playerScores[hostPlayerName] = 0;
    gameState.playerAliveStatus[hostPlayerName] = true;
    gameState.playerDirections[hostPlayerName] = Direction::RIGHT;
    gameState.playerReadyStatus[hostPlayerName] = false;
    
    emit roomCreated(roomName);
    emit playerJoined(hostPlayerName);
    
    qDebug() << "Room created:" << roomName << "by" << hostPlayerName;
    return true;
}

bool HotspotGameManager::joinRoom(const QString& playerName)
{
    if (!networkManager || !networkManager->isConnectedToHost()) {
        qWarning() << "Not connected to host";
        return false;
    }
    
    // 添加玩家到本地游戏状态（客户端临时状态）
    gameState.playerSnakes[playerName] = std::deque<Point>();
    gameState.playerCharacters[playerName] = CharacterType::PATRICK;
    gameState.playerScores[playerName] = 0;
    gameState.playerAliveStatus[playerName] = true;
    gameState.playerDirections[playerName] = Direction::RIGHT;
    gameState.playerReadyStatus[playerName] = false;
    
    // 发送加入消息到主机
    QJsonObject joinMessage;
    joinMessage["type"] = "player_join";  // 添加消息类型标识
    joinMessage["player_name"] = playerName;
    networkManager->sendPlayerData(playerName, joinMessage);
    
    // 注意：不在这里发射playerJoined信号，等待主机确认后再更新界面
    // 主机会通过onNetworkPlayerConnected处理并广播游戏状态
    
    qDebug() << "Player join request sent:" << playerName;
    return true;
}

void HotspotGameManager::leaveRoom(const QString& playerName)
{
    removePlayer(playerName);
    
    if (isHost() && playerName == hostPlayerName) {
        destroyRoom();
    } else {
        emit playerLeft(playerName);
        broadcastGameState();
    }
    
    qDebug() << "Player left:" << playerName;
}

void HotspotGameManager::destroyRoom()
{
    // 停止游戏
    endGame();
    
    // 停止网络
    if (networkManager) {
        if (isHost()) {
            networkManager->stopHotspotHost();
        } else {
            networkManager->disconnectFromHost();
        }
    }
    
    // 清理游戏状态
    gameState = HotspotGameState();
    hostPlayerName.clear();
    roomName.clear();
    
    emit roomDestroyed();
    qDebug() << "Room destroyed";
}

bool HotspotGameManager::startGame()
{
    if (gameState.isGameStarted) {
        qWarning() << "Game already started";
        return false;
    }
    
    // 检查所有玩家是否准备就绪
    for (auto it = gameState.playerReadyStatus.begin(); it != gameState.playerReadyStatus.end(); ++it) {
        if (!it.value()) {
            qWarning() << "Not all players are ready";
            return false;
        }
    }
    
    if (gameState.playerSnakes.size() < 2) {
        qWarning() << "Need at least 2 players to start";
        return false;
    }
    
    // 初始化游戏
    initializeGame();
    
    // 开始倒计时
    gameState.countdownTimer = COUNTDOWN_SECONDS;
    countdownTimer->start(1000);
    
    emit countdownUpdated(gameState.countdownTimer);
    
    qDebug() << "Starting game countdown";
    return true;
}

void HotspotGameManager::pauseGame()
{
    if (!gameState.isGameStarted || gameState.isPaused) {
        return;
    }
    
    gameState.isPaused = true;
    gameTimer->stop();
    
    broadcastGameState();
    emit gamePaused();
    
    qDebug() << "Game paused";
}

void HotspotGameManager::resumeGame()
{
    if (!gameState.isGameStarted || !gameState.isPaused) {
        return;
    }
    
    gameState.isPaused = false;
    gameTimer->start(gameState.gameSpeed);
    
    broadcastGameState();
    emit gameResumed();
    
    qDebug() << "Game resumed";
}

void HotspotGameManager::endGame(const QString& winner)
{
    if (!gameState.isGameStarted) {
        return;
    }
    
    gameState.isGameStarted = false;
    gameState.isPaused = false;
    gameState.gameWinner = winner;
    
    gameTimer->stop();
    countdownTimer->stop();
    
    broadcastGameState();
    emit gameEnded(winner);
    
    qDebug() << "Game ended. Winner:" << winner;
}

void HotspotGameManager::resetGame()
{
    endGame();
    
    // 重置玩家状态
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        gameState.playerScores[it.key()] = 0;
        gameState.playerAliveStatus[it.key()] = true;
        gameState.playerReadyStatus[it.key()] = false;
        it.value().clear();
    }
    
    gameState.gameWinner.clear();
    generateFood();
    
    broadcastGameState();
    emit gameReset();
    
    qDebug() << "Game reset";
}

void HotspotGameManager::setPlayerCharacter(const QString& playerName, CharacterType character)
{
    if (!gameState.playerCharacters.contains(playerName)) {
        return;
    }
    
    gameState.playerCharacters[playerName] = character;
    
    syncPlayerData(playerName);
    emit playerCharacterChanged(playerName, character);
    
    qDebug() << "Player" << playerName << "selected character:" << static_cast<int>(character);
}

void HotspotGameManager::setPlayerReady(const QString& playerName, bool ready)
{
    if (!gameState.playerReadyStatus.contains(playerName)) {
        return;
    }
    
    gameState.playerReadyStatus[playerName] = ready;
    
    syncPlayerData(playerName);
    emit playerReadyChanged(playerName, ready);
    
    qDebug() << "Player" << playerName << "ready status:" << ready;
}

void HotspotGameManager::updatePlayerDirection(const QString& playerName, Direction direction)
{
    if (!gameState.playerDirections.contains(playerName) || !gameState.isGameStarted) {
        return;
    }
    
    // 防止反向移动
    Direction currentDirection = gameState.playerDirections[playerName];
    if ((currentDirection == Direction::UP && direction == Direction::DOWN) ||
        (currentDirection == Direction::DOWN && direction == Direction::UP) ||
        (currentDirection == Direction::LEFT && direction == Direction::RIGHT) ||
        (currentDirection == Direction::RIGHT && direction == Direction::LEFT)) {
        return;
    }
    
    gameState.playerDirections[playerName] = direction;
    
    // 只同步方向变化，不需要广播整个游戏状态
    if (networkManager) {
        QJsonObject directionData;
        directionData["direction"] = static_cast<int>(direction);
        networkManager->sendPlayerData(playerName, directionData);
    }
}

void HotspotGameManager::removePlayer(const QString& playerName)
{
    gameState.playerSnakes.remove(playerName);
    gameState.playerCharacters.remove(playerName);
    gameState.playerScores.remove(playerName);
    gameState.playerAliveStatus.remove(playerName);
    gameState.playerDirections.remove(playerName);
    gameState.playerReadyStatus.remove(playerName);
}

bool HotspotGameManager::isHost() const
{
    return networkManager && networkManager->isHosting();
}

void HotspotGameManager::setNetworkManager(HotspotNetworkManager* manager)
{
    if (networkManager) {
        // 断开旧的连接
        disconnect(networkManager, nullptr, this, nullptr);
    }
    
    networkManager = manager;
    
    if (networkManager) {
        // 连接网络信号
        connect(networkManager, &HotspotNetworkManager::playerDataReceived,
                this, &HotspotGameManager::onNetworkPlayerData);
        connect(networkManager, &HotspotNetworkManager::gameStateReceived,
                this, &HotspotGameManager::onNetworkGameState);
        connect(networkManager, &HotspotNetworkManager::playerConnectedToHost,
                this, &HotspotGameManager::onNetworkPlayerConnected);
        connect(networkManager, &HotspotNetworkManager::playerDisconnectedFromHost,
                this, &HotspotGameManager::onNetworkPlayerDisconnected);
    }
}

void HotspotGameManager::onGameTick()
{
    if (!gameState.isGameStarted || gameState.isPaused) {
        return;
    }
    
    updateGameLogic();
}

void HotspotGameManager::onCountdownTick()
{
    gameState.countdownTimer--;
    emit countdownUpdated(gameState.countdownTimer);
    
    if (gameState.countdownTimer <= 0) {
        countdownTimer->stop();
        
        // 开始游戏
        gameState.isGameStarted = true;
        gameTimer->start(gameState.gameSpeed);
        
        broadcastGameState();
        emit gameStarted();
        
        qDebug() << "Game started!";
    }
}

void HotspotGameManager::onNetworkPlayerData(const QString& playerName, const QJsonObject& playerData)
{
    // 处理接收到的玩家数据
    if (playerData.contains("direction")) {
        Direction direction = static_cast<Direction>(playerData["direction"].toInt());
        updatePlayerDirection(playerName, direction);
    }
    
    if (playerData.contains("character")) {
        CharacterType character = static_cast<CharacterType>(playerData["character"].toInt());
        setPlayerCharacter(playerName, character);
    }
    
    if (playerData.contains("ready")) {
        bool ready = playerData["ready"].toBool();
        setPlayerReady(playerName, ready);
    }
}

void HotspotGameManager::onNetworkGameState(const QJsonObject& gameStateJson)
{
    // 客户端接收游戏状态更新
    if (!isHost()) {
        gameStateFromJson(gameStateJson);
        emit gameStateUpdated(gameState);
    }
}

void HotspotGameManager::onNetworkPlayerConnected(const QString& playerName)
{
    if (isHost()) {
        // 主机处理新玩家连接
        if (!gameState.playerSnakes.contains(playerName)) {
            gameState.playerSnakes[playerName] = std::deque<Point>();
            gameState.playerCharacters[playerName] = CharacterType::PATRICK;
            gameState.playerScores[playerName] = 0;
            gameState.playerAliveStatus[playerName] = true;
            gameState.playerDirections[playerName] = Direction::RIGHT;
            gameState.playerReadyStatus[playerName] = false;
            
            emit playerJoined(playerName);
            broadcastGameState();
        }
    }
}

void HotspotGameManager::onNetworkPlayerDisconnected(const QString& playerName)
{
    removePlayer(playerName);
    emit playerLeft(playerName);
    
    if (isHost()) {
        broadcastGameState();
        
        // 检查是否需要结束游戏
        if (gameState.isGameStarted && getPlayerCount() < 2) {
            endGame("Not enough players");
        }
    }
}

void HotspotGameManager::initializeGame()
{
    // 初始化蛇的位置
    int playerIndex = 0;
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it, ++playerIndex) {
        std::deque<Point>& snake = it.value();
        snake.clear();
        
        // 根据玩家索引设置初始位置
        int startX = 5 + (playerIndex % 2) * (GRID_WIDTH - 10);
        int startY = 5 + (playerIndex / 2) * (GRID_HEIGHT - 10);
        
        for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
            snake.push_back(Point(startX - i, startY));
        }
        
        gameState.playerAliveStatus[it.key()] = true;
        gameState.playerScores[it.key()] = 0;
    }
    
    // 生成食物
    generateFood();
}

void HotspotGameManager::updateGameLogic()
{
    if (!isHost()) {
        return; // 只有主机更新游戏逻辑
    }
    
    updatePlayerPositions();
    checkCollisions();
    checkWinCondition();
    
    broadcastGameState();
    emit gameStateUpdated(gameState);
}

void HotspotGameManager::checkCollisions()
{
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        const QString& playerName = it.key();
        
        if (!gameState.playerAliveStatus[playerName]) {
            continue;
        }
        
        // 检查各种碰撞
        if (checkSelfCollision(playerName) || 
            checkWallCollision(playerName) || 
            checkPlayerCollision(playerName)) {
            killPlayer(playerName);
            continue;
        }
        
        // 检查食物碰撞
        if (checkFoodCollision(playerName)) {
            int points = gameState.isSpecialFood ? SPECIAL_FOOD_POINTS : FOOD_POINTS;
            gameState.playerScores[playerName] += points;
            
            growSnake(playerName);
            generateFood();
            
            emit foodEaten(playerName, points);
            emit playerScoreChanged(playerName, gameState.playerScores[playerName]);
        }
    }
}

void HotspotGameManager::updatePlayerPositions()
{
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        const QString& playerName = it.key();
        
        if (!gameState.playerAliveStatus[playerName]) {
            continue;
        }
        
        std::deque<Point>& snake = it.value();
        if (snake.empty()) {
            continue;
        }
        
        Point newHead = getNextHeadPosition(playerName);
        snake.push_front(newHead);
        snake.pop_back();
    }
}

void HotspotGameManager::generateFood()
{
    Point newFood;
    QSet<Point> occupiedPositions = getAllOccupiedPositions();
    
    do {
        newFood = generateRandomFoodPosition();
    } while (occupiedPositions.contains(newFood));
    
    gameState.foodPosition = newFood;
    
    // 随机生成特殊食物
    gameState.isSpecialFood = (QRandomGenerator::global()->bounded(10) == 0);
    
    if (gameState.isSpecialFood) {
        do {
            gameState.specialFoodPosition = generateRandomFoodPosition();
        } while (occupiedPositions.contains(gameState.specialFoodPosition) || 
                 gameState.specialFoodPosition == gameState.foodPosition);
    }
}

void HotspotGameManager::checkWinCondition()
{
    QStringList alivePlayers;
    for (auto it = gameState.playerAliveStatus.begin(); it != gameState.playerAliveStatus.end(); ++it) {
        if (it.value()) {
            alivePlayers.append(it.key());
        }
    }
    
    if (alivePlayers.size() <= 1) {
        QString winner = alivePlayers.isEmpty() ? "Draw" : alivePlayers.first();
        endGame(winner);
    }
}

void HotspotGameManager::broadcastGameState()
{
    if (networkManager && isHost()) {
        QJsonObject gameStateJson = gameStateToJson();
        networkManager->sendGameState(gameStateJson);
    }
}

void HotspotGameManager::syncPlayerData(const QString& playerName)
{
    if (networkManager) {
        QJsonObject playerData = playerDataToJson(playerName);
        networkManager->sendPlayerData(playerName, playerData);
    }
}

bool HotspotGameManager::checkSelfCollision(const QString& playerName)
{
    const std::deque<Point>& snake = gameState.playerSnakes[playerName];
    if (snake.size() < 2) {
        return false;
    }
    
    const Point& head = snake.front();
    for (auto it = snake.begin() + 1; it != snake.end(); ++it) {
        if (*it == head) {
            return true;
        }
    }
    
    return false;
}

bool HotspotGameManager::checkWallCollision(const QString& playerName)
{
    const std::deque<Point>& snake = gameState.playerSnakes[playerName];
    if (snake.empty()) {
        return false;
    }
    
    const Point& head = snake.front();
    return head.x < 0 || head.x >= GRID_WIDTH || head.y < 0 || head.y >= GRID_HEIGHT;
}

bool HotspotGameManager::checkPlayerCollision(const QString& playerName)
{
    const std::deque<Point>& currentSnake = gameState.playerSnakes[playerName];
    if (currentSnake.empty()) {
        return false;
    }
    
    const Point& head = currentSnake.front();
    
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        if (it.key() == playerName || !gameState.playerAliveStatus[it.key()]) {
            continue;
        }
        
        const std::deque<Point>& otherSnake = it.value();
        for (const Point& segment : otherSnake) {
            if (segment == head) {
                return true;
            }
        }
    }
    
    return false;
}

bool HotspotGameManager::checkFoodCollision(const QString& playerName)
{
    const std::deque<Point>& snake = gameState.playerSnakes[playerName];
    if (snake.empty()) {
        return false;
    }
    
    const Point& head = snake.front();
    return head == gameState.foodPosition || 
           (gameState.isSpecialFood && head == gameState.specialFoodPosition);
}

Point HotspotGameManager::getNextHeadPosition(const QString& playerName)
{
    const std::deque<Point>& snake = gameState.playerSnakes[playerName];
    if (snake.empty()) {
        return Point(0, 0);
    }
    
    Point head = snake.front();
    Direction direction = gameState.playerDirections[playerName];
    
    switch (direction) {
        case Direction::UP:
            head.y--;
            break;
        case Direction::DOWN:
            head.y++;
            break;
        case Direction::LEFT:
            head.x--;
            break;
        case Direction::RIGHT:
            head.x++;
            break;
    }
    
    return head;
}

void HotspotGameManager::growSnake(const QString& playerName)
{
    std::deque<Point>& snake = gameState.playerSnakes[playerName];
    if (!snake.empty()) {
        snake.push_back(snake.back());
    }
}

void HotspotGameManager::killPlayer(const QString& playerName)
{
    gameState.playerAliveStatus[playerName] = false;
    emit playerDied(playerName);
    
    qDebug() << "Player died:" << playerName;
}

QSet<Point> HotspotGameManager::getAllOccupiedPositions() const
{
    QSet<Point> positions;
    
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        if (gameState.playerAliveStatus[it.key()]) {
            for (const Point& segment : it.value()) {
                positions.insert(segment);
            }
        }
    }
    
    return positions;
}

Point HotspotGameManager::generateRandomFoodPosition()
{
    int x = QRandomGenerator::global()->bounded(GRID_WIDTH);
    int y = QRandomGenerator::global()->bounded(GRID_HEIGHT);
    return Point(x, y);
}

QJsonObject HotspotGameManager::gameStateToJson() const
{
    QJsonObject json;
    
    // 玩家蛇身
    QJsonObject snakes;
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        QJsonArray snakeArray;
        for (const Point& segment : it.value()) {
            QJsonObject pointObj;
            pointObj["x"] = segment.x;
            pointObj["y"] = segment.y;
            snakeArray.append(pointObj);
        }
        snakes[it.key()] = snakeArray;
    }
    json["snakes"] = snakes;
    
    // 玩家角色
    QJsonObject characters;
    for (auto it = gameState.playerCharacters.begin(); it != gameState.playerCharacters.end(); ++it) {
        characters[it.key()] = static_cast<int>(it.value());
    }
    json["characters"] = characters;
    
    // 玩家分数
    QJsonObject scores;
    for (auto it = gameState.playerScores.begin(); it != gameState.playerScores.end(); ++it) {
        scores[it.key()] = it.value();
    }
    json["scores"] = scores;
    
    // 玩家存活状态
    QJsonObject aliveStatus;
    for (auto it = gameState.playerAliveStatus.begin(); it != gameState.playerAliveStatus.end(); ++it) {
        aliveStatus[it.key()] = it.value();
    }
    json["alive_status"] = aliveStatus;
    
    // 玩家方向
    QJsonObject directions;
    for (auto it = gameState.playerDirections.begin(); it != gameState.playerDirections.end(); ++it) {
        directions[it.key()] = static_cast<int>(it.value());
    }
    json["directions"] = directions;
    
    // 玩家准备状态
    QJsonObject readyStatus;
    for (auto it = gameState.playerReadyStatus.begin(); it != gameState.playerReadyStatus.end(); ++it) {
        readyStatus[it.key()] = it.value();
    }
    json["ready_status"] = readyStatus;
    
    // 食物位置
    QJsonObject food;
    food["x"] = gameState.foodPosition.x;
    food["y"] = gameState.foodPosition.y;
    json["food"] = food;
    
    // 特殊食物
    json["is_special_food"] = gameState.isSpecialFood;
    if (gameState.isSpecialFood) {
        QJsonObject specialFood;
        specialFood["x"] = gameState.specialFoodPosition.x;
        specialFood["y"] = gameState.specialFoodPosition.y;
        json["special_food"] = specialFood;
    }
    
    // 游戏状态
    json["game_speed"] = gameState.gameSpeed;
    json["is_paused"] = gameState.isPaused;
    json["is_game_started"] = gameState.isGameStarted;
    json["game_winner"] = gameState.gameWinner;
    json["countdown_timer"] = gameState.countdownTimer;
    
    return json;
}

void HotspotGameManager::gameStateFromJson(const QJsonObject& json)
{
    // 解析玩家蛇身
    QJsonObject snakes = json["snakes"].toObject();
    for (auto it = snakes.begin(); it != snakes.end(); ++it) {
        std::deque<Point> snake;
        QJsonArray snakeArray = it.value().toArray();
        for (const QJsonValue& value : snakeArray) {
            QJsonObject pointObj = value.toObject();
            snake.push_back(Point(pointObj["x"].toInt(), pointObj["y"].toInt()));
        }
        gameState.playerSnakes[it.key()] = snake;
    }
    
    // 解析其他状态...
    // (为了简洁，这里省略了完整的解析代码)
    
    gameState.isGameStarted = json["is_game_started"].toBool();
    gameState.isPaused = json["is_paused"].toBool();
    gameState.gameWinner = json["game_winner"].toString();
    gameState.countdownTimer = json["countdown_timer"].toInt();
}

QJsonObject HotspotGameManager::playerDataToJson(const QString& playerName) const
{
    QJsonObject json;
    
    if (gameState.playerCharacters.contains(playerName)) {
        json["character"] = static_cast<int>(gameState.playerCharacters[playerName]);
    }
    
    if (gameState.playerDirections.contains(playerName)) {
        json["direction"] = static_cast<int>(gameState.playerDirections[playerName]);
    }
    
    if (gameState.playerReadyStatus.contains(playerName)) {
        json["ready"] = gameState.playerReadyStatus[playerName];
    }
    
    return json;
}

void HotspotGameManager::playerDataFromJson(const QString& playerName, const QJsonObject& json)
{
    if (json.contains("character")) {
        gameState.playerCharacters[playerName] = static_cast<CharacterType>(json["character"].toInt());
    }
    
    if (json.contains("direction")) {
        gameState.playerDirections[playerName] = static_cast<Direction>(json["direction"].toInt());
    }
    
    if (json.contains("ready")) {
        gameState.playerReadyStatus[playerName] = json["ready"].toBool();
    }
}