#ifndef HOTSPOTNETWORKMANAGER_H
#define HOTSPOTNETWORKMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkInterface>
#include <QHostAddress>
#include <deque>
#include "gamestate.h"

/**
 * 热点共享网络管理器
 * 专为热点网络环境优化的多人联机解决方案
 * 特点：
 * 1. 自动发现热点网络内的主机
 * 2. 简化的连接建立流程
 * 3. 优化的数据同步协议
 * 4. 更好的网络稳定性
 */
class HotspotNetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit HotspotNetworkManager(QObject *parent = nullptr);
    ~HotspotNetworkManager();
    
    // 热点主机功能
    bool startHotspotHost(const QString& roomName, int maxPlayers = 4);
    void stopHotspotHost();
    bool isHosting() const;
    
    // 热点客户端功能
    void startHostDiscovery();
    void stopHostDiscovery();
    bool connectToHost(const QString& hostAddress);
    void disconnectFromHost();
    bool isConnectedToHost() const;
    
    // 游戏数据同步
    void sendPlayerData(const QString& playerName, const QJsonObject& playerData);
    void sendGameState(const QJsonObject& gameState);
    void sendChatMessage(const QString& playerName, const QString& message);
    void sendMessage(const QString& type, const QJsonObject& data = QJsonObject());
    void broadcastToClients(const QJsonObject& message);
    
    // 房间信息
    QString getRoomName() const { return currentRoomName; }
    int getConnectedPlayersCount() const;
    QStringList getConnectedPlayerNames() const;
    
    // 网络状态
    QString getLocalIPAddress() const;
    bool isInHotspotNetwork() const;
    
signals:
    // 主机相关信号
    void hostStarted(const QString& roomName, const QString& ipAddress);
    void hostStopped();
    void playerConnectedToHost(const QString& playerName);
    void playerDisconnectedFromHost(const QString& playerName);
    
    // 客户端相关信号
    void hostDiscovered(const QString& hostAddress, const QString& roomName, int playerCount, int maxPlayers);
    void connectedToHost(const QString& hostAddress);
    void disconnectedFromHost();
    
    // 数据接收信号
    void playerDataReceived(const QString& playerName, const QJsonObject& playerData);
    void gameStateReceived(const QJsonObject& gameState);
    void chatMessageReceived(const QString& playerName, const QString& message);
    
    // 错误信号
    void networkError(const QString& errorMessage);
    
private slots:
    void onNewClientConnection();
    void onClientConnected();
    void onClientDisconnected();
    void onDataReceived();
    void onSocketError(QAbstractSocket::SocketError error);
    void onDiscoveryTimeout();
    void onHeartbeatTimeout();
    void processHostDiscovery();
    void broadcastHostInfo();
    void onUdpDataReceived();
    
private:
    void processMessage(const QJsonObject& message, QTcpSocket* sender = nullptr);
    QJsonObject createMessage(const QString& type, const QJsonObject& data = QJsonObject());
    void setupHeartbeat();
    void cleanupDisconnectedClients();
    QString detectHotspotNetwork();
    bool isValidHotspotIP(const QString& ipAddress) const;
    
    // 网络组件
    QTcpServer* tcpServer;
    QTcpSocket* tcpClient;
    QUdpSocket* udpSocket;
    
    // 定时器
    QTimer* discoveryTimer;
    QTimer* heartbeatTimer;
    QTimer* broadcastTimer;
    
    // 连接管理
    QList<QTcpSocket*> connectedClients;
    QMap<QTcpSocket*, QString> clientPlayerNames;
    QMap<QString, QTcpSocket*> playerSockets;
    
    // 房间状态
    QString currentRoomName;
    QString hostAddress;
    int maxPlayers;
    bool isHost;
    
    // 网络配置
    static const quint16 DEFAULT_PORT = 23456;
    static const quint16 DISCOVERY_PORT = 23457;
    static const int HEARTBEAT_INTERVAL = 3000;  // 3秒心跳
    static const int DISCOVERY_INTERVAL = 2000;  // 2秒发现间隔
    static const int BROADCAST_INTERVAL = 1000;  // 1秒广播间隔
};

#endif // HOTSPOTNETWORKMANAGER_H