#include "multiplayergamemanager.h"
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QDebug>
#include <QDateTime>
#include <algorithm>

MultiPlayerGameManager::MultiPlayerGameManager(QObject *parent)
    : QObject(parent)
    , gameTimer(new QTimer(this))
    , networkManager(nullptr)
{
    gameTimer->setInterval(GAME_TICK_INTERVAL);
    connect(gameTimer, &QTimer::timeout, this, &MultiPlayerGameManager::onGameTick);
}

QString MultiPlayerGameManager::createRoom(const QString& hostName, int maxPlayers)
{
    QString roomId = generateRoomId();
    
    GameRoom room;
    room.roomId = roomId;
    room.hostName = hostName;
    room.maxPlayers = qMax(2, qMin(8, maxPlayers)); // 限制2-8人
    room.players.append(hostName);
    room.isGameStarted = false;
    
    // 设置默认游戏配置
    room.gameSettings["gridWidth"] = GRID_WIDTH;
    room.gameSettings["gridHeight"] = GRID_HEIGHT;
    room.gameSettings["gameSpeed"] = GAME_TICK_INTERVAL;
    room.gameSettings["allowSpecialFood"] = true;
    
    rooms[roomId] = room;
    initializeGameState(roomId);
    
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
    
    if (room.isGameStarted) {
        qDebug() << "Game already started in room:" << roomId;
        return false;
    }
    
    if (room.players.size() >= room.maxPlayers) {
        qDebug() << "Room is full:" << roomId;
        return false;
    }
    
    if (room.players.contains(playerName)) {
        qDebug() << "Player already in room:" << playerName;
        return false;
    }
    
    room.players.append(playerName);
    
    // 初始化玩家游戏状态
    MultiPlayerGameState& gameState = gameStates[roomId];
    gameState.playerScores[playerName] = 0;
    gameState.playerAliveStatus[playerName] = true;
    gameState.playerCharacters[playerName] = CharacterType::SPONGEBOB;
    gameState.playerDirections[playerName] = Direction::RIGHT;
    
    // 设置玩家初始位置
    int playerIndex = room.players.size() - 1;
    Point startPos;
    switch (playerIndex) {
    case 0: startPos = Point(5, 5); break;
    case 1: startPos = Point(GRID_WIDTH - 6, 5); break;
    case 2: startPos = Point(5, GRID_HEIGHT - 6); break;
    case 3: startPos = Point(GRID_WIDTH - 6, GRID_HEIGHT - 6); break;
    default:
        // 随机位置
        startPos = Point(QRandomGenerator::global()->bounded(5, GRID_WIDTH - 5),
                        QRandomGenerator::global()->bounded(5, GRID_HEIGHT - 5));
    }
    
    std::deque<Point> initialSnake;
    initialSnake.push_back(startPos);
    initialSnake.push_back(Point(startPos.x - 1, startPos.y));
    initialSnake.push_back(Point(startPos.x - 2, startPos.y));
    gameState.playerSnakes[playerName] = initialSnake;
    
    emit playerJoinedRoom(roomId, playerName);
    qDebug() << "Player joined room:" << playerName << "->" << roomId;
    
    return true;
}

bool MultiPlayerGameManager::leaveRoom(const QString& roomId, const QString& playerName)
{
    if (!rooms.contains(roomId)) {
        return false;
    }
    
    GameRoom& room = rooms[roomId];
    room.players.removeAll(playerName);
    
    // 清理玩家游戏状态
    MultiPlayerGameState& gameState = gameStates[roomId];
    gameState.playerSnakes.remove(playerName);
    gameState.playerScores.remove(playerName);
    gameState.playerAliveStatus.remove(playerName);
    gameState.playerCharacters.remove(playerName);
    gameState.playerDirections.remove(playerName);
    
    emit playerLeftRoom(roomId, playerName);
    
    // 如果房主离开或房间为空，销毁房间
    if (room.players.isEmpty() || room.hostName == playerName) {
        destroyRoom(roomId);
    } else if (room.hostName == playerName && !room.players.isEmpty()) {
        // 转移房主权限
        room.hostName = room.players.first();
    }
    
    return true;
}

void MultiPlayerGameManager::destroyRoom(const QString& roomId)
{
    if (!rooms.contains(roomId)) {
        return;
    }
    
    rooms.remove(roomId);
    gameStates.remove(roomId);
    
    emit roomDestroyed(roomId);
    qDebug() << "Room destroyed:" << roomId;
}

QStringList MultiPlayerGameManager::getAvailableRooms() const
{
    QStringList availableRooms;
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        const GameRoom& room = it.value();
        if (!room.isGameStarted && room.players.size() < room.maxPlayers) {
            availableRooms.append(it.key());
        }
    }
    return availableRooms;
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
    if (room.isGameStarted || room.players.size() < 2) {
        return false;
    }
    
    room.isGameStarted = true;
    
    // 重新初始化游戏状态
    initializeGameState(roomId);
    generateNewFood(roomId, GRID_WIDTH, GRID_HEIGHT);
    
    // 启动游戏循环
    if (!gameTimer->isActive()) {
        gameTimer->start();
    }
    
    emit gameStarted(roomId);
    qDebug() << "Game started in room:" << roomId;
    
    return true;
}

void MultiPlayerGameManager::pauseGame(const QString& roomId)
{
    if (gameStates.contains(roomId)) {
        gameStates[roomId].isPaused = true;
        syncGameState(roomId);
    }
}

void MultiPlayerGameManager::resumeGame(const QString& roomId)
{
    if (gameStates.contains(roomId)) {
        gameStates[roomId].isPaused = false;
        syncGameState(roomId);
    }
}

void MultiPlayerGameManager::endGame(const QString& roomId)
{
    if (!rooms.contains(roomId)) {
        return;
    }
    
    GameRoom& room = rooms[roomId];
    room.isGameStarted = false;
    
    // 检查是否还有其他房间在游戏中
    bool hasActiveGames = false;
    for (const auto& r : rooms) {
        if (r.isGameStarted) {
            hasActiveGames = true;
            break;
        }
    }
    
    if (!hasActiveGames) {
        gameTimer->stop();
    }
    
    emit gameEnded(roomId, gameStates[roomId].winner);
    qDebug() << "Game ended in room:" << roomId;
}

void MultiPlayerGameManager::updatePlayerDirection(const QString& roomId, const QString& playerName, Direction direction)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    MultiPlayerGameState& gameState = gameStates[roomId];
    if (gameState.playerAliveStatus.value(playerName, false)) {
        gameState.playerDirections[playerName] = direction;
    }
}

void MultiPlayerGameManager::updatePlayerPosition(const QString& roomId, const QString& playerName, const std::deque<Point>& snakeBody)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    gameStates[roomId].playerSnakes[playerName] = snakeBody;
}

void MultiPlayerGameManager::updatePlayerScore(const QString& roomId, const QString& playerName, int score)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    gameStates[roomId].playerScores[playerName] = score;
}

void MultiPlayerGameManager::setPlayerCharacter(const QString& roomId, const QString& playerName, CharacterType character)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    gameStates[roomId].playerCharacters[playerName] = character;
}

void MultiPlayerGameManager::syncGameState(const QString& roomId)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    emit gameStateUpdated(roomId, gameStates[roomId]);
    broadcastGameState(roomId);
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
    
    // 检查与所有蛇的碰撞（包括自己的身体）
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        const QString& otherPlayer = it.key();
        const std::deque<Point>& otherSnake = it.value();
        
        if (!gameState.playerAliveStatus.value(otherPlayer, false)) {
            continue;
        }
        
        // 对于自己的蛇，跳过头部检查（因为头部会移动）
        size_t startIndex = (otherPlayer == playerName) ? 1 : 0;
        
        for (size_t i = startIndex; i < otherSnake.size(); ++i) {
            if (newHead == otherSnake[i]) {
                return true;
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
    
    return gameStates[roomId].foodPosition == position;
}

void MultiPlayerGameManager::generateNewFood(const QString& roomId, int gridWidth, int gridHeight)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    MultiPlayerGameState& gameState = gameStates[roomId];
    QSet<Point> occupiedPositions = getAllOccupiedPositions(roomId);
    
    Point newFoodPos;
    int attempts = 0;
    do {
        newFoodPos = Point(QRandomGenerator::global()->bounded(gridWidth),
                          QRandomGenerator::global()->bounded(gridHeight));
        attempts++;
    } while (occupiedPositions.contains(newFoodPos) && attempts < 100);
    
    gameState.foodPosition = newFoodPos;
    
    // 随机决定是否生成特殊食物
    gameState.isSpecialFood = (QRandomGenerator::global()->bounded(100) < 20); // 20%概率
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
    stateObj["isPaused"] = gameState.isPaused;
    stateObj["winner"] = gameState.winner;
    
    // 食物信息
    QJsonObject foodObj;
    foodObj["x"] = gameState.foodPosition.x;
    foodObj["y"] = gameState.foodPosition.y;
    foodObj["isSpecial"] = gameState.isSpecialFood;
    stateObj["food"] = foodObj;
    
    // 玩家信息
    QJsonArray playersArray;
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        const QString& playerName = it.key();
        
        QJsonObject playerObj;
        playerObj["name"] = playerName;
        playerObj["score"] = gameState.playerScores.value(playerName, 0);
        playerObj["isAlive"] = gameState.playerAliveStatus.value(playerName, false);
        playerObj["character"] = static_cast<int>(gameState.playerCharacters.value(playerName, CharacterType::SPONGEBOB));
        
        // 蛇身位置
        QJsonArray bodyArray;
        const std::deque<Point>& snake = it.value();
        for (const Point& point : snake) {
            QJsonObject pointObj;
            pointObj["x"] = point.x;
            pointObj["y"] = point.y;
            bodyArray.append(pointObj);
        }
        playerObj["body"] = bodyArray;
        
        playersArray.append(playerObj);
    }
    stateObj["players"] = playersArray;
    
    networkManager->sendGameState(stateObj);
}

void MultiPlayerGameManager::sendPlayerUpdate(const QString& roomId, const QString& playerName)
{
    if (!networkManager || !gameStates.contains(roomId)) {
        return;
    }
    
    const MultiPlayerGameState& gameState = gameStates[roomId];
    
    if (gameState.playerSnakes.contains(playerName)) {
        networkManager->sendPlayerPosition(gameState.playerSnakes[playerName]);
    }
    
    if (gameState.playerScores.contains(playerName)) {
        networkManager->sendScoreUpdate(gameState.playerScores[playerName]);
    }
}

void MultiPlayerGameManager::onGameTick()
{
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        const QString& roomId = it.key();
        const GameRoom& room = it.value();
        
        if (room.isGameStarted && gameStates.contains(roomId) && !gameStates[roomId].isPaused) {
            updateGameLogic(roomId);
        }
    }
}

void MultiPlayerGameManager::onNetworkPlayerInfoReceived(const PlayerInfo& playerInfo)
{
    // 处理网络接收到的玩家信息
    // 这里可以根据需要更新本地状态
}

void MultiPlayerGameManager::onNetworkPlayerPositionReceived(const QString& playerName, const std::deque<Point>& snakeBody)
{
    // 更新其他玩家的位置信息
    for (auto& gameState : gameStates) {
        if (gameState.playerSnakes.contains(playerName)) {
            gameState.playerSnakes[playerName] = snakeBody;
        }
    }
}

void MultiPlayerGameManager::onNetworkPlayerDisconnected(const QString& playerName)
{
    // 处理玩家断线
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        const QString& roomId = it.key();
        leaveRoom(roomId, playerName);
    }
}

void MultiPlayerGameManager::initializeGameState(const QString& roomId)
{
    if (!rooms.contains(roomId)) {
        return;
    }
    
    MultiPlayerGameState gameState;
    const GameRoom& room = rooms[roomId];
    
    // 初始化所有玩家状态
    for (const QString& playerName : room.players) {
        gameState.playerScores[playerName] = 0;
        gameState.playerAliveStatus[playerName] = true;
        gameState.playerCharacters[playerName] = CharacterType::SPONGEBOB;
        gameState.playerDirections[playerName] = Direction::RIGHT;
        
        // 设置初始蛇位置
        int playerIndex = room.players.indexOf(playerName);
        Point startPos;
        switch (playerIndex) {
        case 0: startPos = Point(5, 5); break;
        case 1: startPos = Point(GRID_WIDTH - 6, 5); break;
        case 2: startPos = Point(5, GRID_HEIGHT - 6); break;
        case 3: startPos = Point(GRID_WIDTH - 6, GRID_HEIGHT - 6); break;
        default:
            startPos = Point(QRandomGenerator::global()->bounded(5, GRID_WIDTH - 5),
                            QRandomGenerator::global()->bounded(5, GRID_HEIGHT - 5));
        }
        
        std::deque<Point> initialSnake;
        initialSnake.push_back(startPos);
        initialSnake.push_back(Point(startPos.x - 1, startPos.y));
        initialSnake.push_back(Point(startPos.x - 2, startPos.y));
        gameState.playerSnakes[playerName] = initialSnake;
    }
    
    gameStates[roomId] = gameState;
}

void MultiPlayerGameManager::updateGameLogic(const QString& roomId)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    MultiPlayerGameState& gameState = gameStates[roomId];
    
    // 移动所有活着的玩家的蛇
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        const QString& playerName = it.key();
        std::deque<Point>& snake = it.value();
        
        if (!gameState.playerAliveStatus.value(playerName, false) || snake.empty()) {
            continue;
        }
        
        // 计算新的头部位置
        Point head = snake.front();
        Point newHead = head;
        Direction direction = gameState.playerDirections.value(playerName, Direction::RIGHT);
        
        switch (direction) {
        case Direction::UP: newHead.y--; break;
        case Direction::DOWN: newHead.y++; break;
        case Direction::LEFT: newHead.x--; break;
        case Direction::RIGHT: newHead.x++; break;
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
            int points = gameState.isSpecialFood ? 50 : 10;
            gameState.playerScores[playerName] += points;
            emit foodEaten(roomId, playerName, points);
            
            generateNewFood(roomId, GRID_WIDTH, GRID_HEIGHT);
        } else {
            snake.pop_back(); // 没吃到食物，移除尾部
        }
    }
    
    checkWinCondition(roomId);
    syncGameState(roomId);
}

void MultiPlayerGameManager::checkAllCollisions(const QString& roomId)
{
    // 这个函数在updateGameLogic中已经包含了碰撞检测逻辑
}

void MultiPlayerGameManager::updateScores(const QString& roomId)
{
    // 分数更新在updateGameLogic中处理
}

void MultiPlayerGameManager::checkWinCondition(const QString& roomId)
{
    if (!gameStates.contains(roomId)) {
        return;
    }
    
    MultiPlayerGameState& gameState = gameStates[roomId];
    
    // 统计活着的玩家
    QStringList alivePlayers;
    for (auto it = gameState.playerAliveStatus.begin(); it != gameState.playerAliveStatus.end(); ++it) {
        if (it.value()) {
            alivePlayers.append(it.key());
        }
    }
    
    // 如果只剩一个玩家或没有玩家，游戏结束
    if (alivePlayers.size() <= 1) {
        if (alivePlayers.size() == 1) {
            gameState.winner = alivePlayers.first();
        } else {
            // 平局，选择分数最高的玩家
            QString topPlayer;
            int topScore = -1;
            for (auto it = gameState.playerScores.begin(); it != gameState.playerScores.end(); ++it) {
                if (it.value() > topScore) {
                    topScore = it.value();
                    topPlayer = it.key();
                }
            }
            gameState.winner = topPlayer;
        }
        
        endGame(roomId);
    }
}

QSet<Point> MultiPlayerGameManager::getAllOccupiedPositions(const QString& roomId) const
{
    QSet<Point> positions;
    
    if (!gameStates.contains(roomId)) {
        return positions;
    }
    
    const MultiPlayerGameState& gameState = gameStates[roomId];
    
    // 添加所有蛇的位置
    for (auto it = gameState.playerSnakes.begin(); it != gameState.playerSnakes.end(); ++it) {
        if (gameState.playerAliveStatus.value(it.key(), false)) {
            for (const Point& point : it.value()) {
                positions.insert(point);
            }
        }
    }
    
    return positions;
}

QString MultiPlayerGameManager::generateRoomId() const
{
    QString roomId;
    do {
        roomId = QString::number(QRandomGenerator::global()->bounded(100000, 999999));
    } while (rooms.contains(roomId));
    
    return roomId;
}