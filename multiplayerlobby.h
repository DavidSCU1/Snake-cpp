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
#include <QCheckBox>
#include "gamestate.h"
#include "multiplayergamemanager.h"
#include "characterselection.h"
#include <QMap>
#include <QDateTime>

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
    void onManualConnectClicked(); // 新增槽函数声明
    void onRefreshClicked();
    void onBackClicked();
    void onRoomSelectionChanged();
    void onPlayerNameChanged();
    void refreshRoomList();
    
    // 等待界面槽函数
    void onStartGameClicked();
    void onLeaveRoomClicked();
    void showWaitingInterface();
    void hideWaitingInterface();
    
    // 角色选择界面槽函数
    void showCharacterSelection();
    void hideCharacterSelection();
    void onCharacterSelected(CharacterType character);
    void onCharacterSelectionStart();
    void onCharacterSelectionBack();
    
    // MultiPlayerGameManager 信号槽
    void onRoomCreated(const QString& roomId, const GameRoom& room);
    void onPlayerJoinedRoom(const QString& roomId, const QString& playerName);
    void onPlayerLeftRoom(const QString& roomId, const QString& playerName);
    void onGameStarted(const QString& roomId);
    void onGameEnded(const QString& roomId, const QString& winner);
    void onRoomDestroyed(const QString& roomId);
    void onRoomDiscovered(const QString& host, int port);
    void onConnectionError(const QString& error);
    
    // 网络管理器信号槽
    void onCharacterSelectionStarted(const QString& roomId);
    void onPlayerCharacterSelected(const QString& roomId, const QString& playerName, int character);
    void onAllPlayersReady(const QString& roomId);
    
    // 角色选择准备机制槽函数
    void onPlayerReadyChanged(bool ready);
    void onAllPlayersReadyInSelection();
    void onPlayerReadyReceived(const QString& roomId, const QString& playerName, bool ready);
    void onGameCountdownReceived(const QString& roomId, int countdown); // 新增：处理游戏倒计时
    
private:
    void sendJoinSuccessAck();
    void setupUI();
    void setupWaitingInterface();
    void setupCharacterSelection();
    void updateRoomInfo(const GameRoom& room);
    void clearRoomInfo();
    bool validatePlayerName() const;
    void startGameCountdown(); // 新增：开始游戏倒计时函数
    
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
    QCheckBox* allowJoinCheckBox = nullptr;
    
    // 控制按钮
    QWidget* buttonWidget;
    QHBoxLayout* buttonLayout;
    QPushButton* createRoomButton;
    QPushButton* joinRoomButton;
    QPushButton* manualConnectButton; // 新增手动连接按钮
    QLineEdit* manualIpEdit; // 新增手动输入IP框
    QPushButton* backButton;
    
    // 等待界面组件
    QWidget* waitingWidget;
    QVBoxLayout* waitingLayout;
    QLabel* waitingTitleLabel;
    QLabel* waitingRoomIdLabel;
    QLabel* waitingPlayerCountLabel;
    QListWidget* waitingPlayerListWidget;
    QPushButton* startGameButton;
    QPushButton* leaveRoomButton;
    
    // 角色选择界面组件
    CharacterSelection* characterSelectionWidget = nullptr;
    
    // 游戏相关
    GameWidget* gameWidget;
    MultiPlayerGameManager* multiPlayerManager;
    QString currentRoomId;
    QString playerName;
    
    // 定时器
    QTimer* refreshTimer;
    QMap<QString, qint64> discoveredRooms; // 房间唯一标识 -> 最后一次收到广播的时间
    QTimer* roomCleanupTimer = nullptr;
    QTimer* countdownTimer = nullptr;
    QLabel* countdownLabel = nullptr;
};

#endif // MULTIPLAYERLOBBY_H