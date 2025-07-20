#include "multiplayerlobby.h"
#include <QDebug>
#include <QApplication>
#include <QRandomGenerator>

MultiPlayerLobby::MultiPlayerLobby(QWidget *parent)
    : QWidget(parent)
    , gameWidget(nullptr)
    , multiPlayerManager(new MultiPlayerGameManager(this))
    , isInRoom(false)
    , refreshTimer(new QTimer(this))
{
    setupUI();
    
    // 连接多人游戏管理器信号
    connect(multiPlayerManager, &MultiPlayerGameManager::roomCreated, this, &MultiPlayerLobby::onRoomCreated);
    connect(multiPlayerManager, &MultiPlayerGameManager::playerJoinedRoom, this, &MultiPlayerLobby::onPlayerJoinedRoom);
    connect(multiPlayerManager, &MultiPlayerGameManager::playerLeftRoom, this, &MultiPlayerLobby::onPlayerLeftRoom);
    connect(multiPlayerManager, &MultiPlayerGameManager::gameStarted, this, &MultiPlayerLobby::onGameStarted);
    connect(multiPlayerManager, &MultiPlayerGameManager::roomDestroyed, this, &MultiPlayerLobby::onRoomDestroyed);
    connect(multiPlayerManager, &MultiPlayerGameManager::gameEnded, this, &MultiPlayerLobby::onGameEnded);
    
    // 设置定时刷新
    refreshTimer->setInterval(5000); // 每5秒刷新一次房间列表
    connect(refreshTimer, &QTimer::timeout, this, &MultiPlayerLobby::refreshRoomList);
    refreshTimer->start();
    
    // 初始刷新房间列表
    refreshRoomList();
}

MultiPlayerLobby::~MultiPlayerLobby()
{
    if (isInRoom && !currentRoomId.isEmpty()) {
        multiPlayerManager->leaveRoom(currentRoomId, playerName);
    }
}

void MultiPlayerLobby::setupUI()
{
    setWindowTitle("多人游戏大厅");
    setMinimumSize(900, 600);
    
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // 顶部布局
    topLayout = new QHBoxLayout();
    topLayout->setSpacing(15);
    
    // 左侧 - 房间列表
    roomListGroup = new QGroupBox("可用房间", this);
    roomListGroup->setFixedWidth(250);
    roomListLayout = new QVBoxLayout(roomListGroup);
    
    roomListWidget = new QListWidget(roomListGroup);
    roomListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(roomListWidget, &QListWidget::itemSelectionChanged, this, &MultiPlayerLobby::onRoomSelectionChanged);
    roomListLayout->addWidget(roomListWidget);
    
    refreshButton = new QPushButton("刷新房间列表", roomListGroup);
    refreshButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 5px; padding: 8px; }"
                                "QPushButton:hover { background-color: #45a049; }");
    connect(refreshButton, &QPushButton::clicked, this, &MultiPlayerLobby::onRefreshClicked);
    roomListLayout->addWidget(refreshButton);
    
    topLayout->addWidget(roomListGroup);
    
    // 中间 - 房间信息
    roomInfoGroup = new QGroupBox("房间信息", this);
    roomInfoGroup->setFixedWidth(300);
    roomInfoLayout = new QVBoxLayout(roomInfoGroup);
    
    roomIdLabel = new QLabel("房间ID: 未选择", roomInfoGroup);
    roomIdLabel->setStyleSheet("font-weight: bold; color: #2196F3;");
    roomInfoLayout->addWidget(roomIdLabel);
    
    hostNameLabel = new QLabel("房主: 未知", roomInfoGroup);
    hostNameLabel->setStyleSheet("color: #FF9800;");
    roomInfoLayout->addWidget(hostNameLabel);
    
    playerCountLabel = new QLabel("玩家数量: 0/0", roomInfoGroup);
    playerCountLabel->setStyleSheet("color: #4CAF50;");
    roomInfoLayout->addWidget(playerCountLabel);
    
    gameStatusLabel = new QLabel("状态: 等待中", roomInfoGroup);
    gameStatusLabel->setStyleSheet("color: #9C27B0;");
    roomInfoLayout->addWidget(gameStatusLabel);
    
    QLabel* playersLabel = new QLabel("房间内玩家:", roomInfoGroup);
    playersLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    roomInfoLayout->addWidget(playersLabel);
    
    playersListWidget = new QListWidget(roomInfoGroup);
    playersListWidget->setMaximumHeight(150);
    roomInfoLayout->addWidget(playersListWidget);
    
    topLayout->addWidget(roomInfoGroup);
    
    // 右侧 - 操作面板
    actionGroup = new QGroupBox("操作", this);
    actionGroup->setFixedWidth(280);
    actionLayout = new QVBoxLayout(actionGroup);
    
    // 玩家名称输入
    playerNameLabel = new QLabel("玩家名称:", actionGroup);
    playerNameLabel->setStyleSheet("font-weight: bold;");
    actionLayout->addWidget(playerNameLabel);
    
    playerNameEdit = new QLineEdit(actionGroup);
    playerNameEdit->setPlaceholderText("请输入您的游戏名称");
    playerNameEdit->setText(QString("Player_%1").arg(QRandomGenerator::global()->bounded(1000, 9999)));
    playerNameEdit->setStyleSheet("padding: 8px; border: 2px solid #ddd; border-radius: 5px;");
    connect(playerNameEdit, &QLineEdit::textChanged, this, &MultiPlayerLobby::onPlayerNameChanged);
    actionLayout->addWidget(playerNameEdit);
    
    actionLayout->addSpacing(20);
    
    // 创建房间区域
    QLabel* createLabel = new QLabel("创建新房间:", actionGroup);
    createLabel->setStyleSheet("font-weight: bold; color: #2196F3;");
    actionLayout->addWidget(createLabel);
    
    maxPlayersLabel = new QLabel("最大玩家数:", actionGroup);
    actionLayout->addWidget(maxPlayersLabel);
    
    maxPlayersSpinBox = new QSpinBox(actionGroup);
    maxPlayersSpinBox->setRange(2, 8);
    maxPlayersSpinBox->setValue(4);
    maxPlayersSpinBox->setStyleSheet("padding: 5px; border: 2px solid #ddd; border-radius: 5px;");
    actionLayout->addWidget(maxPlayersSpinBox);
    
    createRoomButton = new QPushButton("创建房间", actionGroup);
    createRoomButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; border: none; border-radius: 5px; padding: 10px; font-weight: bold; }"
                                   "QPushButton:hover { background-color: #1976D2; }"
                                   "QPushButton:disabled { background-color: #ccc; }");
    connect(createRoomButton, &QPushButton::clicked, this, &MultiPlayerLobby::onCreateRoomClicked);
    actionLayout->addWidget(createRoomButton);
    
    actionLayout->addSpacing(20);
    
    // 加入房间区域
    QLabel* joinLabel = new QLabel("加入选中房间:", actionGroup);
    joinLabel->setStyleSheet("font-weight: bold; color: #4CAF50;");
    actionLayout->addWidget(joinLabel);
    
    joinRoomButton = new QPushButton("加入房间", actionGroup);
    joinRoomButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 5px; padding: 10px; font-weight: bold; }"
                                 "QPushButton:hover { background-color: #45a049; }"
                                 "QPushButton:disabled { background-color: #ccc; }");
    joinRoomButton->setEnabled(false);
    connect(joinRoomButton, &QPushButton::clicked, this, &MultiPlayerLobby::onJoinRoomClicked);
    actionLayout->addWidget(joinRoomButton);
    
    actionLayout->addStretch();
    
    topLayout->addWidget(actionGroup);
    
    mainLayout->addLayout(topLayout);
    
    // 底部按钮
    bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    
    backButton = new QPushButton("返回主菜单", this);
    backButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; border: none; border-radius: 5px; padding: 10px 20px; font-weight: bold; }"
                              "QPushButton:hover { background-color: #d32f2f; }");
    connect(backButton, &QPushButton::clicked, this, &MultiPlayerLobby::onBackClicked);
    bottomLayout->addWidget(backButton);
    
    mainLayout->addLayout(bottomLayout);
    
    // 初始状态
    clearRoomInfo();
    onPlayerNameChanged();
}

void MultiPlayerLobby::setGameWidget(GameWidget* gameWidget)
{
    this->gameWidget = gameWidget;
    if (gameWidget) {
        // 让GameWidget使用我们的多人游戏管理器，而不是相反
        gameWidget->setMultiPlayerManager(multiPlayerManager);
    }
}

void MultiPlayerLobby::refreshRoomList()
{
    if (!multiPlayerManager) {
        return;
    }
    
    QStringList availableRooms = multiPlayerManager->getAvailableRooms();
    
    roomListWidget->clear();
    
    for (const QString& roomId : availableRooms) {
        GameRoom room = multiPlayerManager->getRoomInfo(roomId);
        
        QString displayText = QString("%1 (%2/%3) - %4")
                                .arg(room.hostName + "的房间")
                                .arg(room.players.size())
                                .arg(room.maxPlayers)
                                .arg(room.isGameStarted ? "游戏中" : "等待中");
        
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, roomId);
        roomListWidget->addItem(item);
    }
    
    if (roomListWidget->count() == 0) {
        QListWidgetItem* emptyItem = new QListWidgetItem("暂无可用房间");
        emptyItem->setFlags(Qt::NoItemFlags);
        emptyItem->setTextAlignment(Qt::AlignCenter);
        roomListWidget->addItem(emptyItem);
    }
}

void MultiPlayerLobby::onCreateRoomClicked()
{
    if (!validatePlayerName()) return;
    
    playerName = playerNameEdit->text().trimmed();
    int maxPlayers = maxPlayersSpinBox->value();
    
    currentRoomId = multiPlayerManager->createRoom(playerName, maxPlayers);
    isInRoom = true;
    
    // 禁用创建和加入按钮
    createRoomButton->setEnabled(false);
    joinRoomButton->setEnabled(false);
    playerNameEdit->setEnabled(false);
}

void MultiPlayerLobby::onJoinRoomClicked()
{
    if (!validatePlayerName()) return;
    
    QListWidgetItem* selectedItem = roomListWidget->currentItem();
    if (!selectedItem || selectedItem->data(Qt::UserRole).toString().isEmpty()) {
        QMessageBox::warning(this, "错误", "请选择一个房间！");
        return;
    }
    
    QString roomId = selectedItem->data(Qt::UserRole).toString();
    playerName = playerNameEdit->text().trimmed();
    
    if (multiPlayerManager->joinRoom(roomId, playerName)) {
        currentRoomId = roomId;
        isInRoom = true;
        
        // 禁用创建和加入按钮
        createRoomButton->setEnabled(false);
        joinRoomButton->setEnabled(false);
        playerNameEdit->setEnabled(false);
        
        qDebug() << "Joined room:" << roomId;
    } else {
        QMessageBox::warning(this, "错误", "无法加入房间！房间可能已满或不存在。");
    }
}

void MultiPlayerLobby::onRefreshClicked()
{
    refreshRoomList();
}

void MultiPlayerLobby::onBackClicked()
{
    if (isInRoom && !currentRoomId.isEmpty()) {
        multiPlayerManager->leaveRoom(currentRoomId, playerName);
        isInRoom = false;
        currentRoomId.clear();
    }
    
    emit backToMenu();
}

void MultiPlayerLobby::onRoomSelectionChanged()
{
    QListWidgetItem* selectedItem = roomListWidget->currentItem();
    if (selectedItem && !selectedItem->data(Qt::UserRole).toString().isEmpty()) {
        QString roomId = selectedItem->data(Qt::UserRole).toString();
        GameRoom room = multiPlayerManager->getRoomInfo(roomId);
        updateRoomInfo(room);
        
        // 只有在不在房间内且玩家名有效时才能加入
        joinRoomButton->setEnabled(!isInRoom && validatePlayerName());
    } else {
        clearRoomInfo();
        joinRoomButton->setEnabled(false);
    }
}

void MultiPlayerLobby::onPlayerNameChanged()
{
    bool nameValid = validatePlayerName();
    createRoomButton->setEnabled(!isInRoom && nameValid);
    
    QListWidgetItem* selectedItem = roomListWidget->currentItem();
    bool roomSelected = selectedItem && !selectedItem->data(Qt::UserRole).toString().isEmpty();
    joinRoomButton->setEnabled(!isInRoom && nameValid && roomSelected);
}

void MultiPlayerLobby::onRoomCreated(const QString& roomId, const GameRoom& room)
{
    qDebug() << "Room created signal received:" << roomId;
    refreshRoomList();
    
    if (roomId == currentRoomId) {
        updateRoomInfo(room);
    }
}

void MultiPlayerLobby::onPlayerJoinedRoom(const QString& roomId, const QString& playerName)
{
    qDebug() << "Player joined room:" << playerName << "->" << roomId;
    refreshRoomList();
    
    if (roomId == currentRoomId) {
        GameRoom room = multiPlayerManager->getRoomInfo(roomId);
        updateRoomInfo(room);
    }
}

void MultiPlayerLobby::onPlayerLeftRoom(const QString& roomId, const QString& playerName)
{
    qDebug() << "Player left room:" << playerName << "<-" << roomId;
    refreshRoomList();
    
    if (roomId == currentRoomId) {
        GameRoom room = multiPlayerManager->getRoomInfo(roomId);
        if (room.roomId.isEmpty()) {
            // 房间已被销毁
            clearRoomInfo();
            isInRoom = false;
            currentRoomId.clear();
            
            // 重新启用按钮
            createRoomButton->setEnabled(true);
            joinRoomButton->setEnabled(false);
            playerNameEdit->setEnabled(true);
        } else {
            updateRoomInfo(room);
        }
    }
}

void MultiPlayerLobby::onGameStarted(const QString& roomId)
{
    if (roomId == currentRoomId && gameWidget) {
        qDebug() << "Game starting for room:" << roomId;
        
        // 设置GameWidget的房间信息
        gameWidget->setCurrentRoomId(roomId);
        gameWidget->setPlayerName(playerName);
        
        // 启动游戏
        emit gameStarted();
        
        // 隐藏大厅界面
        hide();
    }
}

void MultiPlayerLobby::onGameEnded(const QString& roomId, const QString& winner)
{
    if (roomId == currentRoomId) {
        qDebug() << "Game ended for room:" << roomId << "Winner:" << winner;
        
        // 游戏结束后重新启用按钮
        createRoomButton->setEnabled(true);
        joinRoomButton->setEnabled(true);
        playerNameEdit->setEnabled(true);
        
        // 清除房间状态
        isInRoom = false;
        currentRoomId.clear();
        
        // 刷新房间列表
        refreshRoomList();
        
        // 显示大厅界面
        show();
    }
}

void MultiPlayerLobby::onRoomDestroyed(const QString& roomId)
{
    qDebug() << "Room destroyed:" << roomId;
    refreshRoomList();
    
    if (roomId == currentRoomId) {
        clearRoomInfo();
        isInRoom = false;
        currentRoomId.clear();
        
        // 重新启用按钮
        createRoomButton->setEnabled(true);
        joinRoomButton->setEnabled(false);
        playerNameEdit->setEnabled(true);
        
        QMessageBox::information(this, "提示", "房间已被销毁。");
    }
}

void MultiPlayerLobby::updateRoomInfo(const GameRoom& room)
{
    roomIdLabel->setText(QString("房间ID: %1").arg(room.roomId));
    hostNameLabel->setText(QString("房主: %1").arg(room.hostName));
    playerCountLabel->setText(QString("玩家数量: %1/%2").arg(room.players.size()).arg(room.maxPlayers));
    gameStatusLabel->setText(QString("状态: %1").arg(room.isGameStarted ? "游戏中" : "等待中"));
    
    playersListWidget->clear();
    for (const QString& player : room.players) {
        QString displayText = player;
        if (player == room.hostName) {
            displayText += " (房主)";
        }
        playersListWidget->addItem(displayText);
    }
}

void MultiPlayerLobby::clearRoomInfo()
{
    roomIdLabel->setText("房间ID: 未选择");
    hostNameLabel->setText("房主: 未知");
    playerCountLabel->setText("玩家数量: 0/0");
    gameStatusLabel->setText("状态: 等待中");
    playersListWidget->clear();
}

bool MultiPlayerLobby::validatePlayerName() const
{
    QString name = playerNameEdit->text().trimmed();
    return !name.isEmpty() && name.length() >= 2 && name.length() <= 20;
}