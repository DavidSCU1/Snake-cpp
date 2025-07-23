#include "hotspotlobby.h"
#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QNetworkInterface>
#include <QDebug>

// 样式常量定义
const QString HotspotLobby::BUTTON_STYLE = 
    "QPushButton {"
    "    background-color: #4CAF50;"
    "    border: none;"
    "    color: white;"
    "    padding: 10px 20px;"
    "    text-align: center;"
    "    font-size: 14px;"
    "    border-radius: 5px;"
    "}"
    "QPushButton:hover {"
    "    background-color: #45a049;"
    "}"
    "QPushButton:pressed {"
    "    background-color: #3d8b40;"
    "}"
    "QPushButton:disabled {"
    "    background-color: #cccccc;"
    "    color: #666666;"
    "}";

const QString HotspotLobby::INPUT_STYLE = 
    "QLineEdit, QComboBox {"
    "    padding: 8px;"
    "    border: 2px solid #ddd;"
    "    border-radius: 4px;"
    "    font-size: 14px;"
    "}"
    "QLineEdit:focus, QComboBox:focus {"
    "    border-color: #4CAF50;"
    "}";

const QString HotspotLobby::LABEL_STYLE = 
    "QLabel {"
    "    font-size: 14px;"
    "    color: #333;"
    "    margin: 5px 0;"
    "}";

const QString HotspotLobby::GROUP_STYLE = 
    "QGroupBox {"
    "    font-weight: bold;"
    "    border: 2px solid #ddd;"
    "    border-radius: 5px;"
    "    margin-top: 10px;"
    "    padding-top: 10px;"
    "}"
    "QGroupBox::title {"
    "    subcontrol-origin: margin;"
    "    left: 10px;"
    "    padding: 0 5px 0 5px;"
    "}";

HotspotLobby::HotspotLobby(QWidget *parent)
    : QWidget(parent)
    , networkManager(nullptr)
    , gameManager(nullptr)
    , mainLayout(new QVBoxLayout(this))
    , currentWidget(nullptr)
    , isHost(false)
    , isInRoom(false)
    , refreshTimer(new QTimer(this))
    , networkStatusTimer(new QTimer(this))
    , statusMessageTimer(new QTimer(this))
{
    setupUI();
    
    // 设置定时器
    refreshTimer->setSingleShot(false);
    networkStatusTimer->setSingleShot(false);
    statusMessageTimer->setSingleShot(true);
    
    connect(refreshTimer, &QTimer::timeout, this, &HotspotLobby::onRefreshTimer);
    connect(networkStatusTimer, &QTimer::timeout, this, &HotspotLobby::onNetworkStatusTimer);
    connect(statusMessageTimer, &QTimer::timeout, [this]() {
        statusLabel->setText("");
    });
    
    // 启动网络状态定时器
    networkStatusTimer->start(2000);
    
    // 显示模式选择界面
    showHostMode();
}

HotspotLobby::~HotspotLobby()
{
    if (networkManager) {
        if (isHost) {
            networkManager->stopHotspotHost();
        } else {
            networkManager->disconnectFromHost();
        }
    }
}

void HotspotLobby::setNetworkManager(HotspotNetworkManager* manager)
{
    if (networkManager) {
        disconnect(networkManager, nullptr, this, nullptr);
    }
    
    networkManager = manager;
    
    if (networkManager) {
        // 连接网络信号
        connect(networkManager, &HotspotNetworkManager::hostStarted,
                this, &HotspotLobby::onHostStarted);
        connect(networkManager, &HotspotNetworkManager::hostStopped,
                this, &HotspotLobby::onHostStopped);
        connect(networkManager, &HotspotNetworkManager::hostDiscovered,
                this, &HotspotLobby::onHostDiscovered);
        connect(networkManager, &HotspotNetworkManager::connectedToHost,
                this, &HotspotLobby::onConnectedToHost);
        connect(networkManager, &HotspotNetworkManager::disconnectedFromHost,
                this, &HotspotLobby::onDisconnectedFromHost);
        connect(networkManager, &HotspotNetworkManager::networkError,
                this, &HotspotLobby::onNetworkError);
    }
}

void HotspotLobby::setGameManager(HotspotGameManager* manager)
{
    if (gameManager) {
        disconnect(gameManager, nullptr, this, nullptr);
    }
    
    gameManager = manager;
    
    if (gameManager) {
        // 连接游戏信号
        connect(gameManager, &HotspotGameManager::playerJoined,
                this, &HotspotLobby::onPlayerJoined);
        connect(gameManager, &HotspotGameManager::playerLeft,
                this, &HotspotLobby::onPlayerLeft);
        connect(gameManager, &HotspotGameManager::playerCharacterChanged,
                this, &HotspotLobby::onPlayerCharacterChanged);
        connect(gameManager, &HotspotGameManager::playerReadyChanged,
                this, &HotspotLobby::onPlayerReadyChanged);
        connect(gameManager, &HotspotGameManager::gameStarted,
                this, &HotspotLobby::onGameStarted);
        connect(gameManager, &HotspotGameManager::gameEnded,
                this, &HotspotLobby::onGameEnded);
        connect(gameManager, &HotspotGameManager::countdownUpdated,
                this, &HotspotLobby::onCountdownUpdated);
    }
}

void HotspotLobby::showHostMode()
{
    if (currentWidget) {
        mainLayout->removeWidget(currentWidget);
        currentWidget->hide();
    }
    
    currentWidget = modeSelectionWidget;
    mainLayout->addWidget(currentWidget);
    currentWidget->show();
    
    updateNetworkStatus();
}

void HotspotLobby::showClientMode()
{
    if (currentWidget) {
        mainLayout->removeWidget(currentWidget);
        currentWidget->hide();
    }
    
    currentWidget = clientModeWidget;
    mainLayout->addWidget(currentWidget);
    currentWidget->show();
    
    // 开始搜索房间
    if (networkManager) {
        networkManager->startHostDiscovery();
        refreshTimer->start(3000);
    }
    
    updateNetworkStatus();
}

void HotspotLobby::showGameLobby()
{
    if (currentWidget) {
        mainLayout->removeWidget(currentWidget);
        currentWidget->hide();
    }
    
    currentWidget = gameLobbyWidget;
    mainLayout->addWidget(currentWidget);
    currentWidget->show();
    
    isInRoom = true;
    updatePlayerList();
    updateGameControls();
    updateNetworkStatus();
}

void HotspotLobby::resetLobby()
{
    isHost = false;
    isInRoom = false;
    currentPlayerName.clear();
    
    refreshTimer->stop();
    
    if (networkManager) {
        networkManager->stopHostDiscovery();
    }
    
    availableRoomsList->clear();
    playerListWidget->clear();
    chatDisplay->clear();
    
    showHostMode();
}

void HotspotLobby::setupUI()
{
    setStyleSheet(BUTTON_STYLE + INPUT_STYLE + LABEL_STYLE + GROUP_STYLE);
    
    setupModeSelectionUI();
    setupHostModeUI();
    setupClientModeUI();
    setupGameLobbyUI();
    setupNetworkStatusUI();
    
    // 状态栏
    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    mainLayout->addWidget(statusLabel);
    
    setLayout(mainLayout);
}

void HotspotLobby::setupModeSelectionUI()
{
    modeSelectionWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(modeSelectionWidget);
    
    // 标题
    QLabel* titleLabel = new QLabel("热点多人游戏");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; margin: 20px; }");
    layout->addWidget(titleLabel);
    
    // 说明文本
    QLabel* descLabel = new QLabel("请选择游戏模式：");
    descLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(descLabel);
    
    layout->addSpacing(20);
    
    // 主机模式按钮
    hostModeButton = new QPushButton("创建房间 (主机模式)");
    hostModeButton->setMinimumHeight(50);
    connect(hostModeButton, &QPushButton::clicked, [this]() {
        if (currentWidget) {
            mainLayout->removeWidget(currentWidget);
            currentWidget->hide();
        }
        currentWidget = hostModeWidget;
        mainLayout->addWidget(currentWidget);
        currentWidget->show();
    });
    layout->addWidget(hostModeButton);
    
    layout->addSpacing(10);
    
    // 客户端模式按钮
    clientModeButton = new QPushButton("加入房间 (客户端模式)");
    clientModeButton->setMinimumHeight(50);
    connect(clientModeButton, &QPushButton::clicked, this, &HotspotLobby::showClientMode);
    layout->addWidget(clientModeButton);
    
    layout->addSpacing(20);
    
    // 返回主菜单按钮
    backToMenuButton = new QPushButton("返回主菜单");
    backToMenuButton->setStyleSheet(
        "QPushButton { background-color: #f44336; }"
        "QPushButton:hover { background-color: #da190b; }"
        "QPushButton:pressed { background-color: #b71c1c; }"
    );
    connect(backToMenuButton, &QPushButton::clicked, this, &HotspotLobby::backToMainMenu);
    layout->addWidget(backToMenuButton);
    
    layout->addStretch();
}

void HotspotLobby::setupHostModeUI()
{
    hostModeWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(hostModeWidget);
    
    // 标题
    QLabel* titleLabel = new QLabel("创建房间");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { font-size: 20px; font-weight: bold; margin: 10px; }");
    layout->addWidget(titleLabel);
    
    // 房间设置
    QGroupBox* roomSettingsGroup = new QGroupBox("房间设置");
    QGridLayout* settingsLayout = new QGridLayout(roomSettingsGroup);
    
    settingsLayout->addWidget(new QLabel("房间名称:"), 0, 0);
    roomNameEdit = new QLineEdit();
    roomNameEdit->setPlaceholderText("输入房间名称");
    settingsLayout->addWidget(roomNameEdit, 0, 1);
    
    settingsLayout->addWidget(new QLabel("玩家名称:"), 1, 0);
    hostPlayerNameEdit = new QLineEdit();
    hostPlayerNameEdit->setPlaceholderText("输入您的名称");
    connect(hostPlayerNameEdit, &QLineEdit::textChanged, this, &HotspotLobby::onPlayerNameChanged);
    settingsLayout->addWidget(hostPlayerNameEdit, 1, 1);
    
    settingsLayout->addWidget(new QLabel("最大玩家数:"), 2, 0);
    maxPlayersCombo = new QComboBox();
    maxPlayersCombo->addItems({"2", "3", "4"});
    maxPlayersCombo->setCurrentText("4");
    settingsLayout->addWidget(maxPlayersCombo, 2, 1);
    
    layout->addWidget(roomSettingsGroup);
    
    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    createRoomButton = new QPushButton("创建房间");
    connect(createRoomButton, &QPushButton::clicked, this, &HotspotLobby::onCreateRoomClicked);
    buttonLayout->addWidget(createRoomButton);
    
    backFromHostButton = new QPushButton("返回");
    backFromHostButton->setStyleSheet(
        "QPushButton { background-color: #f44336; }"
        "QPushButton:hover { background-color: #da190b; }"
    );
    connect(backFromHostButton, &QPushButton::clicked, this, &HotspotLobby::showHostMode);
    buttonLayout->addWidget(backFromHostButton);
    
    layout->addLayout(buttonLayout);
    layout->addStretch();
}

void HotspotLobby::setupClientModeUI()
{
    clientModeWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(clientModeWidget);
    
    // 标题
    QLabel* titleLabel = new QLabel("加入房间");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { font-size: 20px; font-weight: bold; margin: 10px; }");
    layout->addWidget(titleLabel);
    
    // 玩家设置
    QGroupBox* playerGroup = new QGroupBox("玩家设置");
    QHBoxLayout* playerLayout = new QHBoxLayout(playerGroup);
    
    playerLayout->addWidget(new QLabel("玩家名称:"));
    clientPlayerNameEdit = new QLineEdit();
    clientPlayerNameEdit->setPlaceholderText("输入您的名称");
    connect(clientPlayerNameEdit, &QLineEdit::textChanged, this, &HotspotLobby::onPlayerNameChanged);
    playerLayout->addWidget(clientPlayerNameEdit);
    
    layout->addWidget(playerGroup);
    
    // 可用房间
    QGroupBox* roomsGroup = new QGroupBox("可用房间");
    QVBoxLayout* roomsLayout = new QVBoxLayout(roomsGroup);
    
    availableRoomsList = new QListWidget();
    availableRoomsList->setMinimumHeight(200);
    roomsLayout->addWidget(availableRoomsList);
    
    QHBoxLayout* roomButtonLayout = new QHBoxLayout();
    
    refreshRoomsButton = new QPushButton("刷新房间");
    connect(refreshRoomsButton, &QPushButton::clicked, this, &HotspotLobby::onRefreshRoomsClicked);
    roomButtonLayout->addWidget(refreshRoomsButton);
    
    joinRoomButton = new QPushButton("加入房间");
    connect(joinRoomButton, &QPushButton::clicked, this, &HotspotLobby::onJoinRoomClicked);
    roomButtonLayout->addWidget(joinRoomButton);
    
    roomsLayout->addLayout(roomButtonLayout);
    layout->addWidget(roomsGroup);
    
    // 返回按钮
    backFromClientButton = new QPushButton("返回");
    backFromClientButton->setStyleSheet(
        "QPushButton { background-color: #f44336; }"
        "QPushButton:hover { background-color: #da190b; }"
    );
    connect(backFromClientButton, &QPushButton::clicked, this, &HotspotLobby::showHostMode);
    layout->addWidget(backFromClientButton);
    
    layout->addStretch();
}

void HotspotLobby::setupGameLobbyUI()
{
    gameLobbyWidget = new QWidget();
    QHBoxLayout* mainLobbyLayout = new QHBoxLayout(gameLobbyWidget);
    
    // 左侧：房间信息和玩家列表
    QVBoxLayout* leftLayout = new QVBoxLayout();
    
    // 房间信息
    roomInfoLabel = new QLabel("房间信息");
    roomInfoLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; }");
    leftLayout->addWidget(roomInfoLabel);
    
    // 玩家列表
    QGroupBox* playersGroup = new QGroupBox("玩家列表");
    QVBoxLayout* playersLayout = new QVBoxLayout(playersGroup);
    
    playerListWidget = new QListWidget();
    playerListWidget->setMinimumHeight(200);
    playersLayout->addWidget(playerListWidget);
    
    leftLayout->addWidget(playersGroup);
    
    // 玩家设置
    QGroupBox* settingsGroup = new QGroupBox("玩家设置");
    QGridLayout* settingsLayout = new QGridLayout(settingsGroup);
    
    settingsLayout->addWidget(new QLabel("角色选择:"), 0, 0);
    characterSelectionCombo = new QComboBox();
    characterSelectionCombo->addItem("海绵宝宝", static_cast<int>(CharacterType::SPONGEBOB));
    characterSelectionCombo->addItem("派大星", static_cast<int>(CharacterType::PATRICK));
    characterSelectionCombo->addItem("章鱼哥", static_cast<int>(CharacterType::SQUIDWARD));
    characterSelectionCombo->addItem("蟹老板", static_cast<int>(CharacterType::MR_KRABS));
    characterSelectionCombo->addItem("珊迪", static_cast<int>(CharacterType::SANDY));
    characterSelectionCombo->addItem("痞老板", static_cast<int>(CharacterType::PLANKTON));
    connect(characterSelectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &HotspotLobby::onCharacterSelectionChanged);
    settingsLayout->addWidget(characterSelectionCombo, 0, 1);
    
    readyCheckBox = new QCheckBox("准备就绪");
    connect(readyCheckBox, &QCheckBox::toggled, this, &HotspotLobby::onReadyToggled);
    settingsLayout->addWidget(readyCheckBox, 1, 0, 1, 2);
    
    leftLayout->addWidget(settingsGroup);
    
    // 游戏控制按钮
    QHBoxLayout* gameButtonLayout = new QHBoxLayout();
    
    startGameButton = new QPushButton("开始游戏");
    startGameButton->setEnabled(false);
    connect(startGameButton, &QPushButton::clicked, this, &HotspotLobby::onStartGameClicked);
    gameButtonLayout->addWidget(startGameButton);
    
    leaveRoomButton = new QPushButton("离开房间");
    leaveRoomButton->setStyleSheet(
        "QPushButton { background-color: #f44336; }"
        "QPushButton:hover { background-color: #da190b; }"
    );
    connect(leaveRoomButton, &QPushButton::clicked, this, &HotspotLobby::onLeaveRoomClicked);
    gameButtonLayout->addWidget(leaveRoomButton);
    
    leftLayout->addLayout(gameButtonLayout);
    
    mainLobbyLayout->addLayout(leftLayout, 2);
    
    // 右侧：聊天系统
    setupChatUI(mainLobbyLayout);
}

void HotspotLobby::setupChatUI(QHBoxLayout* parentLayout)
{
    chatGroupBox = new QGroupBox("聊天");
    QVBoxLayout* chatLayout = new QVBoxLayout(chatGroupBox);
    
    chatDisplay = new QTextEdit();
    chatDisplay->setReadOnly(true);
    chatDisplay->setMaximumHeight(200);
    chatLayout->addWidget(chatDisplay);
    
    QHBoxLayout* chatInputLayout = new QHBoxLayout();
    
    chatInput = new QLineEdit();
    chatInput->setPlaceholderText("输入消息...");
    connect(chatInput, &QLineEdit::returnPressed, this, &HotspotLobby::onSendChatMessage);
    chatInputLayout->addWidget(chatInput);
    
    sendChatButton = new QPushButton("发送");
    connect(sendChatButton, &QPushButton::clicked, this, &HotspotLobby::onSendChatMessage);
    chatInputLayout->addWidget(sendChatButton);
    
    chatLayout->addLayout(chatInputLayout);
    
    parentLayout->addWidget(chatGroupBox, 1);
}

void HotspotLobby::setupNetworkStatusUI()
{
    networkStatusGroup = new QGroupBox("网络状态");
    QGridLayout* statusLayout = new QGridLayout(networkStatusGroup);
    
    statusLayout->addWidget(new QLabel("连接状态:"), 0, 0);
    connectionStatusLabel = new QLabel("未连接");
    statusLayout->addWidget(connectionStatusLabel, 0, 1);
    
    statusLayout->addWidget(new QLabel("网络类型:"), 1, 0);
    networkTypeLabel = new QLabel("未知");
    statusLayout->addWidget(networkTypeLabel, 1, 1);
    
    statusLayout->addWidget(new QLabel("IP地址:"), 2, 0);
    ipAddressLabel = new QLabel("未知");
    statusLayout->addWidget(ipAddressLabel, 2, 1);
    
    statusLayout->addWidget(new QLabel("信号强度:"), 3, 0);
    signalStrengthBar = new QProgressBar();
    signalStrengthBar->setRange(0, 100);
    signalStrengthBar->setValue(0);
    statusLayout->addWidget(signalStrengthBar, 3, 1);
    
    // 将网络状态组添加到主布局的底部
    mainLayout->addWidget(networkStatusGroup);
}

void HotspotLobby::onCreateRoomClicked()
{
    QString roomName = roomNameEdit->text().trimmed();
    QString playerName = hostPlayerNameEdit->text().trimmed();
    
    if (roomName.isEmpty()) {
        showStatusMessage("请输入房间名称");
        return;
    }
    
    if (playerName.isEmpty()) {
        showStatusMessage("请输入玩家名称");
        return;
    }
    
    if (!networkManager) {
        showStatusMessage("网络管理器未初始化");
        return;
    }
    
    currentPlayerName = playerName;
    isHost = true;
    
    // 创建房间
    if (gameManager && gameManager->createRoom(playerName, roomName)) {
        showStatusMessage("正在创建房间...");
    } else {
        showStatusMessage("创建房间失败");
    }
}

void HotspotLobby::onJoinRoomClicked()
{
    QListWidgetItem* selectedItem = availableRoomsList->currentItem();
    if (!selectedItem) {
        showStatusMessage("请选择一个房间");
        return;
    }
    
    QString playerName = clientPlayerNameEdit->text().trimmed();
    if (playerName.isEmpty()) {
        showStatusMessage("请输入玩家名称");
        return;
    }
    
    QString hostAddress = selectedItem->data(Qt::UserRole).toString();
    if (hostAddress.isEmpty()) {
        showStatusMessage("无效的房间地址");
        return;
    }
    
    currentPlayerName = playerName;
    isHost = false;
    
    // 连接到主机
    if (networkManager && networkManager->connectToHost(hostAddress)) {
        showStatusMessage("正在连接到房间...");
    } else {
        showStatusMessage("连接失败");
    }
}

void HotspotLobby::onRefreshRoomsClicked()
{
    availableRoomsList->clear();
    if (networkManager) {
        networkManager->startHostDiscovery();
        showStatusMessage("正在搜索房间...");
    }
}

void HotspotLobby::onStartGameClicked()
{
    if (gameManager && isHost) {
        if (gameManager->startGame()) {
            showStatusMessage("游戏即将开始...");
        } else {
            showStatusMessage("无法开始游戏，请确保所有玩家都已准备");
        }
    }
}

void HotspotLobby::onLeaveRoomClicked()
{
    if (gameManager) {
        gameManager->leaveRoom(currentPlayerName);
    }
    
    resetLobby();
    showStatusMessage("已离开房间");
}

void HotspotLobby::onBackClicked()
{
    resetLobby();
}

void HotspotLobby::onPlayerNameChanged()
{
    // 更新当前玩家名称
    QLineEdit* sender = qobject_cast<QLineEdit*>(this->sender());
    if (sender) {
        currentPlayerName = sender->text().trimmed();
    }
}

void HotspotLobby::onCharacterSelectionChanged()
{
    if (gameManager && !currentPlayerName.isEmpty()) {
        CharacterType character = static_cast<CharacterType>(characterSelectionCombo->currentData().toInt());
        gameManager->setPlayerCharacter(currentPlayerName, character);
    }
}

void HotspotLobby::onReadyToggled(bool ready)
{
    if (gameManager && !currentPlayerName.isEmpty()) {
        gameManager->setPlayerReady(currentPlayerName, ready);
    }
}

void HotspotLobby::onSendChatMessage()
{
    QString message = chatInput->text().trimmed();
    if (message.isEmpty() || !networkManager) {
        return;
    }
    
    networkManager->sendChatMessage(currentPlayerName, message);
    addChatMessage(QString("%1: %2").arg(currentPlayerName, message));
    chatInput->clear();
}

void HotspotLobby::onRefreshTimer()
{
    // 定期刷新房间列表
    if (networkManager && currentWidget == clientModeWidget) {
        networkManager->startHostDiscovery();
    }
}

void HotspotLobby::onNetworkStatusTimer()
{
    updateNetworkStatus();
}

void HotspotLobby::onHostStarted(const QString& roomName, const QString& ipAddress)
{
    showGameLobby();
    roomInfoLabel->setText(QString("房间: %1\nIP: %2").arg(roomName, ipAddress));
    showStatusMessage("房间创建成功");
}

void HotspotLobby::onHostStopped()
{
    resetLobby();
    showStatusMessage("房间已关闭");
}

void HotspotLobby::onHostDiscovered(const QString& hostAddress, const QString& roomName, int playerCount, int maxPlayers)
{
    // 检查房间是否已存在
    for (int i = 0; i < availableRoomsList->count(); ++i) {
        QListWidgetItem* item = availableRoomsList->item(i);
        if (item->data(Qt::UserRole).toString() == hostAddress) {
            // 更新已有房间信息
            item->setText(QString("%1 (%2/%3)").arg(roomName).arg(playerCount).arg(maxPlayers));
            return;
        }
    }
    
    // 新增房间条目
    QString itemText = QString("%1 (%2/%3)").arg(roomName).arg(playerCount).arg(maxPlayers);
    QListWidgetItem* item = new QListWidgetItem(itemText);
    item->setData(Qt::UserRole, hostAddress);
    availableRoomsList->addItem(item);
}

void HotspotLobby::onConnectedToHost(const QString& hostAddress)
{
    if (gameManager && gameManager->joinRoom(currentPlayerName)) {
        showGameLobby();
        roomInfoLabel->setText(QString("已连接到: %1").arg(hostAddress));
        showStatusMessage("成功加入房间");
    }
}

void HotspotLobby::onDisconnectedFromHost()
{
    resetLobby();
    showStatusMessage("与主机断开连接");
}

void HotspotLobby::onNetworkError(const QString& errorMessage)
{
    showStatusMessage(QString("网络错误: %1").arg(errorMessage));
}

void HotspotLobby::onPlayerJoined(const QString& playerName)
{
    updatePlayerList();
    addChatMessage(QString("玩家 %1 加入了房间").arg(playerName));
}

void HotspotLobby::onPlayerLeft(const QString& playerName)
{
    updatePlayerList();
    addChatMessage(QString("玩家 %1 离开了房间").arg(playerName));
}

void HotspotLobby::onPlayerCharacterChanged(const QString& playerName, CharacterType character)
{
    updatePlayerList();
    addChatMessage(QString("玩家 %1 选择了角色: %2").arg(playerName, getCharacterName(character)));
}

void HotspotLobby::onPlayerReadyChanged(const QString& playerName, bool ready)
{
    updatePlayerList();
    updateGameControls();
    addChatMessage(QString("玩家 %1 %2").arg(playerName, ready ? "已准备" : "取消准备"));
}

void HotspotLobby::onGameStarted()
{
    emit gameStartRequested();
}

void HotspotLobby::onGameEnded(const QString& winner)
{
    showStatusMessage(QString("游戏结束，获胜者: %1").arg(winner));
}

void HotspotLobby::onCountdownUpdated(int seconds)
{
    showStatusMessage(QString("游戏将在 %1 秒后开始").arg(seconds));
}

void HotspotLobby::updatePlayerList()
{
    if (!gameManager) {
        return;
    }
    
    playerListWidget->clear();
    
    QStringList playerNames = gameManager->getPlayerNames();
    HotspotGameState gameState = gameManager->getGameState();
    
    for (const QString& playerName : playerNames) {
        QString itemText = playerName;
        
        if (gameState.playerCharacters.contains(playerName)) {
            CharacterType character = gameState.playerCharacters[playerName];
            itemText += QString(" (%1)").arg(getCharacterName(character));
        }
        
        if (gameState.playerReadyStatus.contains(playerName) && gameState.playerReadyStatus[playerName]) {
            itemText += " [已准备]";
        }
        
        if (playerName == currentPlayerName) {
            itemText += " (我)";
        }
        
        QListWidgetItem* item = new QListWidgetItem(itemText);
        if (gameState.playerReadyStatus.value(playerName, false)) {
            item->setBackground(QBrush(QColor(200, 255, 200)));
        }
        
        playerListWidget->addItem(item);
    }
}

void HotspotLobby::updateNetworkStatus()
{
    if (!networkManager) {
        connectionStatusLabel->setText("未初始化");
        networkTypeLabel->setText("未知");
        ipAddressLabel->setText("未知");
        signalStrengthBar->setValue(0);
        return;
    }
    
    // 更新连接状态
    if (networkManager->isHosting()) {
        connectionStatusLabel->setText("主机模式");
        connectionStatusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    } else if (networkManager->isConnectedToHost()) {
        connectionStatusLabel->setText("已连接");
        connectionStatusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    } else {
        connectionStatusLabel->setText("未连接");
        connectionStatusLabel->setStyleSheet("QLabel { color: red; }");
    }
    
    // 更新网络类型和IP
    if (networkManager->isInHotspotNetwork()) {
        networkTypeLabel->setText("热点网络");
        networkTypeLabel->setStyleSheet("QLabel { color: green; }");
        
        QString localIP = networkManager->getLocalIPAddress();
        ipAddressLabel->setText(localIP);
        
        // 模拟信号强度
        signalStrengthBar->setValue(85);
    } else {
        networkTypeLabel->setText("非热点网络");
        networkTypeLabel->setStyleSheet("QLabel { color: orange; }");
        ipAddressLabel->setText("未知");
        signalStrengthBar->setValue(0);
    }
}

void HotspotLobby::updateGameControls()
{
    if (!gameManager || !isHost) {
        startGameButton->setEnabled(false);
        return;
    }
    
    // 检查是否所有玩家都已准备
    HotspotGameState gameState = gameManager->getGameState();
    bool allReady = true;
    int playerCount = gameState.playerReadyStatus.size();
    
    if (playerCount < 2) {
        allReady = false;
    } else {
        for (auto it = gameState.playerReadyStatus.begin(); it != gameState.playerReadyStatus.end(); ++it) {
            if (!it.value()) {
                allReady = false;
                break;
            }
        }
    }
    
    startGameButton->setEnabled(allReady);
}

void HotspotLobby::addChatMessage(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString formattedMessage = QString("[%1] %2").arg(timestamp, message);
    chatDisplay->append(formattedMessage);
    
    // 自动滚动到底部
    QTextCursor cursor = chatDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    chatDisplay->setTextCursor(cursor);
}

void HotspotLobby::showStatusMessage(const QString& message, int timeout)
{
    statusLabel->setText(message);
    statusMessageTimer->start(timeout);
}

QString HotspotLobby::getCharacterName(CharacterType character) const
{
    switch (character) {
        case CharacterType::SPONGEBOB: return "海绵宝宝";
        case CharacterType::PATRICK: return "派大星";
        case CharacterType::SQUIDWARD: return "章鱼哥";
        case CharacterType::MR_KRABS: return "蟹老板";
        case CharacterType::SANDY: return "珊迪";
        case CharacterType::PLANKTON: return "痞老板";
        default: return "未知";
    }
}

QPixmap HotspotLobby::getCharacterIcon(CharacterType character) const
{
    // 这里可以返回角色图标，暂时返回空
    Q_UNUSED(character)
    return QPixmap();
}