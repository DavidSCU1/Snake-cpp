#include "networkmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostAddress>
#include <QDebug>
#include <deque>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , server(nullptr)
    , clientSocket(nullptr)
    , heartbeatTimer(new QTimer(this))
    , isServer(false)
{
    heartbeatTimer->setInterval(5000); // 5秒心跳
    connect(heartbeatTimer, &QTimer::timeout, this, &NetworkManager::sendHeartbeat);
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
        qDebug() << "Server started on port" << port;
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

void NetworkManager::sendPlayerPosition(const std::deque<Point>& snakeBody)
{
    QJsonArray bodyArray;
    for (const auto& point : snakeBody) {
        QJsonObject pointObj;
        pointObj["x"] = point.x;
        pointObj["y"] = point.y;
        bodyArray.append(pointObj);
    }
    
    QJsonObject data;
    data["body"] = bodyArray;
    
    QJsonObject message = createMessage("playerPosition", data);
    
    if (isServer) {
        broadcastMessage(message);
    } else if (clientSocket) {
        QJsonDocument doc(message);
        clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
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

void NetworkManager::onNewConnection()
{
    while (server->hasPendingConnections()) {
        QTcpSocket* client = server->nextPendingConnection();
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
}

void NetworkManager::onClientDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    if (isServer) {
        clients.removeAll(socket);
        clientNames.remove(socket);
        qDebug() << "Client disconnected";
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
    QStringList messages = QString::fromUtf8(data).split('\n', Qt::SkipEmptyParts);
    
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
        qDebug() << "Socket error:" << socket->errorString();
    }
}

void NetworkManager::sendHeartbeat()
{
    QJsonObject message = createMessage("heartbeat");
    
    if (isServer) {
        broadcastMessage(message);
    }
}

void NetworkManager::sendRoomList(const QJsonArray& rooms)
{
    QJsonObject data;
    data["rooms"] = rooms;
    
    QJsonObject message = createMessage("roomList", data);
    broadcastMessage(message);
}

void NetworkManager::requestRoomList()
{
    QJsonObject message = createMessage("requestRoomList");
    
    if (clientSocket) {
        QJsonDocument doc(message);
        clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::processMessage(const QJsonObject& message, QTcpSocket* sender)
{
    QString type = message["type"].toString();
    QJsonObject data = message["data"].toObject();
    
    if (type == "playerInfo") {
        emit playerInfoReceived(data, sender);
    } else if (type == "gameState") {
        emit gameStateReceived(data);
    } else if (type == "scoreUpdate") {
        emit scoreUpdateReceived(data);
    } else if (type == "playerPosition") {
        emit playerPositionReceived(data);
    } else if (type == "heartbeat") {
        // 心跳消息，无需处理
    } else if (type == "roomList") {
        emit roomListReceived(data["rooms"].toArray());
    } else if (type == "requestRoomList") {
        if (isServer) {
            emit requestRoomListReceived(sender);
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

void NetworkManager::broadcastMessage(const QJsonObject& message, QTcpSocket* excludeSocket)
{
    if (!isServer) return;
    
    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";
    
    for (QTcpSocket* client : clients) {
        if (client != excludeSocket && client->state() == QAbstractSocket::ConnectedState) {
            client->write(data);
        }
    }
}