#include "multiplayermodeselection.h"
#include <QFont>

MultiplayerModeSelection::MultiplayerModeSelection(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

MultiplayerModeSelection::~MultiplayerModeSelection()
{
}

void MultiplayerModeSelection::setupUI()
{
    setStyleSheet("QWidget#multiplayerModeSelectionWidget { background: transparent; }");
    setObjectName("multiplayerModeSelectionWidget");
    
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // 标题
    titleLabel = new QLabel("选择多人游戏模式", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("华文彩云", 28);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #FF6347; margin: 20px;");
    mainLayout->addWidget(titleLabel);
    
    // 按钮容器
    QWidget* buttonContainer = new QWidget(this);
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(20);
    
    // 本地联机按钮
    localCoopButton = new QPushButton("🎮 本地联机", buttonContainer);
    localCoopButton->setFixedSize(250, 60);
    localCoopButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #4169E1; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 15px; "
        "    font-size: 18px; "
        "    font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "    background-color: #1E90FF; "
        "}"
        "QPushButton:pressed { "
        "    background-color: #0000CD; "
        "}"
    );
    connect(localCoopButton, &QPushButton::clicked, this, &MultiplayerModeSelection::onLocalCoopClicked);
    buttonLayout->addWidget(localCoopButton);
    
    // 说明文字
    QLabel* localCoopDesc = new QLabel("双人在同一台电脑上游戏\n玩家一：WASD控制\n玩家二：方向键控制", buttonContainer);
    localCoopDesc->setAlignment(Qt::AlignCenter);
    localCoopDesc->setStyleSheet("color: #666666; font-size: 14px; margin: 10px;");
    buttonLayout->addWidget(localCoopDesc);
    
    // 局域网联机按钮
    networkButton = new QPushButton("🌐 局域网联机", buttonContainer);
    networkButton->setFixedSize(250, 60);
    networkButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #32CD32; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 15px; "
        "    font-size: 18px; "
        "    font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "    background-color: #00FF00; "
        "}"
        "QPushButton:pressed { "
        "    background-color: #228B22; "
        "}"
    );
    connect(networkButton, &QPushButton::clicked, this, &MultiplayerModeSelection::onNetworkClicked);
    buttonLayout->addWidget(networkButton);
    
    // 说明文字
    QLabel* networkDesc = new QLabel("通过网络与其他玩家游戏\n可创建房间或加入房间", buttonContainer);
    networkDesc->setAlignment(Qt::AlignCenter);
    networkDesc->setStyleSheet("color: #666666; font-size: 14px; margin: 10px;");
    buttonLayout->addWidget(networkDesc);
    
    mainLayout->addWidget(buttonContainer);
    
    // 返回按钮
    backButton = new QPushButton("返回主菜单", this);
    backButton->setFixedSize(150, 40);
    backButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #6C757D; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 8px; "
        "    font-size: 14px; "
        "}"
        "QPushButton:hover { "
        "    background-color: #5A6268; "
        "}"
    );
    connect(backButton, &QPushButton::clicked, this, &MultiplayerModeSelection::onBackClicked);
    mainLayout->addWidget(backButton, 0, Qt::AlignCenter);
}

void MultiplayerModeSelection::onLocalCoopClicked()
{
    emit modeSelected(MultiplayerMode::LOCAL_COOP);
}

void MultiplayerModeSelection::onNetworkClicked()
{
    emit modeSelected(MultiplayerMode::NETWORK);
}

void MultiplayerModeSelection::onBackClicked()
{
    emit backToMainMenu();
}