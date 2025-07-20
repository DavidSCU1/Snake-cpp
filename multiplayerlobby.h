#ifndef MULTIPLAYERLOBBY_H
#define MULTIPLAYERLOBBY_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QTimer>
#include <QMessageBox>
#include <QGroupBox>
#include "gamestate.h"
#include "multiplayergamemanager.h"

class GameWidget;

class MultiPlayerLobby : public QWidget
{
    Q_OBJECT

public:
    explicit MultiPlayerLobby(QWidget *parent = nullptr);
    ~MultiPlayerLobby();
    
    void setGameWidget(GameWidget* gameWidget);
    
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
    void refreshRoomList();
    
    // MultiPlayerGameManager 信号槽
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
    QHBoxLayout* contentLayout;
    
    // 左侧房间列表
    QWidget* roomListWidget;
    QVBoxLayout* roomListLayout;
    QLabel* roomListLabel;
    QListWidget* roomList;
    QPushButton* refreshButton;
    
    // 右侧房间信息和控制
    QWidget* roomInfoWidget;
    QVBoxLayout* roomInfoLayout;
    
    // 房间信息显示
    QGroupBox* roomInfoGroup;
    QVBoxLayout* roomInfoGroupLayout;
    QLabel* roomIdLabel;
    QLabel* roomNameLabel;
    QLabel* playerCountLabel;
    QLabel* roomStatusLabel;
    QListWidget* playerListWidget;
    
    // 玩家设置
    QGroupBox* playerSettingsGroup;
    QVBoxLayout* playerSettingsLayout;
    QLabel* playerNameLabel;
    QLineEdit* playerNameEdit;
    
    // 房间创建设置
    QGroupBox* createRoomGroup;
    QVBoxLayout* createRoomLayout;
    QLabel* maxPlayersLabel;
    QSpinBox* maxPlayersSpinBox;
    
    // 控制按钮
    QWidget* buttonWidget;
    QHBoxLayout* buttonLayout;
    QPushButton* createRoomButton;
    QPushButton* joinRoomButton;
    QPushButton* backButton;
    
    // 游戏相关
    GameWidget* gameWidget;
    MultiPlayerGameManager* multiPlayerManager;
    QString currentRoomId;
    QString playerName;
    
    // 定时器
    QTimer* refreshTimer;
};

#endif // MULTIPLAYERLOBBY_H