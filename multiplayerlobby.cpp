#include "multiplayerlobby.h"
#include "gamewidget.h"
#include <QDebug>
#include <QHostInfo>
#include <QProcess>
#include <QCheckBox>

MultiPlayerLobby::MultiPlayerLobby(QWidget *parent)
    : QWidget(parent)
    , gameWidget(nullptr)
    , multiPlayerManager(new MultiPlayerGameManager(this))
    , refreshTimer(new QTimer(this))
{
    setupUI();
    
    // 初始化NetworkManager并设置给MultiPlayerGameManager
    NetworkManager* networkManager = new NetworkManager(this);
    multiPlayerManager->setNetworkManager(networkManager);
    
    // 连接NetworkManager的房间发现信号
    connect(networkManager, &NetworkManager::roomDiscovered, this, &MultiPlayerLobby::onRoomDiscovered);
    
    // 启动房间发现功能
    networkManager->startRoomDiscovery(12345);
    
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
    
    // 初始化房间清理定时器
    roomCleanupTimer = new QTimer(this);
    connect(roomCleanupTimer, &QTimer::timeout, this, [this]() {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        QList<QString> toRemove;
        for (auto it = discoveredRooms.begin(); it != discoveredRooms.end(); ++it) {
            // 超过5秒未收到广播则移除
            if (now - it.value() > 5000) {
                toRemove.append(it.key());
            }
        }
        for (const QString& key : toRemove) {
            discoveredRooms.remove(key);
            // 同步移除房间列表中的项
            for (int i = 0; i < roomList->count(); ++i) {
                QListWidgetItem* item = roomList->item(i);
                if (item->data(Qt::UserRole).toString() == key) {
                    delete roomList->takeItem(i);
                    break;
                }
            }
        }
    });
    roomCleanupTimer->start(1000); // 每秒检查一次
    
    // 初始化玩家名称
    playerName = playerNameEdit->text();
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
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
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
    QFont labelFont = roomListLabel->font();
    labelFont.setPointSize(12);
    labelFont.setBold(true);
    roomListLabel->setFont(labelFont);
    roomListLayout->addWidget(roomListLabel);
    
    roomList = new QListWidget(roomListWidget);
    roomList->setMinimumHeight(200);
    connect(roomList, &QListWidget::itemSelectionChanged, this, &MultiPlayerLobby::onRoomSelectionChanged);
    roomListLayout->addWidget(roomList);
    
    refreshButton = new QPushButton("刷新房间列表", roomListWidget);
    refreshButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; padding: 8px; border-radius: 4px; }"
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
    
    // 新增：手动输入IP和按钮
    manualIpEdit = new QLineEdit(playerSettingsGroup);
    manualIpEdit->setPlaceholderText("手动输入房主IP地址");
    manualConnectButton = new QPushButton("手动连接", playerSettingsGroup);
    manualConnectButton->setStyleSheet("QPushButton { background-color: #8BC34A; color: white; border: none; padding: 8px; border-radius: 4px; }QPushButton:hover { background-color: #689F38; }");
    connect(manualConnectButton, &QPushButton::clicked, this, &MultiPlayerLobby::onManualConnectClicked);
    playerSettingsLayout->addWidget(manualIpEdit);
    playerSettingsLayout->addWidget(manualConnectButton);
    
    // 最大玩家数设置
    QLabel* maxPlayersLabel = new QLabel("最大玩家数:", playerSettingsGroup);
    maxPlayersSpinBox = new QSpinBox(playerSettingsGroup);
    maxPlayersSpinBox->setMinimum(2);
    maxPlayersSpinBox->setMaximum(8);
    maxPlayersSpinBox->setValue(4); // 默认4人
    
    playerSettingsLayout->addWidget(maxPlayersLabel);
    playerSettingsLayout->addWidget(maxPlayersSpinBox);
    
    roomInfoLayout->addWidget(playerSettingsGroup);
    
    // 控制按钮
    buttonWidget = new QWidget(roomInfoWidget);
    buttonLayout = new QHBoxLayout(buttonWidget);
    
    createRoomButton = new QPushButton("创建房间", buttonWidget);
    createRoomButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; border: none; padding: 10px; border-radius: 4px; }"
                                   "QPushButton:hover { background-color: #1976D2; }"
                                   "QPushButton:disabled { background-color: #CCCCCC; }");
    connect(createRoomButton, &QPushButton::clicked, this, &MultiPlayerLobby::onCreateRoomClicked);
    
    joinRoomButton = new QPushButton("加入房间", buttonWidget);
    joinRoomButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; border: none; padding: 10px; border-radius: 4px; }"
                                 "QPushButton:hover { background-color: #F57C00; }"
                                 "QPushButton:disabled { background-color: #CCCCCC; }");
    joinRoomButton->setEnabled(false);
    connect(joinRoomButton, &QPushButton::clicked, this, &MultiPlayerLobby::onJoinRoomClicked);
    contentLayout->addWidget(joinRoomButton);

    // 勾选框状态变化时，控制加入按钮
    connect(allowJoinCheckBox, &QCheckBox::stateChanged, this, [this](int state){
        joinRoomButton->setEnabled(state == Qt::Checked);
    });
    
    buttonLayout->addWidget(createRoomButton);
    buttonLayout->addWidget(joinRoomButton);
    
    roomInfoLayout->addWidget(buttonWidget);
    
    // 返回按钮
    backButton = new QPushButton("返回主菜单", roomInfoWidget);
    backButton->setStyleSheet("QPushButton { background-color: #6C757D; color: white; border: none; padding: 10px; border-radius: 4px; }"
                             "QPushButton:hover { background-color: #5A6268; }");
    connect(backButton, &QPushButton::clicked, this, &MultiPlayerLobby::onBackClicked);
    roomInfoLayout->addWidget(backButton);
    
    contentLayout->addWidget(roomInfoWidget);
    
    mainLayout->addLayout(contentLayout);
    
    // 创建等待界面
    setupWaitingInterface();
    
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
        // 显示等待界面而不是弹窗
        showWaitingInterface();
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
        // 如果等待界面正在显示，更新等待界面
        if (waitingWidget && waitingWidget->isVisible()) {
            showWaitingInterface(); // 刷新等待界面信息
        } else {
            QMessageBox::information(this, "玩家加入", QString("玩家 %1 加入了房间！").arg(playerName));
            GameRoom room = multiPlayerManager->getRoomInfo(roomId);
            updateRoomInfo(room);
        }
    }
    refreshRoomList();
}

void MultiPlayerLobby::onPlayerLeftRoom(const QString& roomId, const QString& playerName)
{
    if (roomId == currentRoomId) {
        // 如果等待界面正在显示，更新等待界面
        if (waitingWidget && waitingWidget->isVisible()) {
            showWaitingInterface(); // 刷新等待界面信息
        } else {
            QMessageBox::information(this, "玩家离开", QString("玩家 %1 离开了房间。").arg(playerName));
            GameRoom room = multiPlayerManager->getRoomInfo(roomId);
            updateRoomInfo(room);
        }
    }
    refreshRoomList();
}

void MultiPlayerLobby::onGameStarted(const QString& roomId)
{
    if (roomId == currentRoomId) {
        // 隐藏等待界面
        if (waitingWidget && waitingWidget->isVisible()) {
            hideWaitingInterface();
        }
        
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
        // 隐藏等待界面
        if (waitingWidget && waitingWidget->isVisible()) {
            hideWaitingInterface();
        }
        
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

// 处理发现的房间
void MultiPlayerLobby::onRoomDiscovered(const QString& host, int port)
{
    QString roomKey = QString("%1:%2").arg(host).arg(port);
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    // 记录房间最后一次广播时间
    discoveredRooms[roomKey] = now;
    // 检查房间是否已存在于列表
    bool found = false;
    for (int i = 0; i < roomList->count(); ++i) {
        QListWidgetItem* item = roomList->item(i);
        if (item->data(Qt::UserRole).toString() == roomKey) {
            found = true;
            break;
        }
    }
    if (!found) {
        QString displayText = QString("发现服务器 %1:%2").arg(host).arg(port);
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, roomKey);
        item->setData(Qt::UserRole + 1, host);
        item->setData(Qt::UserRole + 2, port);
        roomList->addItem(item);
        qDebug() << "Room discovered:" << host << ":" << port;
    }
}

void MultiPlayerLobby::onManualConnectClicked()
{
    QString ip = manualIpEdit->text().trimmed();
    if (ip.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入房主的IP地址！");
        return;
    }
    // ping 检测
    QProcess pingProcess;
    QString pingCmd = QString("ping -n 1 %1").arg(ip);
    pingProcess.start(pingCmd);
    if (!pingProcess.waitForFinished(2000)) {
        QMessageBox::warning(this, "错误", "Ping 超时，无法连接到房主！");
        return;
    }
    QString output = pingProcess.readAllStandardOutput();
    if (!output.contains("TTL=")) {
        QMessageBox::warning(this, "错误", "Ping 失败，无法连接到房主！");
        return;
    }
    // 自动尝试端口区间
    NetworkManager* networkManager = multiPlayerManager ? multiPlayerManager->findChild<NetworkManager*>() : nullptr;
    if (networkManager) {
        bool connected = false;
        for (quint16 port = 12345; port < 12365; ++port) {
            QTcpSocket testSocket;
            testSocket.connectToHost(ip, port);
            if (testSocket.waitForConnected(300)) {
                testSocket.disconnectFromHost();
                networkManager->connectToServer(ip, port);
                QMessageBox::information(this, "提示", QString("已连接到房主（端口%1），请继续加入房间。\n如加入失败请重试。\n如多次失败请联系房主或检查防火墙。").arg(port));
                connected = true;
                break;
            }
        }
        if (!connected) {
            QMessageBox::warning(this, "错误", "未能自动检测到可用端口，可能房主未开启或网络异常。");
        }
    } else {
        QMessageBox::warning(this, "错误", "未找到网络管理器，无法连接！");
    }
}

// 设置等待界面
void MultiPlayerLobby::setupWaitingInterface()
{
    waitingWidget = new QWidget(this);
    waitingLayout = new QVBoxLayout(waitingWidget);
    waitingLayout->setContentsMargins(50, 50, 50, 50);
    waitingLayout->setSpacing(20);
    
    // 等待界面标题
    waitingTitleLabel = new QLabel("房间等待中", waitingWidget);
    waitingTitleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = waitingTitleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    waitingTitleLabel->setFont(titleFont);
    waitingTitleLabel->setStyleSheet("color: #FF6347; margin: 20px;");
    waitingLayout->addWidget(waitingTitleLabel);
    
    // 房间信息
    waitingRoomIdLabel = new QLabel("房间ID: ", waitingWidget);
    waitingRoomIdLabel->setAlignment(Qt::AlignCenter);
    QFont infoFont = waitingRoomIdLabel->font();
    infoFont.setPointSize(14);
    waitingRoomIdLabel->setFont(infoFont);
    waitingLayout->addWidget(waitingRoomIdLabel);
    
    waitingPlayerCountLabel = new QLabel("玩家数量: 1/4", waitingWidget);
    waitingPlayerCountLabel->setAlignment(Qt::AlignCenter);
    waitingPlayerCountLabel->setFont(infoFont);
    waitingLayout->addWidget(waitingPlayerCountLabel);
    
    // 玩家列表
    QLabel* playersLabel = new QLabel("房间内玩家:", waitingWidget);
    playersLabel->setAlignment(Qt::AlignCenter);
    playersLabel->setFont(infoFont);
    waitingLayout->addWidget(playersLabel);
    
    waitingPlayerListWidget = new QListWidget(waitingWidget);
    waitingPlayerListWidget->setMaximumHeight(150);
    waitingPlayerListWidget->setStyleSheet("QListWidget { border: 2px solid #ddd; border-radius: 8px; padding: 10px; }");
    waitingLayout->addWidget(waitingPlayerListWidget);
    
    // 按钮区域
    QWidget* waitingButtonWidget = new QWidget(waitingWidget);
    QHBoxLayout* waitingButtonLayout = new QHBoxLayout(waitingButtonWidget);
    waitingButtonLayout->setSpacing(20);
    
    startGameButton = new QPushButton("开始游戏", waitingButtonWidget);
    startGameButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; padding: 15px 30px; border-radius: 8px; font-size: 16px; }"
                                  "QPushButton:hover { background-color: #45a049; }"
                                  "QPushButton:disabled { background-color: #CCCCCC; }");
    startGameButton->setEnabled(false); // 默认禁用，需要至少2个玩家
    connect(startGameButton, &QPushButton::clicked, this, &MultiPlayerLobby::onStartGameClicked);
    
    leaveRoomButton = new QPushButton("离开房间", waitingButtonWidget);
    leaveRoomButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; border: none; padding: 15px 30px; border-radius: 8px; font-size: 16px; }"
                                  "QPushButton:hover { background-color: #da190b; }");
    connect(leaveRoomButton, &QPushButton::clicked, this, &MultiPlayerLobby::onLeaveRoomClicked);
    
    waitingButtonLayout->addWidget(startGameButton);
    waitingButtonLayout->addWidget(leaveRoomButton);
    waitingLayout->addWidget(waitingButtonWidget);
    
    // 添加到主布局但初始隐藏
    mainLayout->addWidget(waitingWidget);
    waitingWidget->hide();
}

// 显示等待界面
void MultiPlayerLobby::showWaitingInterface()
{
    // 隐藏主界面内容
    roomListWidget->hide();
    roomInfoWidget->hide();
    
    // 更新等待界面信息
    if (!currentRoomId.isEmpty()) {
        GameRoom room = multiPlayerManager->getRoomInfo(currentRoomId);
        waitingRoomIdLabel->setText(QString("房间ID: %1").arg(currentRoomId));
        waitingPlayerCountLabel->setText(QString("玩家数量: %1/%2").arg(room.currentPlayers).arg(room.maxPlayers));
        
        // 更新玩家列表
        waitingPlayerListWidget->clear();
        for (const QString& player : room.playerNames) {
            QListWidgetItem* item = new QListWidgetItem(player);
            if (player == playerName) {
                item->setForeground(QColor("#FF6347")); // 高亮自己
                item->setText(player + " (房主)");
            }
            waitingPlayerListWidget->addItem(item);
        }
        
        // 检查是否可以开始游戏（至少2个玩家）
        startGameButton->setEnabled(room.currentPlayers >= 2);
    }
    
    // 显示等待界面
    waitingWidget->show();
}

// 隐藏等待界面
void MultiPlayerLobby::hideWaitingInterface()
{
    waitingWidget->hide();
    roomListWidget->show();
    roomInfoWidget->show();
}

// 开始游戏按钮点击
void MultiPlayerLobby::onStartGameClicked()
{
    if (!currentRoomId.isEmpty()) {
        // 通知游戏管理器开始游戏
        multiPlayerManager->startGame(currentRoomId);
    }
}

// 离开房间按钮点击
void MultiPlayerLobby::onLeaveRoomClicked()
{
    if (!currentRoomId.isEmpty() && !playerName.isEmpty()) {
        multiPlayerManager->leaveRoom(currentRoomId, playerName);
        currentRoomId.clear();
        hideWaitingInterface();
        clearRoomInfo();
        refreshRoomList();
    }
}