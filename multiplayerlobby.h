#ifndef MULTIPLAYERLOBBY_H
#define MULTIPLAYERLOBBY_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QSpinBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QTimer>
#include "multiplayergamemanager.h"
#include "gamewidget.h"

class MultiPlayerLobby : public QWidget
{
    Q_OBJECT

public:
    explicit MultiPlayerLobby(QWidget *parent = nullptr);
    ~MultiPlayerLobby();
    
    void setGameWidget(GameWidget* gameWidget);
    void refreshRoomList();
    
signals:
    void backToMenu();
    void gameStarted();
    
private slots:
    void onCreateRoomClicked();
    void onJoinRoomClicked();
    void onRefreshClicked();
    void onBackClicked();
    void onRoomSelectionChanged();
    void onPlayerNameChanged();
    
    // 多人游戏管理器信号槽
    void onRoomCreated(const QString& roomId, const GameRoom& room);
    void onPlayerJoinedRoom(const QString& roomId, const QString& playerName);
    void onPlayerLeftRoom(const QString& roomId, const QString& playerName);
    void onGameStarted(const QString& roomId);
    void onGameEnded(const QString& roomId, const QString& winner);
    void onRoomDestroyed(const QString& roomId);
    
private:
    void setupUI();
    void updateRoomInfo(const GameRoom& room);
    void clearRoomInfo();
    bool validatePlayerName() const;
    
    // UI 组件
    QVBoxLayout* mainLayout;
    QHBoxLayout* topLayout;
    QHBoxLayout* bottomLayout;
    
    // 左侧面板 - 房间列表
    QGroupBox* roomListGroup;
    QVBoxLayout* roomListLayout;
    QListWidget* roomListWidget;
    QPushButton* refreshButton;
    
    // 中间面板 - 房间信息
    QGroupBox* roomInfoGroup;
    QVBoxLayout* roomInfoLayout;
    QLabel* roomIdLabel;
    QLabel* hostNameLabel;
    QLabel* playerCountLabel;
    QLabel* gameStatusLabel;
    QListWidget* playersListWidget;
    
    // 右侧面板 - 操作
    QGroupBox* actionGroup;
    QVBoxLayout* actionLayout;
    
    // 玩家信息
    QLabel* playerNameLabel;
    QLineEdit* playerNameEdit;
    
    // 创建房间
    QLabel* maxPlayersLabel;
    QSpinBox* maxPlayersSpinBox;
    QPushButton* createRoomButton;
    
    // 加入房间
    QPushButton* joinRoomButton;
    
    // 底部按钮
    QPushButton* backButton;
    
    // 游戏相关
    GameWidget* gameWidget;
    MultiPlayerGameManager* multiPlayerManager;
    
    // 状态
    QString currentRoomId;
    QString playerName;
    bool isInRoom;
    
    // 定时器
    QTimer* refreshTimer;
};

#endif // MULTIPLAYERLOBBY_H