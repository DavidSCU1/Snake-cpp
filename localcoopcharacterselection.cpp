#include "localcoopcharacterselection.h"
#include <QFont>
#include <QMap>

LocalCoopCharacterSelection::LocalCoopCharacterSelection(QWidget *parent)
    : QWidget(parent)
    , player1Character(CharacterType::SPONGEBOB)
    , player2Character(CharacterType::PATRICK)
    , player1Selected(false)
    , player2Selected(false)
{
    setupUI();
}

LocalCoopCharacterSelection::~LocalCoopCharacterSelection()
{
}

void LocalCoopCharacterSelection::setupUI()
{
    setStyleSheet("QWidget#localCoopCharacterSelectionWidget { background: transparent; }");
    setObjectName("localCoopCharacterSelectionWidget");
    
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // 标题
    titleLabel = new QLabel("本地双人游戏 - 角色选择", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("华文彩云", 24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #FF6347; margin: 20px;");
    mainLayout->addWidget(titleLabel);
    
    // 玩家选择容器
    QWidget* playersContainer = new QWidget(this);
    QHBoxLayout* playersLayout = new QHBoxLayout(playersContainer);
    playersLayout->setSpacing(40);
    
    // 玩家一选择区域
    player1Group = new QGroupBox("玩家一 (WASD控制)", playersContainer);
    player1Group->setStyleSheet(
        "QGroupBox { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #4169E1; "
        "    border: 2px solid #4169E1; "
        "    border-radius: 10px; "
        "    margin: 10px; "
        "    padding-top: 15px; "
        "}"
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 10px; "
        "    padding: 0 10px 0 10px; "
        "}"
    );
    QVBoxLayout* player1Layout = new QVBoxLayout(player1Group);
    player1Layout->setAlignment(Qt::AlignCenter);
    player1Layout->setSpacing(10);
    
    // 创建玩家一角色按钮
    QList<CharacterType> characters = {
        CharacterType::SPONGEBOB,
        CharacterType::PATRICK,
        CharacterType::SQUIDWARD,
        CharacterType::SANDY,
        CharacterType::MR_KRABS,
        CharacterType::PLANKTON
    };
    
    for (CharacterType character : characters) {
        createCharacterButton(player1Group, player1Layout, character, true);
    }
    
    playersLayout->addWidget(player1Group);
    
    // 玩家二选择区域
    player2Group = new QGroupBox("玩家二 (方向键控制)", playersContainer);
    player2Group->setStyleSheet(
        "QGroupBox { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #32CD32; "
        "    border: 2px solid #32CD32; "
        "    border-radius: 10px; "
        "    margin: 10px; "
        "    padding-top: 15px; "
        "}"
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 10px; "
        "    padding: 0 10px 0 10px; "
        "}"
    );
    QVBoxLayout* player2Layout = new QVBoxLayout(player2Group);
    player2Layout->setAlignment(Qt::AlignCenter);
    player2Layout->setSpacing(10);
    
    // 创建玩家二角色按钮
    for (CharacterType character : characters) {
        createCharacterButton(player2Group, player2Layout, character, false);
    }
    
    playersLayout->addWidget(player2Group);
    mainLayout->addWidget(playersContainer);
    
    // 按钮容器
    QWidget* buttonContainer = new QWidget(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(20);
    
    // 开始游戏按钮
    startGameButton = new QPushButton("🎮 开始游戏", buttonContainer);
    startGameButton->setFixedSize(150, 50);
    startGameButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #FF6347; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 10px; "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "    background-color: #FF4500; "
        "}"
        "QPushButton:pressed { "
        "    background-color: #DC143C; "
        "}"
        "QPushButton:disabled { "
        "    background-color: #CCCCCC; "
        "    color: #666666; "
        "}"
    );
    startGameButton->setEnabled(false);
    connect(startGameButton, &QPushButton::clicked, this, &LocalCoopCharacterSelection::onStartGameClicked);
    buttonLayout->addWidget(startGameButton);
    
    // 返回按钮
    backButton = new QPushButton("返回", buttonContainer);
    backButton->setFixedSize(100, 50);
    backButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #6C757D; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 10px; "
        "    font-size: 14px; "
        "}"
        "QPushButton:hover { "
        "    background-color: #5A6268; "
        "}"
    );
    connect(backButton, &QPushButton::clicked, this, &LocalCoopCharacterSelection::onBackClicked);
    buttonLayout->addWidget(backButton);
    
    mainLayout->addWidget(buttonContainer);
}

void LocalCoopCharacterSelection::createCharacterButton(QWidget* parent, QVBoxLayout* layout, CharacterType character, bool isPlayer1)
{
    QPushButton* button = new QPushButton(getCharacterEmoji(character) + " " + getCharacterName(character), parent);
    button->setFixedSize(180, 40);
    
    QString normalColor = isPlayer1 ? "#E6F3FF" : "#F0FFF0";
    QString selectedColor = isPlayer1 ? "#4169E1" : "#32CD32";
    QString hoverColor = isPlayer1 ? "#B0D4FF" : "#C0FFC0";
    
    button->setStyleSheet(
        QString("QPushButton { "
        "    background-color: %1; "
        "    border: 2px solid %2; "
        "    border-radius: 8px; "
        "    font-size: 14px; "
        "    padding: 5px; "
        "}"
        "QPushButton:hover { "
        "    background-color: %3; "
        "}").arg(normalColor, selectedColor, hoverColor)
    );
    
    if (isPlayer1) {
        player1Buttons[character] = button;
        connect(button, &QPushButton::clicked, [this, character]() {
            onPlayer1CharacterSelected(character);
        });
    } else {
        player2Buttons[character] = button;
        connect(button, &QPushButton::clicked, [this, character]() {
            onPlayer2CharacterSelected(character);
        });
    }
    
    layout->addWidget(button);
}

void LocalCoopCharacterSelection::onPlayer1CharacterSelected(CharacterType character)
{
    // 重置所有玩家一按钮样式
    for (auto it = player1Buttons.begin(); it != player1Buttons.end(); ++it) {
        it.value()->setStyleSheet(
            "QPushButton { "
            "    background-color: #E6F3FF; "
            "    border: 2px solid #4169E1; "
            "    border-radius: 8px; "
            "    font-size: 14px; "
            "    padding: 5px; "
            "}"
            "QPushButton:hover { "
            "    background-color: #B0D4FF; "
            "}"
        );
    }
    
    // 设置选中按钮样式
    player1Buttons[character]->setStyleSheet(
        "QPushButton { "
        "    background-color: #4169E1; "
        "    color: white; "
        "    border: 2px solid #4169E1; "
        "    border-radius: 8px; "
        "    font-size: 14px; "
        "    font-weight: bold; "
        "    padding: 5px; "
        "}"
    );
    
    player1Character = character;
    player1Selected = true;
    updateStartButtonState();
}

void LocalCoopCharacterSelection::onPlayer2CharacterSelected(CharacterType character)
{
    // 重置所有玩家二按钮样式
    for (auto it = player2Buttons.begin(); it != player2Buttons.end(); ++it) {
        it.value()->setStyleSheet(
            "QPushButton { "
            "    background-color: #F0FFF0; "
            "    border: 2px solid #32CD32; "
            "    border-radius: 8px; "
            "    font-size: 14px; "
            "    padding: 5px; "
            "}"
            "QPushButton:hover { "
            "    background-color: #C0FFC0; "
            "}"
        );
    }
    
    // 设置选中按钮样式
    player2Buttons[character]->setStyleSheet(
        "QPushButton { "
        "    background-color: #32CD32; "
        "    color: white; "
        "    border: 2px solid #32CD32; "
        "    border-radius: 8px; "
        "    font-size: 14px; "
        "    font-weight: bold; "
        "    padding: 5px; "
        "}"
    );
    
    player2Character = character;
    player2Selected = true;
    updateStartButtonState();
}

void LocalCoopCharacterSelection::onStartGameClicked()
{
    if (player1Selected && player2Selected) {
        emit startLocalCoopGame(player1Character, player2Character);
    }
}

void LocalCoopCharacterSelection::onBackClicked()
{
    emit backToModeSelection();
}

void LocalCoopCharacterSelection::updateStartButtonState()
{
    startGameButton->setEnabled(player1Selected && player2Selected);
}

QString LocalCoopCharacterSelection::getCharacterName(CharacterType character)
{
    switch (character) {
        case CharacterType::SPONGEBOB: return "海绵宝宝";
        case CharacterType::PATRICK: return "派大星";
        case CharacterType::SQUIDWARD: return "章鱼哥";
        case CharacterType::SANDY: return "珊迪";
        case CharacterType::MR_KRABS: return "蟹老板";
        case CharacterType::PLANKTON: return "痞老板";
        default: return "未知";
    }
}

QString LocalCoopCharacterSelection::getCharacterEmoji(CharacterType character)
{
    switch (character) {
        case CharacterType::SPONGEBOB: return "🧽";
        case CharacterType::PATRICK: return "⭐";
        case CharacterType::SQUIDWARD: return "🐙";
        case CharacterType::SANDY: return "🐿️";
        case CharacterType::MR_KRABS: return "🦀";
        case CharacterType::PLANKTON: return "🦠";
        default: return "❓";
    }
}