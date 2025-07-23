#include "hotspotnetworkmanager.h"
#include <QNetworkInterface>
#include <QHostInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <random>
#include <QRandomGenerator>

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
    qDebug() << "Starting hotspot host - Room:" << roomName << "Max players:" << maxPlayers;
    
    if (isHosting()) {
        qWarning() << "Already hosting a room";
        return false;
    }
    
    // 检查是否在热点网络中
    if (!isInHotspotNetwork()) {
        QString error = "Not connected to a hotspot network";
        qWarning() << error;
        emit networkError(error);
        return false;
    }
    
    qDebug() << "Hotspot network check passed, proceeding with host setup";
    
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
    
    // 立即广播一次房间信息，然后开始定期广播
    QTimer::singleShot(500, this, &HotspotNetworkManager::broadcastHostInfo); // 延迟500ms确保初始化完成
    broadcastTimer->start(BROADCAST_INTERVAL);
    
    QString localIP = getLocalIPAddress();
    emit hostStarted(roomName, localIP);
    
    qDebug() << "Hotspot host started:" << roomName << "on" << localIP << ":" << DEFAULT_PORT;
    qDebug() << "Host will start broadcasting in 500ms to ensure full initialization";
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
    qDebug() << "Starting host discovery...";
    
    if (!isInHotspotNetwork()) {
        QString error = "Not connected to a hotspot network";
        qWarning() << error;
        emit networkError(error);
        return;
    }
    
    qDebug() << "Hotspot network check passed for discovery";
    
    // 创建或重用UDP套接字用于发现
    if (!udpSocket) {
        qDebug() << "Creating UDP socket for discovery";
        udpSocket = new QUdpSocket(this);
        
        // 尝试绑定到发现端口，如果失败则尝试其他端口
        bool bindSuccess = false;
        QList<quint16> portsToTry = {DISCOVERY_PORT, DISCOVERY_PORT + 1, DISCOVERY_PORT + 2, 0}; // 0表示系统分配
        
        for (quint16 port : portsToTry) {
            if (udpSocket->bind(QHostAddress::Any, port)) {
                qDebug() << "UDP socket bound to port" << udpSocket->localPort();
                bindSuccess = true;
                break;
            } else {
                qDebug() << "Failed to bind to port" << port << ":" << udpSocket->errorString();
            }
        }
        
        if (!bindSuccess) {
            qWarning() << "Failed to bind UDP socket to any port";
            emit networkError(QString("Failed to bind UDP socket: %1").arg(udpSocket->errorString()));
            udpSocket->deleteLater();
            udpSocket = nullptr;
            return;
        }
        
        connect(udpSocket, &QUdpSocket::readyRead, this, &HotspotNetworkManager::onUdpDataReceived);
    } else {
        qDebug() << "Using existing UDP socket for discovery, bound to port" << udpSocket->localPort();
    }
    
    // 开始定期发现主机
    discoveryTimer->start(DISCOVERY_INTERVAL);
    
    qDebug() << "Started host discovery with interval" << DISCOVERY_INTERVAL << "ms";
    
    // 延迟执行第一次发现，确保网络状态稳定
    QTimer::singleShot(200, this, &HotspotNetworkManager::processHostDiscovery);
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
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    qDebug() << "Searching for local IP address, found" << interfaces.size() << "network interfaces:";
    
    QString bestIP;
    QString fallbackIP;
    
    for (const QNetworkInterface& interface : interfaces) {
        if (!(interface.flags() & QNetworkInterface::IsUp) ||
            !(interface.flags() & QNetworkInterface::IsRunning) ||
            (interface.flags() & QNetworkInterface::IsLoopBack)) {
            continue;
        }
        
        qDebug() << "Checking interface:" << interface.name() << "Type:" << interface.type();
        
        QList<QNetworkAddressEntry> entries = interface.addressEntries();
        for (const QNetworkAddressEntry& entry : entries) {
            QHostAddress address = entry.ip();
            if (address.protocol() != QAbstractSocket::IPv4Protocol) {
                continue;
            }
            
            QString ipStr = address.toString();
            qDebug() << "  Checking address:" << ipStr 
                     << "Interface:" << interface.name()
                     << "ValidHotspot:" << isValidHotspotIP(ipStr);
            
            // 优先选择热点IP
            if (isValidHotspotIP(ipStr)) {
                // 优先级：Android热点 > iOS热点 > Windows热点 > 其他
                if (ipStr.startsWith("192.168.43.") || ipStr.startsWith("172.20.")) {
                    qDebug() << "Found high-priority hotspot IP:" << ipStr;
                    return ipStr;
                } else if (bestIP.isEmpty()) {
                    bestIP = ipStr;
                }
            } else if (fallbackIP.isEmpty() && !address.isLoopback()) {
                // 备选IP（非回环的IPv4地址）
                fallbackIP = ipStr;
            }
        }
    }
    
    if (!bestIP.isEmpty()) {
        qDebug() << "Selected best hotspot IP:" << bestIP;
        return bestIP;
    }
    
    if (!fallbackIP.isEmpty()) {
        qWarning() << "No hotspot IP found, using fallback IP:" << fallbackIP;
        return fallbackIP;
    }
    
    qWarning() << "No valid IP found, returning 127.0.0.1";
    return "127.0.0.1";
}

bool HotspotNetworkManager::isInHotspotNetwork() const
{
    QString localIP = getLocalIPAddress();
    
    // 放宽检测条件：只要不是回环地址就认为可能在网络中
    bool hasValidIP = localIP != "127.0.0.1" && !localIP.isEmpty();
    bool isHotspotIP = isValidHotspotIP(localIP);
    
    // 如果有热点IP，直接返回true
    if (isHotspotIP) {
        qDebug() << "In hotspot network - IP:" << localIP << "(hotspot IP detected)";
        return true;
    }
    
    // 如果有有效的非回环IP，也认为可能在网络中（放宽条件）
    if (hasValidIP) {
        qDebug() << "Possibly in network - IP:" << localIP << "(non-loopback IP, allowing connection)";
        return true;
    }
    
    qDebug() << "Not in hotspot network - Local IP:" << localIP 
             << "Valid IP:" << hasValidIP
             << "Hotspot IP:" << isHotspotIP;
    return false;
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
    if (!udpSocket) {
        qWarning() << "UDP socket is null in processHostDiscovery";
        return;
    }
    
    // 检查网络状态
    QString networkBase = getLocalIPAddress();
    if (networkBase.isEmpty() || networkBase == "127.0.0.1") {
        qWarning() << "Invalid network base for discovery:" << networkBase;
        return;
    }
    
    qDebug() << "Starting host discovery from IP:" << networkBase;
    
    // 发送发现请求
    QJsonObject discoveryRequest = createMessage("discover_hosts");
    QJsonDocument doc(discoveryRequest);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    qDebug() << "Discovery request data:" << data;
    
    // 首先尝试广播
    qint64 broadcastBytes = udpSocket->writeDatagram(data, QHostAddress::Broadcast, DISCOVERY_PORT);
    qDebug() << "Sent broadcast discovery request, bytes written:" << broadcastBytes;
    
    // 扫描多个可能的网络段
    QStringList networkPrefixes = {
        "192.168.43.",   // Android热点
        "192.168.137.",  // Windows热点
        "172.20.",       // iOS热点
        "10.0.0.",       // 通用热点
        "192.168.1.",    // 常见路由器
        "192.168.0.",    // 常见路由器
        "10.0.1.",       // 扩展热点段
        "172.16.",       // 私有网络段
    };
    
    bool foundMatchingSegment = false;
    
    // 首先扫描当前网络段
    for (const QString& prefix : networkPrefixes) {
        if (networkBase.startsWith(prefix)) {
            qDebug() << "Scanning current network segment:" << prefix << "*";
            int sentCount = 0;
            for (int i = 1; i < 255; ++i) {
                QString targetIP = prefix + QString::number(i);
                if (targetIP != networkBase) { // 不发送给自己
                    qint64 bytes = udpSocket->writeDatagram(data, QHostAddress(targetIP), DISCOVERY_PORT);
                    if (bytes > 0) sentCount++;
                }
            }
            qDebug() << "Sent discovery requests to" << sentCount << "addresses in current segment" << prefix << "*";
            foundMatchingSegment = true;
            break;
        }
    }
    
    // 如果当前网络段不匹配，扫描常见的热点网络段
    if (!foundMatchingSegment) {
        qDebug() << "Current network segment not recognized, scanning common hotspot segments";
        QStringList commonHotspotPrefixes = {"192.168.43.", "172.20.", "192.168.137."};
        
        for (const QString& prefix : commonHotspotPrefixes) {
            qDebug() << "Scanning hotspot segment:" << prefix << "*";
            int sentCount = 0;
            for (int i = 1; i < 255; ++i) {
                QString targetIP = prefix + QString::number(i);
                qint64 bytes = udpSocket->writeDatagram(data, QHostAddress(targetIP), DISCOVERY_PORT);
                if (bytes > 0) sentCount++;
            }
            qDebug() << "Sent discovery requests to" << sentCount << "addresses in hotspot segment" << prefix << "*";
        }
    }
}

void HotspotNetworkManager::broadcastHostInfo()
{
    if (!isHosting() || !udpSocket) {
        qDebug() << "Cannot broadcast: isHosting=" << isHosting() << ", udpSocket=" << (udpSocket != nullptr);
        return;
    }
    
    QString localIP = getLocalIPAddress();
    if (localIP == "127.0.0.1" || localIP.isEmpty()) {
        qWarning() << "Invalid local IP for broadcasting:" << localIP << ", retrying IP detection...";
        // 重新检测IP地址
        QTimer::singleShot(1000, this, &HotspotNetworkManager::broadcastHostInfo);
        return;
    }
    
    QJsonObject hostInfo = createMessage("host_info");
    hostInfo["room_name"] = currentRoomName;
    hostInfo["player_count"] = getConnectedPlayersCount();
    hostInfo["max_players"] = maxPlayers;
    hostInfo["host_address"] = localIP;
    
    QJsonDocument doc(hostInfo);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    // 多种方式广播主机信息
    int successfulBroadcasts = 0;
    
    // 1. 标准广播
    qint64 broadcastBytes = udpSocket->writeDatagram(data, QHostAddress::Broadcast, DISCOVERY_PORT);
    if (broadcastBytes > 0) {
        successfulBroadcasts++;
        qDebug() << "Standard broadcast successful, bytes:" << broadcastBytes;
    }
    
    // 2. 向常见热点网络段广播
    QStringList hotspotPrefixes = {"192.168.43.", "172.20.", "192.168.137.", "192.168.1.", "192.168.0."};
    for (const QString& prefix : hotspotPrefixes) {
        if (localIP.startsWith(prefix)) {
            // 向同网段的广播地址发送
            QString broadcastAddr = prefix + "255";
            qint64 bytes = udpSocket->writeDatagram(data, QHostAddress(broadcastAddr), DISCOVERY_PORT);
            if (bytes > 0) {
                successfulBroadcasts++;
                qDebug() << "Segment broadcast to" << broadcastAddr << "successful, bytes:" << bytes;
            }
            break;
        }
    }
    
    qDebug() << "Broadcasting host info:" << currentRoomName << "on IP:" << localIP 
             << "successful broadcasts:" << successfulBroadcasts << "data size:" << data.size();
    
    if (successfulBroadcasts == 0) {
        qWarning() << "All broadcast attempts failed, will retry in next interval";
    }
}

void HotspotNetworkManager::onUdpDataReceived()
{
    if (!udpSocket) {
        qWarning() << "UDP socket is null in onUdpDataReceived";
        return;
    }
    
    qDebug() << "UDP data received, pending datagrams:" << udpSocket->hasPendingDatagrams();
    
    int processedCount = 0;
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        
        qint64 bytesRead = udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        processedCount++;
        
        if (bytesRead <= 0) {
            qWarning() << "Failed to read UDP datagram, bytes read:" << bytesRead;
            continue;
        }
        
        qDebug() << "Received UDP datagram" << processedCount << "from" << sender.toString() << ":" << senderPort 
                 << "size:" << bytesRead;
        
        // 忽略来自自己的消息
        QString localIP = getLocalIPAddress();
        if (sender.toString() == localIP) {
            qDebug() << "Ignoring message from self:" << sender.toString();
            continue;
        }
        
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(datagram, &error);
        
        if (error.error != QJsonParseError::NoError) {
            qWarning() << "Failed to parse UDP JSON:" << error.errorString() 
                       << "at offset:" << error.offset << "data:" << datagram;
            continue;
        }
        
        if (!doc.isObject()) {
            qWarning() << "UDP message is not a JSON object:" << datagram;
            continue;
        }
        
        QJsonObject message = doc.object();
        QString type = message["type"].toString();
        
        if (type.isEmpty()) {
            qWarning() << "UDP message missing type field:" << datagram;
            continue;
        }
        
        qDebug() << "Processing UDP message type:" << type << "from:" << sender.toString() 
                 << "isHosting:" << isHosting();
        
        if (type == "host_info" && !isHosting()) {
            QString hostAddr = message["host_address"].toString();
            QString roomName = message["room_name"].toString();
            int playerCount = message["player_count"].toInt();
            int maxPlayers = message["max_players"].toInt();
            
            // 验证主机信息的有效性
            if (hostAddr.isEmpty() || roomName.isEmpty() || maxPlayers <= 0) {
                qWarning() << "Invalid host info received:" << "addr=" << hostAddr 
                           << "room=" << roomName << "maxPlayers=" << maxPlayers;
                continue;
            }
            
            qDebug() << "Discovered valid host:" << roomName << "at" << hostAddr 
                     << "players:" << playerCount << "/" << maxPlayers;
            emit hostDiscovered(hostAddr, roomName, playerCount, maxPlayers);
            
        } else if (type == "discover_hosts" && isHosting()) {
            qDebug() << "Received discovery request from" << sender.toString() 
                     << ", responding with host info";
            // 延迟响应，避免网络拥塞
            QTimer::singleShot(100 + QRandomGenerator::global()->bounded(200), this, &HotspotNetworkManager::broadcastHostInfo);
            
        } else {
            qDebug() << "Ignoring UDP message type:" << type << "(isHosting:" << isHosting() << ")";
        }
    }
    
    qDebug() << "Processed" << processedCount << "UDP datagrams";
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
    bool isValid = ipAddress.startsWith("192.168.43.") ||  // Android热点
                   ipAddress.startsWith("192.168.137.") || // Windows热点
                   ipAddress.startsWith("172.20.") ||      // iOS热点
                   ipAddress.startsWith("10.0.0.") ||      // 其他热点
                   ipAddress.startsWith("192.168.1.") ||   // 常见路由器热点
                   ipAddress.startsWith("192.168.0.") ||   // 常见路由器热点
                   ipAddress.startsWith("10.0.1.") ||      // 扩展热点段
                   ipAddress.startsWith("172.16.") ||      // 私有网络段
                   ipAddress.startsWith("192.168.");       // 通用192.168段
    
    qDebug() << "Checking IP validity:" << ipAddress << "-> Valid:" << isValid;
    return isValid;
}