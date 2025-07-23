#include "hotspotnetworkmanager.h"
#include <QNetworkInterface>
#include <QHostInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

HotspotNetworkManager::HotspotNetworkManager(QObject *parent)
    : QObject(parent)
    , tcpServer(nullptr)
    , tcpClient(nullptr)
    , udpSocket(nullptr)
    , discoveryTimer(new QTimer(this))
    , heartbeatTimer(new QTimer(this))
    , broadcastTimer(new QTimer(this))
    , maxPlayers(4)
    , isHost(false)
{
    // 设置定时器
    discoveryTimer->setSingleShot(false);
    heartbeatTimer->setSingleShot(false);
    broadcastTimer->setSingleShot(false);
    
    // 连接定时器信号
    connect(discoveryTimer, &QTimer::timeout, this, &HotspotNetworkManager::processHostDiscovery);
    connect(heartbeatTimer, &QTimer::timeout, this, &HotspotNetworkManager::onHeartbeatTimeout);
    connect(broadcastTimer, &QTimer::timeout, this, &HotspotNetworkManager::broadcastHostInfo);
}

HotspotNetworkManager::~HotspotNetworkManager()
{
    stopHotspotHost();
    disconnectFromHost();
    stopHostDiscovery();
}

bool HotspotNetworkManager::startHotspotHost(const QString& roomName, int maxPlayers)
{
    if (isHosting()) {
        qWarning() << "Already hosting a room";
        return false;
    }
    
    // 检查是否在热点网络中
    if (!isInHotspotNetwork()) {
        emit networkError("Not connected to a hotspot network");
        return false;
    }
    
    // 创建TCP服务器
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &HotspotNetworkManager::onNewClientConnection);
    
    // 启动服务器
    if (!tcpServer->listen(QHostAddress::Any, DEFAULT_PORT)) {
        qWarning() << "Failed to start TCP server:" << tcpServer->errorString();
        delete tcpServer;
        tcpServer = nullptr;
        return false;
    }
    
    // 创建UDP套接字用于广播
    udpSocket = new QUdpSocket(this);
    if (!udpSocket->bind(QHostAddress::Any, DISCOVERY_PORT)) {
        qWarning() << "Failed to bind UDP socket:" << udpSocket->errorString();
    }
    connect(udpSocket, &QUdpSocket::readyRead, this, &HotspotNetworkManager::onUdpDataReceived);
    
    // 设置房间信息
    currentRoomName = roomName;
    this->maxPlayers = maxPlayers;
    isHost = true;
    
    // 开始广播房间信息
    broadcastTimer->start(BROADCAST_INTERVAL);
    
    QString localIP = getLocalIPAddress();
    emit hostStarted(roomName, localIP);
    
    qDebug() << "Hotspot host started:" << roomName << "on" << localIP << ":" << DEFAULT_PORT;
    return true;
}

void HotspotNetworkManager::stopHotspotHost()
{
    if (!isHosting()) {
        return;
    }
    
    // 停止定时器
    broadcastTimer->stop();
    heartbeatTimer->stop();
    
    // 断开所有客户端
    for (QTcpSocket* client : connectedClients) {
        client->disconnectFromHost();
        client->deleteLater();
    }
    connectedClients.clear();
    clientPlayerNames.clear();
    playerSockets.clear();
    
    // 关闭服务器
    if (tcpServer) {
        tcpServer->close();
        tcpServer->deleteLater();
        tcpServer = nullptr;
    }
    
    // 关闭UDP套接字
    if (udpSocket) {
        udpSocket->close();
        udpSocket->deleteLater();
        udpSocket = nullptr;
    }
    
    isHost = false;
    currentRoomName.clear();
    
    emit hostStopped();
    qDebug() << "Hotspot host stopped";
}

bool HotspotNetworkManager::isHosting() const
{
    return isHost && tcpServer && tcpServer->isListening();
}

void HotspotNetworkManager::startHostDiscovery()
{
    if (!isInHotspotNetwork()) {
        emit networkError("Not connected to a hotspot network");
        return;
    }
    
    // 创建UDP套接字用于发现
    if (!udpSocket) {
        udpSocket = new QUdpSocket(this);
        if (!udpSocket->bind(QHostAddress::Any, DISCOVERY_PORT)) {
            qWarning() << "Failed to bind UDP socket for discovery:" << udpSocket->errorString();
        }
        connect(udpSocket, &QUdpSocket::readyRead, this, &HotspotNetworkManager::onUdpDataReceived);
    }
    
    // 开始定期发现主机
    discoveryTimer->start(DISCOVERY_INTERVAL);
    
    qDebug() << "Started host discovery";
}

void HotspotNetworkManager::stopHostDiscovery()
{
    discoveryTimer->stop();
    qDebug() << "Stopped host discovery";
}

bool HotspotNetworkManager::connectToHost(const QString& hostAddress)
{
    if (isConnectedToHost()) {
        qWarning() << "Already connected to a host";
        return false;
    }
    
    // 创建TCP客户端
    tcpClient = new QTcpSocket(this);
    connect(tcpClient, &QTcpSocket::connected, this, &HotspotNetworkManager::onClientConnected);
    connect(tcpClient, &QTcpSocket::disconnected, this, &HotspotNetworkManager::onClientDisconnected);
    connect(tcpClient, &QTcpSocket::readyRead, this, &HotspotNetworkManager::onDataReceived);
    connect(tcpClient, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &HotspotNetworkManager::onSocketError);
    
    // 连接到主机
    tcpClient->connectToHost(hostAddress, DEFAULT_PORT);
    this->hostAddress = hostAddress;
    
    qDebug() << "Attempting to connect to host:" << hostAddress;
    return true;
}

void HotspotNetworkManager::disconnectFromHost()
{
    if (tcpClient) {
        tcpClient->disconnectFromHost();
        tcpClient->deleteLater();
        tcpClient = nullptr;
    }
    
    heartbeatTimer->stop();
    hostAddress.clear();
    
    emit disconnectedFromHost();
    qDebug() << "Disconnected from host";
}

bool HotspotNetworkManager::isConnectedToHost() const
{
    return tcpClient && tcpClient->state() == QAbstractSocket::ConnectedState;
}

void HotspotNetworkManager::sendPlayerData(const QString& playerName, const QJsonObject& playerData)
{
    QJsonObject message = createMessage("player_data");
    message["player_name"] = playerName;
    message["data"] = playerData;
    
    if (isHosting()) {
        broadcastToClients(message);
    } else if (isConnectedToHost()) {
        QJsonDocument doc(message);
        tcpClient->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void HotspotNetworkManager::sendGameState(const QJsonObject& gameState)
{
    QJsonObject message = createMessage("game_state");
    message["state"] = gameState;
    
    if (isHosting()) {
        broadcastToClients(message);
    }
}

void HotspotNetworkManager::sendChatMessage(const QString& playerName, const QString& message)
{
    QJsonObject msg = createMessage("chat_message");
    msg["player_name"] = playerName;
    msg["message"] = message;
    
    if (isHosting()) {
        broadcastToClients(msg);
    } else if (isConnectedToHost()) {
        QJsonDocument doc(msg);
        tcpClient->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void HotspotNetworkManager::broadcastToClients(const QJsonObject& message)
{
    if (!isHosting()) {
        return;
    }
    
    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";
    
    for (QTcpSocket* client : connectedClients) {
        if (client->state() == QAbstractSocket::ConnectedState) {
            client->write(data);
        }
    }
}

int HotspotNetworkManager::getConnectedPlayersCount() const
{
    return connectedClients.size() + (isHosting() ? 1 : 0);
}

QStringList HotspotNetworkManager::getConnectedPlayerNames() const
{
    QStringList names;
    for (auto it = clientPlayerNames.begin(); it != clientPlayerNames.end(); ++it) {
        names.append(it.value());
    }
    return names;
}

QString HotspotNetworkManager::getLocalIPAddress() const
{
    // 获取热点网络的IP地址
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    
    for (const QHostAddress& address : addresses) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol &&
            !address.isLoopback() &&
            isValidHotspotIP(address.toString())) {
            return address.toString();
        }
    }
    
    return "127.0.0.1";
}

bool HotspotNetworkManager::isInHotspotNetwork() const
{
    QString localIP = getLocalIPAddress();
    return isValidHotspotIP(localIP) && localIP != "127.0.0.1";
}

void HotspotNetworkManager::onNewClientConnection()
{
    while (tcpServer->hasPendingConnections()) {
        QTcpSocket* client = tcpServer->nextPendingConnection();
        
        if (connectedClients.size() >= maxPlayers - 1) {
            // 房间已满
            QJsonObject message = createMessage("room_full");
            QJsonDocument doc(message);
            client->write(doc.toJson(QJsonDocument::Compact) + "\n");
            client->disconnectFromHost();
            client->deleteLater();
            continue;
        }
        
        connectedClients.append(client);
        
        connect(client, &QTcpSocket::readyRead, this, &HotspotNetworkManager::onDataReceived);
        connect(client, &QTcpSocket::disconnected, this, &HotspotNetworkManager::onClientDisconnected);
        connect(client, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
                this, &HotspotNetworkManager::onSocketError);
        
        qDebug() << "New client connected from:" << client->peerAddress().toString();
    }
}

void HotspotNetworkManager::onClientConnected()
{
    if (tcpClient) {
        setupHeartbeat();
        emit connectedToHost(hostAddress);
        qDebug() << "Connected to host:" << hostAddress;
    }
}

void HotspotNetworkManager::onClientDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    if (socket == tcpClient) {
        // 客户端断开连接
        tcpClient = nullptr;
        heartbeatTimer->stop();
        emit disconnectedFromHost();
        qDebug() << "Disconnected from host";
    } else {
        // 服务器端客户端断开连接
        QString playerName = clientPlayerNames.value(socket);
        connectedClients.removeAll(socket);
        clientPlayerNames.remove(socket);
        playerSockets.remove(playerName);
        
        if (!playerName.isEmpty()) {
            emit playerDisconnectedFromHost(playerName);
            qDebug() << "Player disconnected:" << playerName;
        }
        
        socket->deleteLater();
    }
}

void HotspotNetworkManager::onDataReceived()
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

void HotspotNetworkManager::onSocketError(QAbstractSocket::SocketError error)
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        emit networkError(QString("Socket error: %1").arg(socket->errorString()));
        qWarning() << "Socket error:" << socket->errorString();
    }
}

void HotspotNetworkManager::onDiscoveryTimeout()
{
    // 处理发现超时
}

void HotspotNetworkManager::onHeartbeatTimeout()
{
    if (isConnectedToHost()) {
        QJsonObject heartbeat = createMessage("heartbeat");
        QJsonDocument doc(heartbeat);
        tcpClient->write(doc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void HotspotNetworkManager::processHostDiscovery()
{
    if (!udpSocket) return;
    
    // 发送发现请求
    QJsonObject discoveryRequest = createMessage("discover_hosts");
    QJsonDocument doc(discoveryRequest);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    // 广播到热点网络
    QString networkBase = getLocalIPAddress();
    if (networkBase.isEmpty()) return;
    
    // 简化的网络扫描：只扫描常见的热点网络段
    QStringList networkPrefixes = {"192.168.43.", "192.168.137.", "10.0.0."};
    
    for (const QString& prefix : networkPrefixes) {
        if (networkBase.startsWith(prefix)) {
            for (int i = 1; i < 255; ++i) {
                QString targetIP = prefix + QString::number(i);
                udpSocket->writeDatagram(data, QHostAddress(targetIP), DISCOVERY_PORT);
            }
            break;
        }
    }
}

void HotspotNetworkManager::broadcastHostInfo()
{
    if (!isHosting() || !udpSocket) return;
    
    QJsonObject hostInfo = createMessage("host_info");
    hostInfo["room_name"] = currentRoomName;
    hostInfo["player_count"] = getConnectedPlayersCount();
    hostInfo["max_players"] = maxPlayers;
    hostInfo["host_address"] = getLocalIPAddress();
    
    QJsonDocument doc(hostInfo);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    // 广播主机信息
    udpSocket->writeDatagram(data, QHostAddress::Broadcast, DISCOVERY_PORT);
}

void HotspotNetworkManager::onUdpDataReceived()
{
    if (!udpSocket) return;
    
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
            QString type = message["type"].toString();
            
            if (type == "host_info" && !isHosting()) {
                QString hostAddr = message["host_address"].toString();
                QString roomName = message["room_name"].toString();
                int playerCount = message["player_count"].toInt();
                int maxPlayers = message["max_players"].toInt();
                emit hostDiscovered(hostAddr, roomName, playerCount, maxPlayers);
                qDebug() << "Discovered host:" << roomName << "at" << hostAddr;
            } else if (type == "discover_hosts" && isHosting()) {
                // 响应发现请求
                broadcastHostInfo();
            }
        }
    }
}

void HotspotNetworkManager::processMessage(const QJsonObject& message, QTcpSocket* sender)
{
    QString type = message["type"].toString();
    
    if (type == "player_join") {
        QString playerName = message["player_name"].toString();
        if (sender && isHosting()) {
            clientPlayerNames[sender] = playerName;
            playerSockets[playerName] = sender;
            emit playerConnectedToHost(playerName);
        }
    } else if (type == "player_data") {
        QString playerName = message["player_name"].toString();
        QJsonObject playerData = message["data"].toObject();
        emit playerDataReceived(playerName, playerData);
    } else if (type == "game_state") {
        QJsonObject gameState = message["state"].toObject();
        emit gameStateReceived(gameState);
    } else if (type == "chat_message") {
        QString playerName = message["player_name"].toString();
        QString chatMessage = message["message"].toString();
        emit chatMessageReceived(playerName, chatMessage);
    }
}

QJsonObject HotspotNetworkManager::createMessage(const QString& type, const QJsonObject& data)
{
    QJsonObject message;
    message["type"] = type;
    message["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    
    if (!data.isEmpty()) {
        for (auto it = data.begin(); it != data.end(); ++it) {
            message[it.key()] = it.value();
        }
    }
    
    return message;
}

void HotspotNetworkManager::setupHeartbeat()
{
    heartbeatTimer->start(HEARTBEAT_INTERVAL);
}

void HotspotNetworkManager::cleanupDisconnectedClients()
{
    for (int i = connectedClients.size() - 1; i >= 0; --i) {
        QTcpSocket* client = connectedClients[i];
        if (client->state() != QAbstractSocket::ConnectedState) {
            QString playerName = clientPlayerNames.value(client);
            connectedClients.removeAt(i);
            clientPlayerNames.remove(client);
            playerSockets.remove(playerName);
            client->deleteLater();
        }
    }
}

QString HotspotNetworkManager::detectHotspotNetwork()
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface& interface : interfaces) {
        if (interface.flags() & QNetworkInterface::IsUp &&
            interface.flags() & QNetworkInterface::IsRunning &&
            !(interface.flags() & QNetworkInterface::IsLoopBack)) {
            
            QList<QNetworkAddressEntry> entries = interface.addressEntries();
            for (const QNetworkAddressEntry& entry : entries) {
                QHostAddress address = entry.ip();
                if (address.protocol() == QAbstractSocket::IPv4Protocol &&
                    isValidHotspotIP(address.toString())) {
                    return address.toString();
                }
            }
        }
    }
    
    return QString();
}

bool HotspotNetworkManager::isValidHotspotIP(const QString& ipAddress) const
{
    // 检查是否为常见的热点网络IP段
    return ipAddress.startsWith("192.168.43.") ||  // Android热点
           ipAddress.startsWith("192.168.137.") || // Windows热点
           ipAddress.startsWith("172.20.") ||      // iOS热点
           ipAddress.startsWith("10.0.0.");        // 其他热点
}