#ifndef HOTSPOTLOBBY_H
#define HOTSPOTLOBBY_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QComboBox>
#include <QProgressBar>
#include <QTimer>
#include <QGroupBox>
#include <QTextEdit>
#include <QCheckBox>
#include "hotspotnetworkmanager.h"
#include "hotspotgamemanager.h"
#include "gamestate.h"

/**
 * 热点多人游戏大厅
 * 专为热点网络环境设计的简化大厅界面
 * 特点：
 * 1. 简洁的房间创建和加入流程
 * 2. 实时的网络状态显示
 * 3. 优化的玩家管理界面
 * 4. 直观的游戏准备流程
 */
class HotspotLobby : public QWidget
{
    Q_OBJECT

public:
    explicit HotspotLobby(QWidget *parent = nullptr);
    ~HotspotLobby();
    
    // 设置管理器
    void setNetworkManager(HotspotNetworkManager* manager);
    void setGameManager(HotspotGameManager* manager);
    
    // 界面控制
    void showHostMode();
    void showClientMode();
    void showGameLobby();
    void resetLobby();
    
signals:
    void gameStartRequested();
    void backToMainMenu();
    void characterSelectionRequested();
    
private slots:
    // 网络事件
    void onHostStarted(const QString& roomName, const QString& ipAddress);
    void onHostStopped();
    void onHostDiscovered(const QString& hostAddress, const QString& roomName, int playerCount, int maxPlayers);
    void onConnectedToHost(const QString& hostAddress);
    void onDisconnectedFromHost();
    void onNetworkError(const QString& errorMessage);
    
    // 玩家事件
    void onPlayerJoined(const QString& playerName);
    void onPlayerLeft(const QString& playerName);
    void onPlayerCharacterChanged(const QString& playerName, CharacterType character);
    void onPlayerReadyChanged(const QString& playerName, bool ready);
    
    // 游戏事件
    void onGameStarted();
    void onGameEnded(const QString& winner);
    void onCountdownUpdated(int seconds);
    
    // UI事件
    void onCreateRoomClicked();
    void onJoinRoomClicked();
    void onRefreshRoomsClicked();
    void onStartGameClicked();
    void onLeaveRoomClicked();
    void onBackClicked();
    void onPlayerNameChanged();
    void onCharacterSelectionChanged();
    void onReadyToggled(bool ready);
    void onSendChatMessage();
    
    // 定时器事件
    void onRefreshTimer();
    void onNetworkStatusTimer();
    
private:
    void setupUI();
    void setupModeSelectionUI();
    void setupHostModeUI();
    void setupClientModeUI();
    void setupGameLobbyUI();
    void setupNetworkStatusUI();
    void setupChatUI(QHBoxLayout* parentLayout);
    
    void updatePlayerList();
    void updateNetworkStatus();
    void updateGameControls();
    void addChatMessage(const QString& message);
    void showStatusMessage(const QString& message, int timeout = 3000);
    
    QString getCharacterName(CharacterType character) const;
    QPixmap getCharacterIcon(CharacterType character) const;
    
    // 管理器
    HotspotNetworkManager* networkManager;
    HotspotGameManager* gameManager;
    
    // 主布局
    QVBoxLayout* mainLayout;
    QWidget* currentWidget;
    
    // 模式选择界面
    QWidget* modeSelectionWidget;
    QPushButton* hostModeButton;
    QPushButton* clientModeButton;
    QPushButton* backToMenuButton;
    
    // 主机模式界面
    QWidget* hostModeWidget;
    QLineEdit* roomNameEdit;
    QLineEdit* hostPlayerNameEdit;
    QComboBox* maxPlayersCombo;
    QPushButton* createRoomButton;
    QPushButton* backFromHostButton;
    
    // 客户端模式界面
    QWidget* clientModeWidget;
    QLineEdit* clientPlayerNameEdit;
    QListWidget* availableRoomsList;
    QPushButton* joinRoomButton;
    QPushButton* refreshRoomsButton;
    QPushButton* backFromClientButton;
    
    // 游戏大厅界面
    QWidget* gameLobbyWidget;
    QLabel* roomInfoLabel;
    QListWidget* playerListWidget;
    QComboBox* characterSelectionCombo;
    QCheckBox* readyCheckBox;
    QPushButton* startGameButton;
    QPushButton* leaveRoomButton;
    
    // 聊天系统
    QGroupBox* chatGroupBox;
    QTextEdit* chatDisplay;
    QLineEdit* chatInput;
    QPushButton* sendChatButton;
    
    // 网络状态显示
    QGroupBox* networkStatusGroup;
    QLabel* connectionStatusLabel;
    QLabel* networkTypeLabel;
    QLabel* ipAddressLabel;
    QProgressBar* signalStrengthBar;
    
    // 状态栏
    QLabel* statusLabel;
    
    // 定时器
    QTimer* refreshTimer;
    QTimer* networkStatusTimer;
    QTimer* statusMessageTimer;
    
    // 状态变量
    QString currentPlayerName;
    bool isHost;
    bool isInRoom;
    
    // 样式常量
    static const QString BUTTON_STYLE;
    static const QString INPUT_STYLE;
    static const QString LABEL_STYLE;
    static const QString GROUP_STYLE;
};

#endif // HOTSPOTLOBBY_H