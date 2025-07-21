#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <deque>
#include "gamestate.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    
    // 服务器功能
    bool startServer(quint16 port = 12345);
    void stopServer();
    bool isServerRunning() const;
    
    // 客户端功能
    void connectToServer(const QString& hostAddress, quint16 port = 12345);
    void disconnectFromServer();
    bool isConnectedToServer() const;
    
    // 消息发送
    void sendPlayerInfo(const PlayerInfo& playerInfo);
    void sendGameState(const QJsonObject& gameState);
    void sendScoreUpdate(int score);
    void sendPlayerPosition(const std::deque<Point>& snakeBody);
    void broadcastMessage(const QJsonObject& message, QTcpSocket* excludeSocket = nullptr);
    
    // 获取连接的玩家数量
    int getConnectedPlayersCount() const;
    
    void sendRoomList(const QJsonArray& roomList); // 新增：服务器向客户端发送房间列表
    void requestRoomList(); // 新增：客户端请求房间列表
signals:
    void playerConnected(const QString& playerName);
    void playerDisconnected(const QString& playerName);
    void playerInfoReceived(const QJsonObject& data, QTcpSocket* sender = nullptr);
    void gameStateReceived(const QJsonObject& gameState);
    void scoreUpdateReceived(const QJsonObject& data);
    void playerPositionReceived(const QJsonObject& data);
    void connectionError(const QString& error);
    void requestRoomListReceived(QTcpSocket* requester); // 新增：收到房间列表请求信号
    void roomListReceived(const QJsonArray& rooms); // 新增：收到房间列表信号
    
private slots:
    void onNewConnection();
    void onClientConnected();
    void onClientDisconnected();
    void onDataReceived();
    void onSocketError(QAbstractSocket::SocketError error);
    void sendHeartbeat();
    
private:
    void processMessage(const QJsonObject& message, QTcpSocket* sender = nullptr);
    QJsonObject createMessage(const QString& type, const QJsonObject& data = QJsonObject());
    
    // 服务器相关
    QTcpServer* server;
    QList<QTcpSocket*> clients;
    QMap<QTcpSocket*, QString> clientNames;
    
    // 客户端相关
    QTcpSocket* clientSocket;
    
    // 心跳机制
    QTimer* heartbeatTimer;
    
    bool isServer;
};

#endif // NETWORKMANAGER_H