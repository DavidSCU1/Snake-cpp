#include "networkmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostAddress>
#include <QDebug>
#include <deque>
#include <QUdpSocket>
#include <QNetworkInterface>

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent)
    , server(nullptr)
    , clientSocket(nullptr)
    , udpSocket(new QUdpSocket(this))
    , heartbeatTimer(new QTimer(this))
    , roomBroadcastTimer(new QTimer(this))
    , allowJoinMidGame(true)
    , isServer(false)
{
    heartbeatTimer->setInterval(5000); // 5秒心跳
    connect(heartbeatTimer, &QTimer::timeout, this, &NetworkManager::sendHeartbeat);
    connect(udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::processRoomDiscovery);
    connect(roomBroadcastTimer, &QTimer::timeout, this, &NetworkManager::broadcastRoomInfo); // 定时广播房间信息
}

bool NetworkManager::startServer(quint16 port)
{
    if (server) {
        stopServer();
    }
    
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &NetworkManager::onNewConnection);
    
    if (server->listen(QHostAddress::Any, port)) {
        isServer = true;
        heartbeatTimer->start();
        
        // 确保UDP socket准备好用于广播
        if (udpSocket->state() != QAbstractSocket::BoundState) {
            // 如果UDP socket未绑定，尝试绑定到任意端口用于发送广播
            if (!udpSocket->bind()) {
                qDebug() << "Warning: Failed to bind UDP socket for broadcasting:" << udpSocket->errorString();
            }
        }
        
        roomBroadcastTimer->start(); // 启动房间广播定时器
        qDebug() << "Server started on port" << port;

        broadcastRoomInfo(); // 立即广播一次房间信息
        return true;
    } else {
        qDebug() << "Failed to start server:" << server->errorString();
        delete server;
        server = nullptr;
        return false;
    }
}

void NetworkManager::stopServer()
{
    if (server) {
        // 停止房间广播定时器
        roomBroadcastTimer->stop();
        // 断开所有客户端
        for (auto client : clients) {
            client->disconnectFromHost();
        }
        clients.clear();
        clientNames.clear();
        
        server->close();
        delete server;
        server = nullptr;
        isServer = false;
        heartbeatTimer->stop();
    }
}

bool NetworkManager::isServerRunning() const
{
    return server && server->isListening();
}

void NetworkManager::connectToServer(const QString& hostAddress, quint16 port)
{
    if (clientSocket) {
        disconnectFromServer();
    }
    
    clientSocket = new QTcpSocket(this);
    connect(clientSocket, &QTcpSocket::connected, this, &NetworkManager::onClientConnected);
    connect(clientSocket, &QTcpSocket::disconnected, this, &NetworkManager::onClientDisconnected);
    connect(clientSocket, &QTcpSocket::readyRead, this, &NetworkManager::onDataReceived);
    connect(clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkManager::onSocketError);
    
    clientSocket->connectToHost(hostAddress, port);
    isServer = false;
}

void NetworkManager::disconnectFromServer()
{
    if (clientSocket) {
        clientSocket->disconnectFromHost();
        clientSocket->deleteLater();
        clientSocket = nullptr;
        heartbeatTimer->stop();
    }
}

bool NetworkManager::isConnectedToServer() const
{
    return clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState;
}

void NetworkManager::sendPlayerInfo(const PlayerInfo& playerInfo)
{
    QJsonObject data;
    data["name"] = QString::fromStdString(playerInfo.name);
    data["score"] = playerInfo.score;
    data["character"] = static_cast<int>(playerInfo.character);
    data["isAlive"] = playerInfo.isAlive;
    
    QJsonObject message = createMessage("playerInfo", data);
    
    if (isServer) {
        broadcastMessage(message);
    } else if (clientSocket) {
        QJsonDocument doc(message);
        clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::sendGameState(const QJsonObject& gameState)
{
    QJsonObject message = createMessage("gameState", gameState);
    
    if (isServer) {
        broadcastMessage(message);
    } else if (clientSocket) {
        QJsonDocument doc(message);
        clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::sendScoreUpdate(int score)
{
    QJsonObject data;
    data["score"] = score;
    
    QJsonObject message = createMessage("scoreUpdate", data);
    
    if (isServer) {
        broadcastMessage(message);
    } else if (clientSocket) {
        QJsonDocument doc(message);
        clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::sendPlayerPosition(const QString& playerName, const std::deque<Point>& snakeBody)
{
    QJsonObject data;
    data["playerName"] = playerName;

    QJsonArray bodyArray;
    for (const auto& point : snakeBody) {
        QJsonObject pointObj;
        pointObj["x"] = point.x;
        pointObj["y"] = point.y;
        bodyArray.append(pointObj);
    }

    data["body"] = bodyArray;
    
    QJsonObject message = createMessage("playerPosition", data);
    
    if (isServer) {
        broadcastMessage(message);
    } else if (clientSocket) {
        QJsonDocument doc(message);
        clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::sendCharacterSelectionStart()
{
    QJsonObject data;
    data["roomId"] = "default"; // 可以根据需要传入实际的roomId
    
    QJsonObject message = createMessage("characterSelectionStart", data);
    
    if (isServer) {
        broadcastMessage(message);
    } else if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState) {
        QJsonDocument doc(message);
        clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::sendCharacterSelection(const QString& playerName, int character)
{
    QJsonObject data;
    data["roomId"] = "default"; // 可以根据需要传入实际的roomId
    data["playerName"] = playerName;
    data["character"] = character;
    
    QJsonObject message = createMessage("characterSelection", data);
    
    if (isServer) {
        broadcastMessage(message);
    } else if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState) {
        QJsonDocument doc(message);
        clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::sendMessage(const QByteArray& message)
{
    if (isServer) {
        // 服务器模式：广播给所有客户端
        for (auto client : clients) {
            if (client->state() == QAbstractSocket::ConnectedState) {
                client->write(message);
            }
        }
    } else if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState) {
        // 客户端模式：发送给服务器
        clientSocket->write(message);
    }
}

int NetworkManager::getConnectedPlayersCount() const
{
    if (isServer) {
        return clients.size();
    } else {
        return isConnectedToServer() ? 1 : 0;
    }
}

QTcpSocket* NetworkManager::getClientSocket() const
{
    return clientSocket;
}

void NetworkManager::onNewConnection()
{
    while (server->hasPendingConnections()) {
        QTcpSocket* client = server->nextPendingConnection();

        if (!allowJoinMidGame) {
            qDebug() << "New connection rejected: mid-game join not allowed.";
            client->disconnectFromHost();
            client->deleteLater();
            continue;
        }

        clients.append(client);
        connect(client, &QTcpSocket::readyRead, this, &NetworkManager::onDataReceived);
        connect(client, &QTcpSocket::disconnected, this, &NetworkManager::onClientDisconnected);
        connect(client, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
                this, &NetworkManager::onSocketError);

        qDebug() << "New client connected from" << client->peerAddress().toString();
    }
}

void NetworkManager::onClientConnected()
{
    qDebug() << "Connected to server";
    heartbeatTimer->start();
    // 新增：客户端连接成功后立即发送玩家信息
    if (!isServer && clientSocket) {
        // 发送完整的玩家信息
        QJsonObject playerData;
        playerData["name"] = pendingPlayerName;
        playerData["score"] = 0;
        playerData["character"] = static_cast<int>(CharacterType::PATRICK);
        playerData["isAlive"] = true;
        QJsonObject message = createMessage("playerInfo", playerData);
        QJsonDocument doc(message);
        clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
        qDebug() << "Sent player info for:" << pendingPlayerName;
    }
}

void NetworkManager::onClientDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    if (isServer) {
        QString playerName = clientNames.value(socket, "Unknown");
        clients.removeAll(socket);
        clientNames.remove(socket);
        emit playerDisconnected(playerName);
        qDebug() << "Client disconnected:" << playerName;
        
        // 正常断开连接时，不发送错误信息，只记录日志
        qDebug() << "Player" << playerName << "disconnected normally";
    } else {
        qDebug() << "Disconnected from server";
        heartbeatTimer->stop();
    }
    
    socket->deleteLater();
}

void NetworkManager::onDataReceived()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    QByteArray data = socket->readAll();
    QStringList messages = QString::fromUtf8(data).split("\n", Qt::SkipEmptyParts);
    
    for (const QString& messageStr : messages) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(messageStr.toUtf8(), &error);
        
        if (error.error == QJsonParseError::NoError && doc.isObject()) {
            processMessage(doc.object(), socket);
        }
    }
}

void NetworkManager::onSocketError(QAbstractSocket::SocketError error)
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        QString errorMessage;
        if (isServer) {
            // 服务器端：客户端连接出现问题
            QString playerName = clientNames.value(socket, "未知玩家");
            if (error == QAbstractSocket::RemoteHostClosedError) {
                errorMessage = QString("玩家 %1 断开了连接").arg(playerName);
            } else {
                errorMessage = QString("玩家 %1 连接出现错误: %2").arg(playerName).arg(socket->errorString());
            }
        } else {
            // 客户端：与服务器连接出现问题
            if (error == QAbstractSocket::RemoteHostClosedError) {
                errorMessage = "服务器关闭了连接";
            } else {
                errorMessage = QString("连接服务器时出现错误: %1").arg(socket->errorString());
            }
        }
        
        emit connectionError(errorMessage);
        qDebug() << "Socket error:" << errorMessage;
    }
}

void NetworkManager::sendHeartbeat()
{
    QJsonObject message = createMessage("heartbeat");
    
    if (isServer) {
        broadcastMessage(message);
    } else if (clientSocket) {
        QJsonDocument doc(message);
        clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::processMessage(const QJsonObject& message, QTcpSocket* sender)
{
    QString type = message["type"].toString();
    QJsonObject data = message["data"].toObject();
    
    if (type == "playerInfo") {
        PlayerInfo playerInfo;
        playerInfo.name = data["name"].toString().toStdString();
        playerInfo.score = data["score"].toInt();
        playerInfo.character = static_cast<CharacterType>(data["character"].toInt());
        playerInfo.isAlive = data["isAlive"].toBool();
        
        if (isServer && sender) {
            QString playerName = QString::fromStdString(playerInfo.name);
            clientNames[sender] = playerName;
            emit playerConnected(playerName);
            // 转发给其他客户端
            broadcastMessage(message, sender);
        }
        
        emit playerInfoReceived(playerInfo);
    }
    else if (type == "gameState") {
        if (isServer && sender) {
            broadcastMessage(message, sender);
        }
        emit gameStateReceived(data);
    }
    else if (type == "scoreUpdate") {
        QString playerName = clientNames.value(sender, "Unknown");
        int score = data["score"].toInt();
        
        if (isServer && sender) {
            broadcastMessage(message, sender);
        }
        
        emit scoreUpdateReceived(playerName, score);
    }
    else if (type == "playerPosition") {
        QString playerName = clientNames.value(sender, "Unknown");
        QJsonArray bodyArray = data["body"].toArray();
        
        std::deque<Point> snakeBody;
        for (const auto& value : bodyArray) {
            QJsonObject pointObj = value.toObject();
            snakeBody.push_back(Point(pointObj["x"].toInt(), pointObj["y"].toInt()));
        }
        
        if (isServer && sender) {
            broadcastMessage(message, sender);
        }
        
        emit playerPositionReceived(playerName, snakeBody);
    }
    else if (type == "characterSelectionStart") {
        QString roomId = data["roomId"].toString();
        if (isServer && sender) {
            broadcastMessage(message, sender);
        }
        emit characterSelectionStarted(roomId);
    }
    else if (type == "characterSelection") {
        QString roomId = data["roomId"].toString();
        QString playerName = data["playerName"].toString();
        int character = data["character"].toInt();
        
        if (isServer && sender) {
            broadcastMessage(message, sender);
        }
        
        emit characterSelectionReceived(roomId, playerName, character);
    }
    else if (type == "playerJoined") {
        QString roomId = data["roomId"].toString();
        QString playerName = data["playerName"].toString();
        emit playerJoined(roomId, playerName);
    }
    else if (type == "playerReady") {
        QString roomId = message["roomId"].toString();
        QString playerName = message["playerName"].toString();
        bool ready = message["ready"].toBool();
        
        if (isServer && sender) {
            // 服务器转发给其他客户端
            broadcastMessage(message, sender);
        }
        
        emit playerReadyReceived(roomId, playerName, ready);
    }
    else if (type == "gameCountdown") {
        QString roomId = message["roomId"].toString();
        int countdown = message["countdown"].toInt();
        
        if (isServer && sender) {
            // 服务器转发给其他客户端
            broadcastMessage(message, sender);
        }
        
        emit gameCountdownReceived(roomId, countdown);
    }
    // 忽略心跳消息
}

void NetworkManager::broadcastMessage(const QJsonObject& message, QTcpSocket* excludeSocket)
{
    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";
    
    for (auto client : clients) {
        if (client != excludeSocket && client->state() == QAbstractSocket::ConnectedState) {
            client->write(data);
        }
    }
}

QJsonObject NetworkManager::createMessage(const QString& type, const QJsonObject& data)
{
    QJsonObject message;
    message["type"] = type;
    message["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    if (!data.isEmpty()) {
        message["data"] = data;
    }
    return message;
}

// 启动房间发现
void NetworkManager::startRoomDiscovery(quint16 port)
{
    // 如果UDP socket已经绑定，先解绑
    if (udpSocket->state() == QAbstractSocket::BoundState) {
        udpSocket->close();
    }
    
    // 绑定到广播接收端口
    if (udpSocket->bind(QHostAddress::Any, 45454, QUdpSocket::ShareAddress)) {
        qDebug() << "UDP socket bound to port 45454 for room discovery";
    } else {
        qDebug() << "Failed to bind UDP socket for room discovery:" << udpSocket->errorString();
        // 尝试重新创建UDP socket
        udpSocket->deleteLater();
        udpSocket = new QUdpSocket(this);
        connect(udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::processRoomDiscovery);
        if (udpSocket->bind(QHostAddress::Any, 45454, QUdpSocket::ShareAddress)) {
            qDebug() << "UDP socket recreated and bound successfully";
        } else {
            qDebug() << "Failed to recreate UDP socket:" << udpSocket->errorString();
        }
    }
}

// 广播房间信息
void NetworkManager::broadcastRoomInfo()
{
    if (!server || !server->isListening()) {
        qDebug() << "Server not running, cannot broadcast room info";
        return;
    }
    
    // 获取房间信息（需要从MultiPlayerGameManager获取）
    QJsonObject roomInfo;
    roomInfo["type"] = "roomInfo";
    roomInfo["port"] = server->serverPort();
    // 获取本机实际IP地址
    QString localIp;
    const QList<QHostAddress>& addresses = QNetworkInterface::allAddresses();
    for (const QHostAddress& addr : addresses) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && addr != QHostAddress::LocalHost) {
            localIp = addr.toString();
            break;
        }
    }
    if (localIp.isEmpty()) {
        localIp = QHostAddress(QHostAddress::LocalHost).toString();
    }
    roomInfo["host"] = localIp;
    roomInfo["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    
    // 添加房间详细信息
    emit requestRoomInfo(roomInfo);

    QJsonDocument doc(roomInfo);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    // 确保UDP socket可用于广播
    if (!udpSocket) {
        qDebug() << "UDP socket is null, cannot broadcast";
        return;
    }
    
    // 如果UDP socket未绑定，尝试绑定
    if (udpSocket->state() != QAbstractSocket::BoundState) {
        if (!udpSocket->bind()) {
            qDebug() << "Failed to bind UDP socket for broadcasting:" << udpSocket->errorString();
            return;
        }
    }
    
    // 发送广播
    qint64 bytesWritten = udpSocket->writeDatagram(data, QHostAddress::Broadcast, 45454);
    if (bytesWritten == -1) {
        qDebug() << "Failed to send broadcast:" << udpSocket->errorString();
    } else {
        qDebug() << "Room info broadcasted on port" << server->serverPort() << "(" << bytesWritten << "bytes)";
    }
}

// 处理房间发现
void NetworkManager::processRoomDiscovery()
{
    QList<QHostAddress> localAddresses = QNetworkInterface::allAddresses();
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(datagram, &error);

        if (error.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject message = doc.object();
            if (message["type"].toString() == "roomInfo") {
                QString host = message["host"].toString();
                bool isLocal = false;
                for (const QHostAddress& addr : localAddresses) {
                    if (addr.toString() == host) {
                        isLocal = true;
                        break;
                    }
                }
                if (isLocal) continue; // 忽略本机广播
                emit roomDiscovered(host, message["port"].toInt());
                qDebug() << "Discovered room:" << host << ":" << message["port"].toInt();
            }
        }
    }
}

bool NetworkManager::startServerAuto(quint16& actualPort, quint16 basePort, int maxTries) {
    for (int i = 0; i < maxTries; ++i) {
        quint16 tryPort = basePort + i;
        if (startServer(tryPort)) {
            actualPort = tryPort;
            return true;
        }
    }
    actualPort = 0;
    return false;
}

quint16 NetworkManager::getServerPort() const {
    if (server && server->isListening()) {
        return server->serverPort();
    }
    return 0;
}