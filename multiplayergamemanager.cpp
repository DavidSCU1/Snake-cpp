#include "multiplayergamemanager.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MultiPlayerGameManager::MultiPlayerGameManager(QObject *parent)
    : QObject(parent)
    , networkManager(nullptr)
    , gameTimer(new QTimer(this))
{
    gameTimer->setInterval(200); // 200ms游戏循环
    connect(gameTimer, &QTimer::timeout, this, &MultiPlayerGameManager::onGameTick);
}

QString MultiPlayerGameManager::createRoom(const QString& hostName, int maxPlayers)
{
    // 检查该玩家是否已经创建了房间
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        const GameRoom& existingRoom = it.value();
        if (existingRoom.hostName == hostName) {
            qDebug() << "Player" << hostName << "already has a room:" << existingRoom.roomId;
            return existingRoom.roomId; // 返回已存在的房间ID
        }
    }
    
    QString roomId = generateRoomId();
    
    GameRoom room;
    room.roomId = roomId;
    room.hostName = hostName;
    room.maxPlayers = maxPlayers;
    room.currentPlayers = 1;
    room.isGameStarted = false;
    room.playerNames.append(hostName);
    
    rooms[roomId] = room;
    
    // 初始化游戏状态
    initializeGameState(roomId);
    
    // 将主机玩家添加到游戏状态
    MultiPlayerGameState& gameState = gameStates[roomId];
    gameState.playerAliveStatus[hostName] = true;
    gameState.playerScores[hostName] = 0;
    gameState.playerCharacters[hostName] = CharacterType::SPONGEBOB; // 默认角色
    
    // 启动网络服务器并广播房间信息
    if (networkManager) {
        if (!networkManager->isServerRunning()) {
            if (networkManager->startServer()) {
                qDebug() << "Server started for room" << roomId;
            } else {
                qDebug() << "Failed to start server for room" << roomId;
            }
        }
        networkManager->broadcastRoomInfo();
    }
    
    emit roomCreated(roomId, room);
    
    qDebug() << "Room created:" << roomId << "by" << hostName;
    return roomId;
}

bool MultiPlayerGameManager::joinRoom(const QString& roomId, const QString& playerName)
{
    if (!rooms.contains(roomId)) {
        qDebug() << "Room not found:" << roomId;
        return false;
    }
    
    GameRoom& room = rooms[roomId];
    
    // 检查房间是否已满
    if (room.currentPlayers >= room.maxPlayers) {
        qDebug() << "Room is full:" << roomId;
        return false;
    }
    
    // 检查玩家名是否已存在
    if (room.playerNames.contains(playerName)) {
        qDebug() << "Player name already exists:" << playerName;
        return false;
    }
    
    // 检查游戏是否已开始
    if (room.isGameStarted) {
        qDebug() << "Game already started in room:" << roomId;
        return false;
    }
    
    // 添加玩家到房间
    room.playerNames.append(playerName);
    room.currentPlayers++;
    
    // 添加玩家到游戏状态
    MultiPlayerGameState& gameState = gameStates[roomId];
    gameState.playerAliveStatus[playerName] = true;
    gameState.playerScores[playerName] = 0;
    gameState.playerCharacters[playerName] = CharacterType::PATRICK; // 默认角色
    
    emit playerJoinedRoom(roomId, playerName);
    
    qDebug() << "Player" << playerName << "joined room" << roomId;
    return true;
}

bool MultiPlayerGameManager::leaveRoom(const QString& roomId, const QString& playerName)
{
    if (!rooms.contains(roomId)) {
        return false;
    }
    
    GameRoom& room = rooms[roomId];
    
    if (!room.playerNames.contains(playerName)) {
        return false;
    }
    
    // 从房间移除玩家
    room.playerNames.removeAll(playerName);
    room.currentPlayers--;
    
    // 从游戏状态移除玩家
    if (gameStates.contains(roomId)) {
        MultiPlayerGameState& gameState = gameStates[roomId];
        gameState.playerAliveStatus.remove(playerName);
        gameState.playerScores.remove(playerName);
        gameState.playerCharacters.remove(playerName);
        gameState.playerSnakes.remove(playerName);
        gameState.playerDirections.remove(playerName);
    }
    
    emit playerLeftRoom(roomId, playerName);
    
    // 如果房间空了，销毁房间
    if (room.currentPlayers == 0) {
        destroyRoom(roomId);
    } else if (room.hostName == playerName && !room.playerNames.isEmpty()) {
        // 如果主机离开，转移主机权限给第一个玩家
        room.hostName = room.playerNames.first();
    }
    
    qDebug() << "Player" << playerName << "left room" << roomId;
    return true;
}

void MultiPlayerGameManager::destroyRoom(const QString& roomId)
{
    if (!rooms.contains(roomId)) {
        return;
    }
    
    // 停止游戏定时器
    if (gameStates.contains(roomId) && gameStates[roomId].isPaused == false) {
        gameTimer->stop();
    }
    
    rooms.remove(roomId);
    gameStates.remove(roomId);
    
    // 如果没有房间了，停止服务器
    if (rooms.isEmpty() && networkManager && networkManager->isServerRunning()) {
        networkManager->stopServer();
        qDebug() << "Server stopped - no rooms remaining";
    }
    
    emit roomDestroyed(roomId);
    
    qDebug() << "Room destroyed:" << roomId;
}

QStringList MultiPlayerGameManager::getAvailableRooms() const
{
    QStringList roomIds;
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        const GameRoom& room = it.value();
        // 只返回未开始且未满的房间
        if (!room.isGameStarted && room.currentPlayers < room.maxPlayers) {
            roomIds.append(it.key());
        }
    }
    return roomIds;
}

GameRoom MultiPlayerGameManager::getRoomInfo(const QString& roomId) const
{
    return rooms.value(roomId, GameRoom());
}

bool MultiPlayerGameManager::startGame(const QString& roomId)
{
    if (!rooms.contains(roomId)) {
        return false;
    }
    
    GameRoom& room = rooms[roomId];
    
    if (room.isGameStarted) {
        return false;
    }
    
    if (room.currentPlayers < 2) {
        qDebug() << "Not enough players to start game in room" << roomId;
        return false;
    }
    
    room.isGameStarted = true;
    
    // 初始化游戏状态
    initializeGameState(roomId);
    
    // 启动游戏循环
    if (!gameTimer->isActive()) {
        gameTimer->start();
    }
    
    emit gameStarted(roomId);
    
    qDebug() << "Game started in room" << roomId;
    return true;
}

void MultiPlayerGameManager::pauseGame(const QString& roomId)
{
    if (gameStates.contains(roomId)) {
        gameStates[roomId].isPaused = true;
        qDebug() << "Game paused in room" << roomId;
    }
}

void MultiPlayerGameManager::resumeGame(const QString& roomId)
{
    if (gameStates.contains(roomId)) {
        gameStates[roomId].isPaused = false;
        qDebug() << "Game resumed in room" << roomId;
    }
}

void MultiPlayerGameManager::endGame(const QString& roomId)
{
    if (!rooms.contains(roomId) || !gameStates.contains(roomId)) {
        return;
    }
    
    GameRoom& room = rooms[roomId];
    room.isGameStarted = false;
    
    // 找出获胜者（得分最高的玩家）
    const MultiPlayerGameState& gameState = gameStates[roomId];
    QString winner;
    int highestScore = -1;
    
    for (auto it = gameState.playerScores.begin(); it != gameState.playerScores.end(); ++it) {
        if (it.value() > highestScore) {
            highestScore = it.value();
            winner = it.key();
        }
    }
    
    emit gameEnded(roomId, winner);
    
    qDebug() << "Game ended in room" << roomId << "Winner:" << winner;
}

void MultiPlayerGameManager::updatePlayerDirection(const QString& roomId, const QString& playerName, Direction direction)
{
    if (gameStates.contains(roomId)) {
        gameStates[roomId].playerDirections[playerName] = direction;
    }
}

void MultiPlayerGameManager::updatePlayerPosition(const QString& roomId, const QString& playerName, const std::deque<Point>& snakeBody)
{
    if (gameStates.contains(roomId)) {
        gameStates[roomId].playerSnakes[playerName] = snakeBody;
        syncGameState(roomId);
    }
}

void MultiPlayerGameManager::updatePlayerScore(const QString& roomId, const QString& playerName, int score)
{
    if (gameStates.contains(roomId)) {
        gameStates[roomId].playerScores[playerName] = score;
        syncGameState(roomId);
    }
}

void MultiPlayerGameManager::setPlayerCharacter(const QString& roomId, const QString& playerName, CharacterType character)
{
    if (gameStates.contains(roomId)) {
        gameStates[roomId].playerCharacters[playerName] = character;
    }
}

void MultiPlayerGameManager::syncGameState(const QString& roomId)
{
    if (gameStates.contains(roomId)) {
        emit gameStateUpdated(roomId, gameStates[roomId]);
        broadcastGameState(roomId);
    }
}

MultiPlayerGameState MultiPlayerGameManager::getGameState(const QString& roomId) const
{
    return gameStates.value(roomId, MultiPlayerGameState());
}

bool MultiPlayerGameManager::checkPlayerCollision(const QString& roomId, const QString& playerName, const Point& newHead)
{
    if (!gameStates.contains(roomId)) {
        return false;
    }
    
    const MultiPlayerGameState& gameState = gameStates[roomId];
    
    // 检查边界碰撞
    if (newHead.x < 0 || newHead.x >= GRID_WIDTH || newHead.y < 0 || newHead.y >= GRID_HEIGHT) {
        return true;
    }
    
    // 检查与所有蛇身的碰撞（包括自己的）
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        const std::deque<Point>& snake = it.value();
        
        // 对于自己的蛇，跳过头部（因为头部会移动）
        if (it.key() == playerName && !snake.empty()) {
            for (size_t i = 1; i < snake.size(); ++i) {
                if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
                    return true;
                }
            }
        } else {
            // 对于其他玩家的蛇，检查整个身体
            for (const Point& segment : snake) {
                if (segment.x == newHead.x && segment.y == newHead.y) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool MultiPlayerGameManager::checkFoodCollision(const QString& roomId, const Point& position)
{
    if (!gameStates.contains(roomId)) {
        return false;
    }
    
    const MultiPlayerGameState& gameState = gameStates[roomId];
    return (gameState.foodPosition.x == position.x && gameState.foodPosition.y == position.y);
}

void MultiPlayerGameManager::generateNewFood(const QString& roomId, int gridWidth, int gridHeight)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    MultiPlayerGameState& gameState = gameStates[roomId];
    
    // 获取所有被占用的位置
    QSet<Point> occupiedPositions = getAllOccupiedPositions(roomId);
    
    // 生成新的食物位置
    Point newFoodPos;
    int attempts = 0;
    const int maxAttempts = 1000;
    
    do {
        newFoodPos.x = QRandomGenerator::global()->bounded(gridWidth);
        newFoodPos.y = QRandomGenerator::global()->bounded(gridHeight);
        attempts++;
    } while (occupiedPositions.contains(newFoodPos) && attempts < maxAttempts);
    
    if (attempts < maxAttempts) {
        gameState.foodPosition = newFoodPos;
        syncGameState(roomId);
    }
}

void MultiPlayerGameManager::setNetworkManager(NetworkManager* manager)
{
    networkManager = manager;
    
    if (networkManager) {
        connect(networkManager, &NetworkManager::playerInfoReceived,
                this, &MultiPlayerGameManager::onNetworkPlayerInfoReceived);
        connect(networkManager, &NetworkManager::playerPositionReceived,
                this, &MultiPlayerGameManager::onNetworkPlayerPositionReceived);
        connect(networkManager, &NetworkManager::playerDisconnected,
                this, &MultiPlayerGameManager::onNetworkPlayerDisconnected);
    }
}

void MultiPlayerGameManager::broadcastGameState(const QString& roomId)
{
    if (!networkManager || !gameStates.contains(roomId)) {
        return;
    }
    
    const MultiPlayerGameState& gameState = gameStates[roomId];
    
    QJsonObject stateObj;
    stateObj["roomId"] = roomId;
    stateObj["foodPosition"] = QJsonObject{{"x", gameState.foodPosition.x}, {"y", gameState.foodPosition.y}};
    stateObj["isSpecialFood"] = gameState.isSpecialFood;
    stateObj["gameSpeed"] = gameState.gameSpeed;
    stateObj["isPaused"] = gameState.isPaused;
    
    // 玩家信息
    QJsonObject playersObj;
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        const QString& playerName = it.key();
        const std::deque<Point>& snake = it.value();
        
        QJsonArray snakeArray;
        for (const Point& segment : snake) {
            snakeArray.append(QJsonObject{{"x", segment.x}, {"y", segment.y}});
        }
        
        QJsonObject playerObj;
        playerObj["snake"] = snakeArray;
        playerObj["score"] = gameState.playerScores.value(playerName, 0);
        playerObj["character"] = static_cast<int>(gameState.playerCharacters.value(playerName, CharacterType::SPONGEBOB));
        playerObj["alive"] = gameState.playerAliveStatus.value(playerName, true);
        
        playersObj[playerName] = playerObj;
    }
    stateObj["players"] = playersObj;
    
    QJsonObject message;
    message["type"] = "gameState";
    message["data"] = stateObj;
    networkManager->broadcastMessage(message);
}

void MultiPlayerGameManager::sendPlayerUpdate(const QString& roomId, const QString& playerName)
{
    if (!networkManager || !gameStates.contains(roomId)) {
        return;
    }
    
    const MultiPlayerGameState& gameState = gameStates[roomId];
    
    if (!gameState.playerSnakes.contains(playerName)) {
        return;
    }
    
    const std::deque<Point>& snake = gameState.playerSnakes[playerName];
    QJsonArray snakeArray;
    for (const Point& segment : snake) {
        snakeArray.append(QJsonObject{{"x", segment.x}, {"y", segment.y}});
    }
    
    QJsonObject updateObj;
    updateObj["roomId"] = roomId;
    updateObj["playerName"] = playerName;
    updateObj["snake"] = snakeArray;
    updateObj["score"] = gameState.playerScores.value(playerName, 0);
    
    QJsonObject message;
    message["type"] = "playerUpdate";
    message["data"] = updateObj;
    networkManager->broadcastMessage(message);
}

void MultiPlayerGameManager::onGameTick()
{
    // 更新所有活跃房间的游戏逻辑
    for (auto it = gameStates.begin(); it != gameStates.end(); ++it) {
        const QString& roomId = it.key();
        if (rooms.contains(roomId) && rooms[roomId].isGameStarted && !it.value().isPaused) {
            updateGameLogic(roomId);
        }
    }
}

void MultiPlayerGameManager::onNetworkPlayerInfoReceived(const PlayerInfo& playerInfo)
{
    // 处理网络玩家信息
    qDebug() << "Received player info:" << playerInfo.name;
}

void MultiPlayerGameManager::onNetworkPlayerPositionReceived(const QString& playerName, const std::deque<Point>& snakeBody)
{
    // 更新网络玩家位置
    for (auto it = gameStates.begin(); it != gameStates.end(); ++it) {
        if (it.value().playerSnakes.contains(playerName)) {
            it.value().playerSnakes[playerName] = snakeBody;
            syncGameState(it.key());
            break;
        }
    }
}

void MultiPlayerGameManager::onNetworkPlayerDisconnected(const QString& playerName)
{
    // 处理玩家断线
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        if (it.value().playerNames.contains(playerName)) {
            leaveRoom(it.key(), playerName);
            break;
        }
    }
}

void MultiPlayerGameManager::initializeGameState(const QString& roomId)
{
    if (!rooms.contains(roomId)) {
        return;
    }
    
    MultiPlayerGameState gameState;
    gameState.gameSpeed = 200;
    gameState.isPaused = false;
    gameState.isSpecialFood = false;
    
    // 初始化食物位置
    gameState.foodPosition.x = QRandomGenerator::global()->bounded(GRID_WIDTH);
    gameState.foodPosition.y = QRandomGenerator::global()->bounded(GRID_HEIGHT);
    
    // 为每个玩家初始化蛇的位置
    const GameRoom& room = rooms[roomId];
    int playerIndex = 0;
    for (const QString& playerName : room.playerNames) {
        // 在不同位置生成蛇
        std::deque<Point> snake;
        Point startPos;
        
        switch (playerIndex % 4) {
        case 0: // 左上角
            startPos = {5, 5};
            break;
        case 1: // 右上角
            startPos = {GRID_WIDTH - 6, 5};
            break;
        case 2: // 左下角
            startPos = {5, GRID_HEIGHT - 6};
            break;
        case 3: // 右下角
            startPos = {GRID_WIDTH - 6, GRID_HEIGHT - 6};
            break;
        }
        
        // 创建初始蛇身（3个段）
        for (int i = 0; i < 3; ++i) {
            Point segment = {startPos.x, startPos.y + i};
            snake.push_back(segment);
        }
        
        gameState.playerSnakes[playerName] = snake;
        gameState.playerDirections[playerName] = Direction::UP;
        gameState.playerAliveStatus[playerName] = true;
        gameState.playerScores[playerName] = 0;
        
        playerIndex++;
    }
    
    gameStates[roomId] = gameState;
}

void MultiPlayerGameManager::updateGameLogic(const QString& roomId)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    MultiPlayerGameState& gameState = gameStates[roomId];
    
    // 更新每个玩家的蛇
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        const QString& playerName = it.key();
        std::deque<Point>& snake = it.value();
        
        if (!gameState.playerAliveStatus.value(playerName, false) || snake.empty()) {
            continue;
        }
        
        // 获取当前方向
        Direction direction = gameState.playerDirections.value(playerName, Direction::UP);
        
        // 计算新的头部位置
        Point newHead = snake.front();
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
        
        // 检查碰撞
        if (checkPlayerCollision(roomId, playerName, newHead)) {
            gameState.playerAliveStatus[playerName] = false;
            emit playerCollision(roomId, playerName);
            continue;
        }
        
        // 移动蛇
        snake.push_front(newHead);
        
        // 检查是否吃到食物
        if (checkFoodCollision(roomId, newHead)) {
            gameState.playerScores[playerName] += 10;
            generateNewFood(roomId, GRID_WIDTH, GRID_HEIGHT);
            emit foodEaten(roomId, playerName, 10);
        } else {
            // 没吃到食物，移除尾部
            snake.pop_back();
        }
    }
    
    // 检查游戏结束条件
    checkWinCondition(roomId);
    
    // 同步游戏状态
    syncGameState(roomId);
}

void MultiPlayerGameManager::checkAllCollisions(const QString& roomId)
{
    // 这个方法可以用于更复杂的碰撞检测逻辑
}

void MultiPlayerGameManager::updateScores(const QString& roomId)
{
    // 这个方法可以用于更复杂的得分逻辑
}

void MultiPlayerGameManager::checkWinCondition(const QString& roomId)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    const MultiPlayerGameState& gameState = gameStates[roomId];
    
    // 计算存活玩家数量
    int aliveCount = 0;
    QString lastAlivePlayer;
    
    for (auto it = gameState.playerAliveStatus.begin(); it != gameState.playerAliveStatus.end(); ++it) {
        if (it.value()) {
            aliveCount++;
            lastAlivePlayer = it.key();
        }
    }
    
    // 如果只剩一个玩家或没有玩家存活，游戏结束
    if (aliveCount <= 1) {
        endGame(roomId);
    }
}

QSet<Point> MultiPlayerGameManager::getAllOccupiedPositions(const QString& roomId) const
{
    QSet<Point> occupied;
    
    if (!gameStates.contains(roomId)) {
        return occupied;
    }
    
    const MultiPlayerGameState& gameState = gameStates[roomId];
    
    // 添加所有蛇身位置
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        const std::deque<Point>& snake = it.value();
        for (const Point& segment : snake) {
            occupied.insert(segment);
        }
    }
    
    // 添加食物位置
    occupied.insert(gameState.foodPosition);
    
    return occupied;
}

QString MultiPlayerGameManager::generateRoomId() const
{
    // 生成6位随机房间ID
    QString roomId;
    for (int i = 0; i < 6; ++i) {
        roomId += QString::number(QRandomGenerator::global()->bounded(10));
    }
    return roomId;
}