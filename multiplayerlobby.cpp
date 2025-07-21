#include "multiplayerlobby.h"
#include "gamewidget.h"
#include <QDebug>

MultiPlayerLobby::MultiPlayerLobby(QWidget *parent)
    : QWidget(parent)
    , gameWidget(nullptr)
    , multiPlayerManager(new MultiPlayerGameManager(this))
    , refreshTimer(new QTimer(this))
{
    setupUI();
    
    // 连接MultiPlayerGameManager信号
    connect(multiPlayerManager, &MultiPlayerGameManager::roomCreated, this, &MultiPlayerLobby::onRoomCreated);
    connect(multiPlayerManager, &MultiPlayerGameManager::playerJoinedRoom, this, &MultiPlayerLobby::onPlayerJoinedRoom);
    connect(multiPlayerManager, &MultiPlayerGameManager::playerLeftRoom, this, &MultiPlayerLobby::onPlayerLeftRoom);
    connect(multiPlayerManager, &MultiPlayerGameManager::gameStarted, this, &MultiPlayerLobby::onGameStarted);
    connect(multiPlayerManager, &MultiPlayerGameManager::roomDestroyed, this, &MultiPlayerLobby::onRoomDestroyed);
    connect(multiPlayerManager, &MultiPlayerGameManager::gameEnded, this, &MultiPlayerLobby::onGameEnded);
    
    // 设置定时刷新
    connect(refreshTimer, &QTimer::timeout, this, &MultiPlayerLobby::refreshRoomList);
    refreshTimer->start(3000); // 每3秒刷新一次房间列表
    
    // 初始刷新
    refreshRoomList();
}

MultiPlayerLobby::~MultiPlayerLobby()
{
    if (!currentRoomId.isEmpty() && !playerName.isEmpty()) {
        multiPlayerManager->leaveRoom(currentRoomId, playerName);
    }
}

void MultiPlayerLobby::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // 标题
    QLabel* titleLabel = new QLabel("多人游戏大厅", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("华文彩云", 18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #FF6347; margin: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // 主要内容区域
    contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(20);
    
    // 左侧房间列表
    roomListWidget = new QWidget(this);
    roomListWidget->setFixedWidth(300);
    roomListLayout = new QVBoxLayout(roomListWidget);
    
    roomListLabel = new QLabel("房间列表", roomListWidget);
    roomListLabel->setAlignment(Qt::AlignCenter);
    QFont labelFont("华文彩云", 12);
    labelFont.setBold(true);
    roomListLabel->setFont(labelFont);
    roomListLayout->addWidget(roomListLabel);
    
    roomList = new QListWidget(roomListWidget);
    roomList->setMinimumHeight(200);
    connect(roomList, &QListWidget::itemSelectionChanged, this, &MultiPlayerLobby::onRoomSelectionChanged);
    roomListLayout->addWidget(roomList);
    
    refreshButton = new QPushButton("刷新房间列表", roomListWidget);
    refreshButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; padding: 8px; border-radius: 4px; font-family: '华文彩云'; }"
                                "QPushButton:hover { background-color: #45a049; }");
    connect(refreshButton, &QPushButton::clicked, this, &MultiPlayerLobby::onRefreshClicked);
    roomListLayout->addWidget(refreshButton);
    
    contentLayout->addWidget(roomListWidget);
    
    // 右侧房间信息和控制
    roomInfoWidget = new QWidget(this);
    roomInfoLayout = new QVBoxLayout(roomInfoWidget);
    
    // 房间信息显示
    roomInfoGroup = new QGroupBox("房间信息", roomInfoWidget);
    roomInfoGroupLayout = new QVBoxLayout(roomInfoGroup);
    
    roomIdLabel = new QLabel("房间ID: 未选择", roomInfoGroup);
    roomNameLabel = new QLabel("房间名: 未选择", roomInfoGroup);
    playerCountLabel = new QLabel("玩家数量: 0/0", roomInfoGroup);
    roomStatusLabel = new QLabel("状态: 未知", roomInfoGroup);
    
    roomInfoGroupLayout->addWidget(roomIdLabel);
    roomInfoGroupLayout->addWidget(roomNameLabel);
    roomInfoGroupLayout->addWidget(playerCountLabel);
    roomInfoGroupLayout->addWidget(roomStatusLabel);
    
    playerListWidget = new QListWidget(roomInfoGroup);
    playerListWidget->setMaximumHeight(100);
    roomInfoGroupLayout->addWidget(new QLabel("房间内玩家:", roomInfoGroup));
    roomInfoGroupLayout->addWidget(playerListWidget);
    
    roomInfoLayout->addWidget(roomInfoGroup);
    
    // 玩家设置
    playerSettingsGroup = new QGroupBox("玩家设置", roomInfoWidget);
    playerSettingsLayout = new QVBoxLayout(playerSettingsGroup);
    
    playerNameLabel = new QLabel("玩家名称:", playerSettingsGroup);
    playerNameEdit = new QLineEdit(playerSettingsGroup);
    playerNameEdit->setPlaceholderText("请输入玩家名称");
    playerNameEdit->setText("Player1"); // 默认名称
    connect(playerNameEdit, &QLineEdit::textChanged, this, &MultiPlayerLobby::onPlayerNameChanged);
    
    playerSettingsLayout->addWidget(playerNameLabel);
    playerSettingsLayout->addWidget(playerNameEdit);
    
    roomInfoLayout->addWidget(playerSettingsGroup);
    
    // 房间创建设置
    createRoomGroup = new QGroupBox("创建房间", roomInfoWidget);
    createRoomLayout = new QVBoxLayout(createRoomGroup);
    
    maxPlayersLabel = new QLabel("最大玩家数:", createRoomGroup);
    maxPlayersSpinBox = new QSpinBox(createRoomGroup);
    maxPlayersSpinBox->setRange(2, 8);
    maxPlayersSpinBox->setValue(4);
    
    createRoomLayout->addWidget(maxPlayersLabel);
    createRoomLayout->addWidget(maxPlayersSpinBox);
    
    roomInfoLayout->addWidget(createRoomGroup);
    
    // 控制按钮
    buttonWidget = new QWidget(roomInfoWidget);
    buttonLayout = new QHBoxLayout(buttonWidget);
    
    createRoomButton = new QPushButton("创建房间", buttonWidget);
    createRoomButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; border: none; padding: 10px; border-radius: 4px; font-family: '华文彩云'; }"
                                   "QPushButton:hover { background-color: #1976D2; }"
                                   "QPushButton:disabled { background-color: #CCCCCC; }");
    connect(createRoomButton, &QPushButton::clicked, this, &MultiPlayerLobby::onCreateRoomClicked);
    
    joinRoomButton = new QPushButton("加入房间", buttonWidget);
    joinRoomButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; border: none; padding: 10px; border-radius: 4px; font-family: '华文彩云'; }"
                                 "QPushButton:hover { background-color: #F57C00; }"
                                 "QPushButton:disabled { background-color: #CCCCCC; }");
    joinRoomButton->setEnabled(false);
    connect(joinRoomButton, &QPushButton::clicked, this, &MultiPlayerLobby::onJoinRoomClicked);
    
    buttonLayout->addWidget(createRoomButton);
    buttonLayout->addWidget(joinRoomButton);
    
    roomInfoLayout->addWidget(buttonWidget);
    
    // 返回按钮
    backButton = new QPushButton("返回主菜单", roomInfoWidget);
    backButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; border: none; padding: 10px; border-radius: 4px; font-family: '华文彩云'; }"
                             "QPushButton:hover { background-color: #5A6268; }");
    connect(backButton, &QPushButton::clicked, this, &MultiPlayerLobby::onBackClicked);
    roomInfoLayout->addWidget(backButton);
    
    contentLayout->addWidget(roomInfoWidget);
    
    mainLayout->addLayout(contentLayout);
    
    // 初始状态
    clearRoomInfo();
}

void MultiPlayerLobby::setGameWidget(GameWidget* gameWidget)
{
    this->gameWidget = gameWidget;
    if (gameWidget) {
        // 设置共享的MultiPlayerGameManager实例
        gameWidget->setMultiPlayerManager(multiPlayerManager);
    }
}

void MultiPlayerLobby::refreshRoomList()
{
    if (!multiPlayerManager) {
        return;
    }
    
    QStringList availableRooms = multiPlayerManager->getAvailableRooms();
    roomList->clear();
    
    for (const QString& roomId : availableRooms) {
        GameRoom room = multiPlayerManager->getRoomInfo(roomId);
        QString displayText = QString("房间 %1 (%2/%3)")
                             .arg(roomId)
                             .arg(room.currentPlayers)
                             .arg(room.maxPlayers);
        
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, roomId);
        
        // 根据房间状态设置颜色
        if (room.currentPlayers >= room.maxPlayers) {
            item->setForeground(QColor("#FF5722")); // 红色表示已满
        } else if (room.isGameStarted) {
            item->setForeground(QColor("#FF9800")); // 橙色表示游戏中
        } else {
            item->setForeground(QColor("#4CAF50")); // 绿色表示可加入
        }
        
        roomList->addItem(item);
    }
}

void MultiPlayerLobby::onCreateRoomClicked()
{
    if (!validatePlayerName()) {
        return;
    }
    
    playerName = playerNameEdit->text().trimmed();
    int maxPlayers = maxPlayersSpinBox->value();
    
    currentRoomId = multiPlayerManager->createRoom(playerName, maxPlayers);
    
    if (!currentRoomId.isEmpty()) {
        QMessageBox::information(this, "成功", QString("房间创建成功！\n房间ID: %1").arg(currentRoomId));
        refreshRoomList();
    } else {
        QMessageBox::warning(this, "错误", "房间创建失败！");
    }
}

void MultiPlayerLobby::onJoinRoomClicked()
{
    if (!validatePlayerName()) {
        return;
    }
    
    QListWidgetItem* currentItem = roomList->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "错误", "请先选择一个房间！");
        return;
    }
    
    QString roomId = currentItem->data(Qt::UserRole).toString();
    playerName = playerNameEdit->text().trimmed();
    
    if (multiPlayerManager->joinRoom(roomId, playerName)) {
        currentRoomId = roomId;
        QMessageBox::information(this, "成功", QString("成功加入房间 %1！").arg(roomId));
        refreshRoomList();
    } else {
        QMessageBox::warning(this, "错误", "加入房间失败！房间可能已满或不存在。");
    }
}

void MultiPlayerLobby::onRefreshClicked()
{
    refreshRoomList();
}

void MultiPlayerLobby::onBackClicked()
{
    if (!currentRoomId.isEmpty() && !playerName.isEmpty()) {
        multiPlayerManager->leaveRoom(currentRoomId, playerName);
        currentRoomId.clear();
        playerName.clear();
    }
    emit backToMenu();
}

void MultiPlayerLobby::onRoomSelectionChanged()
{
    QListWidgetItem* currentItem = roomList->currentItem();
    if (currentItem) {
        QString roomId = currentItem->data(Qt::UserRole).toString();
        GameRoom room = multiPlayerManager->getRoomInfo(roomId);
        updateRoomInfo(room);
        
        // 检查是否可以加入房间
        bool canJoin = !room.isGameStarted && room.currentPlayers < room.maxPlayers;
        joinRoomButton->setEnabled(canJoin && validatePlayerName());
    } else {
        clearRoomInfo();
        joinRoomButton->setEnabled(false);
    }
}

void MultiPlayerLobby::onPlayerNameChanged()
{
    bool isValid = validatePlayerName();
    createRoomButton->setEnabled(isValid);
    
    QListWidgetItem* currentItem = roomList->currentItem();
    if (currentItem) {
        QString roomId = currentItem->data(Qt::UserRole).toString();
        GameRoom room = multiPlayerManager->getRoomInfo(roomId);
        bool canJoin = !room.isGameStarted && room.currentPlayers < room.maxPlayers;
        joinRoomButton->setEnabled(canJoin && isValid);
    }
}

void MultiPlayerLobby::onRoomCreated(const QString& roomId, const GameRoom& room)
{
    Q_UNUSED(room)
    if (roomId == currentRoomId) {
        QMessageBox::information(this, "房间创建", QString("房间 %1 创建成功！等待其他玩家加入...").arg(roomId));
    }
    refreshRoomList();
}

void MultiPlayerLobby::onPlayerJoinedRoom(const QString& roomId, const QString& playerName)
{
    if (roomId == currentRoomId) {
        QMessageBox::information(this, "玩家加入", QString("玩家 %1 加入了房间！").arg(playerName));
        GameRoom room = multiPlayerManager->getRoomInfo(roomId);
        updateRoomInfo(room);
    }
    refreshRoomList();
}

void MultiPlayerLobby::onPlayerLeftRoom(const QString& roomId, const QString& playerName)
{
    if (roomId == currentRoomId) {
        QMessageBox::information(this, "玩家离开", QString("玩家 %1 离开了房间。").arg(playerName));
        GameRoom room = multiPlayerManager->getRoomInfo(roomId);
        updateRoomInfo(room);
    }
    refreshRoomList();
}

void MultiPlayerLobby::onGameStarted(const QString& roomId)
{
    if (roomId == currentRoomId) {
        QMessageBox::information(this, "游戏开始", "游戏即将开始！");
        
        // 设置游戏参数
        if (gameWidget) {
            gameWidget->setCurrentRoomId(currentRoomId);
            gameWidget->setPlayerName(playerName);
        }
        
        emit gameStarted();
    }
    refreshRoomList();
}

void MultiPlayerLobby::onGameEnded(const QString& roomId, const QString& winner)
{
    if (roomId == currentRoomId) {
        QString message;
        if (winner.isEmpty()) {
            message = "游戏结束！";
        } else {
            message = QString("游戏结束！获胜者: %1").arg(winner);
        }
        QMessageBox::information(this, "游戏结束", message);
        
        // 清理当前房间信息
        currentRoomId.clear();
        clearRoomInfo();
    }
    refreshRoomList();
}

void MultiPlayerLobby::onRoomDestroyed(const QString& roomId)
{
    if (roomId == currentRoomId) {
        QMessageBox::information(this, "房间关闭", "房间已被关闭。");
        currentRoomId.clear();
        clearRoomInfo();
    }
    refreshRoomList();
}

void MultiPlayerLobby::updateRoomInfo(const GameRoom& room)
{
    roomIdLabel->setText(QString("房间ID: %1").arg(room.roomId));
    roomNameLabel->setText(QString("房间名: %1").arg(room.hostName + "的房间"));
    playerCountLabel->setText(QString("玩家数量: %1/%2").arg(room.currentPlayers).arg(room.maxPlayers));
    
    QString status = room.isGameStarted ? "游戏中" : "等待中";
    roomStatusLabel->setText(QString("状态: %1").arg(status));
    
    // 更新玩家列表
    playerListWidget->clear();
    for (const QString& player : room.playerNames) {
        playerListWidget->addItem(player);
    }
}

void MultiPlayerLobby::clearRoomInfo()
{
    roomIdLabel->setText("房间ID: 未选择");
    roomNameLabel->setText("房间名: 未选择");
    playerCountLabel->setText("玩家数量: 0/0");
    roomStatusLabel->setText("状态: 未知");
    playerListWidget->clear();
}

bool MultiPlayerLobby::validatePlayerName() const
{
    QString name = playerNameEdit->text().trimmed();
    return !name.isEmpty() && name.length() >= 2 && name.length() <= 20;
}