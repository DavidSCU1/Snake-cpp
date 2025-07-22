#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <deque>
#include <QUdpSocket> // 添加此行以包含 QUdpSocket 的定义
#include "gamestate.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    
    // 服务器功能
    bool startServer(quint16 port = 12345);
    // 自动检测可用端口并启动服务器，返回实际端口
    bool startServerAuto(quint16& actualPort, quint16 basePort = 12345, int maxTries = 20);
    // 获取当前监听端口
    quint16 getServerPort() const;
    void stopServer();
    bool isServerRunning() const;
    
    // 房间发现功能
    void startRoomDiscovery(quint16 port);
    void broadcastRoomInfo();
    void processRoomDiscovery();
    
    // 客户端功能
    void connectToServer(const QString& hostAddress, quint16 port = 12345);
    void disconnectFromServer();
    bool isConnectedToServer() const;
    
    // 消息发送
    void sendPlayerInfo(const PlayerInfo& playerInfo);
    void sendGameState(const QJsonObject& gameState);
    void sendScoreUpdate(int score);
    void sendPlayerPosition(const QString& playerName, const std::deque<Point>& snakeBody);
    void sendCharacterSelectionStart(); // 新增：发送角色选择开始消息
    void sendCharacterSelection(const QString& playerName, int character); // 新增：发送角色选择消息
    void sendMessage(const QByteArray& message); // 新增：发送自定义消息
    void broadcastMessage(const QJsonObject& message, QTcpSocket* excludeSocket = nullptr);
    
    // 获取连接的玩家数量
    int getConnectedPlayersCount() const;
    
    // 获取客户端socket
    QTcpSocket* getClientSocket() const;
    
    // 创建消息（公共方法）
    QJsonObject createMessage(const QString& type, const QJsonObject& data = QJsonObject());
    
signals:
    void playerConnected(const QString& playerName);
    void playerDisconnected(const QString& playerName);
    void playerInfoReceived(const PlayerInfo& playerInfo);
    void gameStateReceived(const QJsonObject& gameState);
    void scoreUpdateReceived(const QString& playerName, int score);
    void playerPositionReceived(const QString& playerName, const std::deque<Point>& snakeBody);
    void connectionError(const QString& error);
    void roomDiscovered(const QString& host, int port); // 修改为两个参数
    void requestRoomInfo(QJsonObject& roomInfo); // 请求房间信息的信号
    void characterSelectionStarted(const QString& roomId); // 新增：角色选择开始信号
    void characterSelectionReceived(const QString& roomId, const QString& playerName, int character); // 新增：角色选择信号
    void playerJoined(const QString& roomId, const QString& playerName); // 新增：玩家加入信号
    void playerReadyReceived(const QString& roomId, const QString& playerName, bool ready); // 新增：玩家准备状态信号
    void gameCountdownReceived(const QString& roomId, int countdown); // 新增：游戏倒计时信号
    
private slots:
    void onNewConnection();
    void onClientConnected();
    void onClientDisconnected();
    void onDataReceived();
    void onSocketError(QAbstractSocket::SocketError error);
    void sendHeartbeat();
    
private:
    void processMessage(const QJsonObject& message, QTcpSocket* sender = nullptr);
    QString pendingPlayerName; // 新增：用于保存待发送的玩家名
    
    // 服务器相关
    QTcpServer* server;
    QList<QTcpSocket*> clients;
    QMap<QTcpSocket*, QString> clientNames;
    
    // 客户端相关
    QTcpSocket* clientSocket;
    
    // 心跳机制
    QTimer* heartbeatTimer;
    
    // UDP相关
    QUdpSocket* udpSocket; // QUdpSocket 类型现在可以识别
    bool allowJoinMidGame;
    
    // 房间广播计时器
    QTimer* roomBroadcastTimer;
    
    bool isServer;
public:
    void setPendingPlayerName(const QString& name) { pendingPlayerName = name; }
};

#endif // NETWORKMANAGER_H