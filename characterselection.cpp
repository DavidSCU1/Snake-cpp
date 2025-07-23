#include "characterselection.h"
#include <QPainter>
#include <QFont>
#include <QEnterEvent>
#include <QDebug>
#include <QSvgRenderer>
#include <QBrush>
#include <QColor>
#include <QMessageBox>

// CharacterButton 实现
CharacterButton::CharacterButton(CharacterType character, QWidget *parent)
    : QPushButton(parent)
    , character(character)
    , hovered(false)
{
    loadCharacterInfo();
    setFixedSize(120, 150);
    setCheckable(true);
    setStyleSheet("QPushButton { border: 2px solid #333; border-radius: 10px; background-color: #f0f0f0; font-family: '华文彩云'; }"
                  "QPushButton:checked { border: 3px solid #ff6b35; background-color: #ffe0d6; }"
                  "QPushButton:hover { background-color: #e0e0e0; }");
}

void CharacterButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制角色图片
    if (!characterPixmap.isNull()) {
        QRect pixmapRect(10, 10, 100, 100);
        painter.drawPixmap(pixmapRect, characterPixmap);
    }
    
    // 绘制角色名称
    QFont font("华文彩云", 10);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(Qt::black);
    
    QRect textRect(0, 115, width(), 30);
    painter.drawText(textRect, Qt::AlignCenter, characterName);
}

void CharacterButton::enterEvent(QEnterEvent *event)
{
    hovered = true;
    QPushButton::enterEvent(event);
}

void CharacterButton::leaveEvent(QEvent *event)
{
    hovered = false;
    QPushButton::leaveEvent(event);
}

void CharacterButton::loadCharacterInfo()
{
    QString basePath = ":/images/";
    QString characterName_en;
    
    switch (character) {
    case CharacterType::SPONGEBOB:
        characterName = "海绵宝宝";
        characterName_en = "spongebob";
        break;
    case CharacterType::PATRICK:
        characterName = "派大星";
        characterName_en = "patrick";
        break;
    case CharacterType::SQUIDWARD:
        characterName = "章鱼哥";
        characterName_en = "squidward";
        break;
    case CharacterType::SANDY:
        characterName = "珊迪";
        characterName_en = "sandy";
        break;
    case CharacterType::MR_KRABS:
        characterName = "蟹老板";
        characterName_en = "mrcrabs";
        break;
    case CharacterType::PLANKTON:
        characterName = "痞老板";
        characterName_en = "plankton";
        break;
    }
    
    // 使用蛇头图片
    QString headPath = basePath + characterName_en + "_head.svg";
    QSvgRenderer headRenderer;
    headRenderer.load(headPath);
    
    if (headRenderer.isValid()) {
        characterPixmap = QPixmap(100, 100);
        characterPixmap.fill(Qt::transparent);
        QPainter painter(&characterPixmap);
        headRenderer.render(&painter);
    } else {
        // 如果蛇头SVG不存在，创建默认图片
        characterPixmap = QPixmap(100, 100);
        QPainter painter(&characterPixmap);
        
        // 根据角色类型设置不同颜色
        QColor color;
        switch (character) {
        case CharacterType::SPONGEBOB: color = Qt::yellow; break;
        case CharacterType::PATRICK: color = Qt::magenta; break;
        case CharacterType::SQUIDWARD: color = Qt::cyan; break;
        case CharacterType::SANDY: color = QColor(139, 69, 19); break;
        case CharacterType::MR_KRABS: color = Qt::red; break;
        case CharacterType::PLANKTON: color = Qt::green; break;
        }
        
        painter.fillRect(0, 0, 100, 100, color);
        painter.setPen(Qt::black);
        painter.drawRect(0, 0, 99, 99);
        
        // 绘制简单的表情
        painter.setBrush(Qt::black);
        painter.drawEllipse(25, 30, 10, 10); // 左眼
        painter.drawEllipse(65, 30, 10, 10); // 右眼
        painter.drawArc(30, 50, 40, 30, 0, 180 * 16); // 嘴巴
    }
}

// CharacterSelection 实现
CharacterSelection::CharacterSelection(QWidget *parent)
    : QWidget(parent)
    , selectedCharacter(CharacterType::SPONGEBOB)
    , selectedButton(nullptr)
    , playerReady(false)
    , isHost(false)
    , countdownTimer(new QTimer(this))
    , countdownValue(5)
{
    setupUI();
    
    // 设置倒计时定时器
    connect(countdownTimer, &QTimer::timeout, this, [this]() {
        countdownValue--;
        if (countdownValue > 0) {
            countdownLabel->setText(QString("游戏将在 %1 秒后开始").arg(countdownValue));
        } else {
            countdownTimer->stop();
            countdownLabel->hide();
            emit startGame();
        }
    });
}

void CharacterSelection::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(50, 30, 50, 30);
    
    // 标题
    titleLabel = new QLabel("选择你的角色", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("华文彩云", 24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #ff6b35; margin: 20px;");
    mainLayout->addWidget(titleLabel);
    
    // 角色选择区域
    QWidget* charactersWidget = new QWidget(this);
    charactersLayout = new QGridLayout(charactersWidget);
    charactersLayout->setSpacing(20);
    charactersLayout->setAlignment(Qt::AlignCenter);
    
    // 创建角色按钮
    QList<CharacterType> characters = {
        CharacterType::SPONGEBOB,
        CharacterType::PATRICK,
        CharacterType::SQUIDWARD,
        CharacterType::SANDY,
        CharacterType::MR_KRABS,
        CharacterType::PLANKTON
    };
    
    for (int i = 0; i < characters.size(); ++i) {
        CharacterButton* button = new CharacterButton(characters[i], this);
        connect(button, &QPushButton::clicked, this, &CharacterSelection::onCharacterButtonClicked);
        characterButtons.append(button);
        
        int row = i / 3;
        int col = i % 3;
        charactersLayout->addWidget(button, row, col);
        
        if (i == 0) {
            button->setChecked(true);
            selectedButton = button;
        }
    }
    
    mainLayout->addWidget(charactersWidget);
    
    // 玩家状态区域
    playersStatusLabel = new QLabel("玩家准备状态", this);
    playersStatusLabel->setAlignment(Qt::AlignCenter);
    QFont statusFont("华文彩云", 14);
    statusFont.setBold(true);
    playersStatusLabel->setFont(statusFont);
    playersStatusLabel->setStyleSheet("color: #333; margin: 10px;");
    mainLayout->addWidget(playersStatusLabel);
    
    playersListWidget = new QListWidget(this);
    playersListWidget->setMaximumHeight(120);
    playersListWidget->setStyleSheet("QListWidget { border: 2px solid #ddd; border-radius: 8px; padding: 5px; }");
    mainLayout->addWidget(playersListWidget);
    
    // 按钮区域
    QWidget* buttonsWidget = new QWidget(this);
    buttonsLayout = new QHBoxLayout(buttonsWidget);
    buttonsLayout->setSpacing(20);
    buttonsLayout->setAlignment(Qt::AlignCenter);
    
    backButton = new QPushButton("返回", this);
    backButton->setFixedSize(100, 40);
    backButton->setStyleSheet("QPushButton { background-color: #6c757d; color: white; border: none; border-radius: 5px; font-size: 14px; font-family: '华文彩云'; }"
                              "QPushButton:hover { background-color: #5a6268; }"
                              "QPushButton:pressed { background-color: #545b62; }");
    connect(backButton, &QPushButton::clicked, this, &CharacterSelection::onBackButtonClicked);
    
    readyButton = new QPushButton("准备", this);
    readyButton->setFixedSize(100, 40);
    readyButton->setStyleSheet("QPushButton { background-color: #ffc107; color: white; border: none; border-radius: 5px; font-size: 14px; font-family: '华文彩云'; }"
                               "QPushButton:hover { background-color: #e0a800; }"
                               "QPushButton:pressed { background-color: #d39e00; }");
    connect(readyButton, &QPushButton::clicked, this, [this]() {
        playerReady = !playerReady;
        if (!currentPlayerName.isEmpty()) {
            playerReadyStatus[currentPlayerName] = playerReady;
        }
        updateReadyButton();
        updatePlayerList();
        checkAllPlayersReady();
        emit playerReadyChanged(playerReady);
    });
    
    startButton = new QPushButton("开始游戏", this);
    startButton->setFixedSize(120, 40);
    startButton->setStyleSheet("QPushButton { background-color: #28a745; color: white; border: none; border-radius: 5px; font-size: 14px; font-family: '华文彩云'; }"
                               "QPushButton:hover { background-color: #218838; }"
                               "QPushButton:pressed { background-color: #1e7e34; }");
    startButton->setVisible(false); // 初始隐藏，只有房主在所有人准备后才显示
    connect(startButton, &QPushButton::clicked, this, &CharacterSelection::onStartButtonClicked);
    
    buttonsLayout->addWidget(backButton);
    buttonsLayout->addWidget(readyButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(startButton);
    
    mainLayout->addWidget(buttonsWidget);
    
    // 倒计时标签
    countdownLabel = new QLabel(this);
    countdownLabel->setAlignment(Qt::AlignCenter);
    QFont countdownFont("华文彩云", 18);
    countdownFont.setBold(true);
    countdownLabel->setFont(countdownFont);
    countdownLabel->setStyleSheet("color: #ff6b35; margin: 10px;");
    countdownLabel->hide();
    mainLayout->addWidget(countdownLabel);
    
    // 设置背景
    setStyleSheet("CharacterSelection { background-color: #f8f9fa; }");
}

void CharacterSelection::setSelectedCharacter(CharacterType character)
{
    selectedCharacter = character;
    updateSelection();
}

void CharacterSelection::onCharacterButtonClicked()
{
    CharacterButton* button = qobject_cast<CharacterButton*>(sender());
    if (!button) return;
    
    CharacterType newCharacter = button->getCharacter();
    
    // 检查角色是否已被其他玩家选择
    if (isCharacterTaken(newCharacter)) {
        // 角色已被占用，不允许选择
        QMessageBox::warning(this, "角色已被选择", "该角色已被其他玩家选择，请选择其他角色。");
        return;
    }
    
    // 取消之前的选择
    if (selectedButton) {
        selectedButton->setChecked(false);
    }
    
    // 设置新的选择
    button->setChecked(true);
    selectedButton = button;
    selectedCharacter = newCharacter;
    
    // 更新当前玩家的角色选择
    if (!currentPlayerName.isEmpty()) {
        playerCharacters[currentPlayerName] = selectedCharacter;
        updateCharacterAvailability();
        updatePlayerList();
    }
    
    emit characterSelected(selectedCharacter);
}

void CharacterSelection::onBackButtonClicked()
{
    emit backToMenu();
}

void CharacterSelection::onStartButtonClicked()
{
    // 开始倒计时
    countdownValue = 5;
    countdownLabel->setText(QString("游戏将在 %1 秒后开始").arg(countdownValue));
    countdownLabel->show();
    countdownTimer->start(1000);
    
    // 隐藏按钮
    readyButton->setVisible(false);
    startButton->setVisible(false);
}

void CharacterSelection::updateReadyButton()
{
    if (playerReady) {
        readyButton->setText("取消准备");
        readyButton->setStyleSheet("QPushButton { background-color: #dc3545; color: white; border: none; border-radius: 5px; font-size: 14px; font-family: '华文彩云'; }"
                                   "QPushButton:hover { background-color: #c82333; }"
                                   "QPushButton:pressed { background-color: #bd2130; }");
    } else {
        readyButton->setText("准备");
        readyButton->setStyleSheet("QPushButton { background-color: #ffc107; color: white; border: none; border-radius: 5px; font-size: 14px; font-family: '华文彩云'; }"
                                   "QPushButton:hover { background-color: #e0a800; }"
                                   "QPushButton:pressed { background-color: #d39e00; }");
    }
}

bool CharacterSelection::checkAllPlayersReady()
{
    bool allReady = true;
    for (auto it = playerReadyStatus.begin(); it != playerReadyStatus.end(); ++it) {
        if (!it.value()) {
            allReady = false;
            break;
        }
    }
    
    // 只有房主才能看到开始游戏按钮
    if (allReady && playerNames.size() > 1) {
        emit allPlayersReady();
    }
    
    return allReady;
}

void CharacterSelection::setPlayerReady(const QString& playerName, bool ready)
{
    playerReadyStatus[playerName] = ready;
    updatePlayerList();
    checkAllPlayersReady();
}

void CharacterSelection::updatePlayerList()
{
    playersListWidget->clear();
    
    for (const QString& playerName : playerNames) {
        QString status = playerReadyStatus.value(playerName, false) ? "已准备" : "未准备";
        QString characterName = "未选择";
        
        if (playerCharacters.contains(playerName)) {
            CharacterType charType = playerCharacters[playerName];
            switch (charType) {
                case CharacterType::SPONGEBOB:
                    characterName = "海绵宝宝";
                    break;
                case CharacterType::PATRICK:
                    characterName = "派大星";
                    break;
                case CharacterType::SQUIDWARD:
                    characterName = "章鱼哥";
                    break;
                case CharacterType::SANDY:
                    characterName = "松鼠珊迪";
                    break;
            }
        }
        
        QString itemText = QString("%1 - %2 - %3").arg(playerName).arg(characterName).arg(status);
        QListWidgetItem* item = new QListWidgetItem(itemText);
        
        if (playerReadyStatus.value(playerName, false)) {
            item->setBackground(QBrush(QColor("#d4edda")));
        } else {
            item->setBackground(QBrush(QColor("#f8d7da")));
        }
        
        playersListWidget->addItem(item);
    }
}

bool CharacterSelection::isPlayerReady(const QString& playerName) const
{
    return playerReadyStatus.value(playerName, false);
}

void CharacterSelection::updatePlayerCharacter(const QString& playerName, CharacterType character)
{
    playerCharacters[playerName] = character;
    updateCharacterAvailability();
    updatePlayerList();
}

void CharacterSelection::setPlayerNames(const QStringList& names)
{
    playerNames = names;
    // 初始化所有玩家的准备状态为false
    for (const QString& name : names) {
        if (!playerReadyStatus.contains(name)) {
            playerReadyStatus[name] = false;
        }
    }
    updateCharacterAvailability();
    updatePlayerList();
}

void CharacterSelection::showStartButton()
{
    startButton->setVisible(true);
}

void CharacterSelection::setIsHost(bool host)
{
    isHost = host;
    // 只有房主才能看到开始游戏按钮（在所有人准备后）
    if (!isHost) {
        startButton->setVisible(false);
    }
}

void CharacterSelection::setCurrentPlayerName(const QString& name)
{
    currentPlayerName = name;
    // 初始化当前玩家的状态
    if (!name.isEmpty()) {
        if (!playerReadyStatus.contains(name)) {
            playerReadyStatus[name] = false;
        }
        playerReady = playerReadyStatus[name];
        updateReadyButton();
    }
}

bool CharacterSelection::isCharacterTaken(CharacterType character) const
{
    // 检查是否有其他玩家已经选择了这个角色
    for (auto it = playerCharacters.begin(); it != playerCharacters.end(); ++it) {
        if (it.key() != currentPlayerName && it.value() == character) {
            return true;
        }
    }
    return false;
}

void CharacterSelection::updateCharacterAvailability()
{
    // 更新角色按钮的可用状态
    for (CharacterButton* button : characterButtons) {
        CharacterType character = button->getCharacter();
        bool isTaken = isCharacterTaken(character);
        
        // 如果角色被占用，禁用按钮并改变样式
        button->setEnabled(!isTaken);
        if (isTaken) {
            button->setStyleSheet("QPushButton { border: 2px solid #ccc; border-radius: 10px; background-color: #f5f5f5; color: #999; font-family: '华文彩云'; }");
        } else {
            button->setStyleSheet("QPushButton { border: 2px solid #333; border-radius: 10px; background-color: #f0f0f0; font-family: '华文彩云'; }"
                                  "QPushButton:checked { border: 3px solid #ff6b35; background-color: #ffe0d6; }"
                                  "QPushButton:hover { background-color: #e0e0e0; }");
        }
    }
}

void CharacterSelection::updateSelection()
{
    for (auto button : characterButtons) {
        if (button->getCharacter() == selectedCharacter) {
            if (selectedButton) {
                selectedButton->setChecked(false);
            }
            button->setChecked(true);
            selectedButton = button;
            break;
        }
    }
}