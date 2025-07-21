#include "localcoopcharacterselection.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QFont>
#include <QDebug>
#include <QPainter>
#include <QEnterEvent>
#include <QSvgRenderer>

// LocalCoopCharacterButton 实现
LocalCoopCharacterButton::LocalCoopCharacterButton(CharacterType character, QWidget *parent)
    : QPushButton(parent)
    , character(character)
    , hovered(false)
    , isDisabledCustom(false)
{
    loadCharacterInfo();
    setFixedSize(120, 150);
    setCheckable(true);
    setStyleSheet("QPushButton { border: 2px solid #333; border-radius: 10px; background-color: #f0f0f0; font-family: '华文彩云'; }"
                  "QPushButton:checked { border: 3px solid #ff6b35; background-color: #ffe0d6; }"
                  "QPushButton:hover { background-color: #e0e0e0; }");
}

void LocalCoopCharacterButton::setDisabled(bool disabled)
{
    isDisabledCustom = disabled;
    QPushButton::setEnabled(!disabled);
    if (disabled) {
        setStyleSheet("QPushButton { border: 2px solid #999; border-radius: 10px; background-color: #d0d0d0; color: #666; font-family: '华文彩云'; }");
    } else {
        setStyleSheet("QPushButton { border: 2px solid #333; border-radius: 10px; background-color: #f0f0f0; font-family: '华文彩云'; }"
                      "QPushButton:checked { border: 3px solid #ff6b35; background-color: #ffe0d6; }"
                      "QPushButton:hover { background-color: #e0e0e0; }");
    }
    update();
}

void LocalCoopCharacterButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 如果按钮被禁用，添加灰色遮罩
    if (isDisabledCustom) {
        painter.fillRect(rect(), QColor(128, 128, 128, 100));
    }
    
    // 绘制角色图片
    if (!characterPixmap.isNull()) {
        QRect pixmapRect(10, 10, 100, 100);
        if (isDisabledCustom) {
            // 绘制灰色版本的图片
            QPixmap grayPixmap = characterPixmap;
            QPainter pixmapPainter(&grayPixmap);
            pixmapPainter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            pixmapPainter.fillRect(grayPixmap.rect(), QColor(128, 128, 128, 150));
            painter.drawPixmap(pixmapRect, grayPixmap);
        } else {
            painter.drawPixmap(pixmapRect, characterPixmap);
        }
    }
    
    // 绘制角色名称
    QFont font("华文彩云", 10);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(isDisabledCustom ? Qt::gray : Qt::black);
    
    QRect textRect(0, 115, width(), 30);
    painter.drawText(textRect, Qt::AlignCenter, characterName);
}

void LocalCoopCharacterButton::enterEvent(QEnterEvent *event)
{
    if (!isDisabledCustom) {
        hovered = true;
        QPushButton::enterEvent(event);
    }
}

void LocalCoopCharacterButton::leaveEvent(QEvent *event)
{
    hovered = false;
    QPushButton::leaveEvent(event);
}

void LocalCoopCharacterButton::loadCharacterInfo()
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
        
        painter.fillRect(characterPixmap.rect(), color);
        painter.setPen(Qt::black);
        painter.drawRect(characterPixmap.rect().adjusted(0, 0, -1, -1));
    }
}

// LocalCoopCharacterSelection 实现
LocalCoopCharacterSelection::LocalCoopCharacterSelection(QWidget *parent)
    : QWidget(parent)
    , player1Character(CharacterType::SPONGEBOB)
    , player2Character(CharacterType::PATRICK)
    , isPlayer1Turn(true)
    , selectedButton(nullptr)
{
    setupUI();
    setupPlayer1Selection();
}

LocalCoopCharacterSelection::~LocalCoopCharacterSelection()
{
}

void LocalCoopCharacterSelection::setupUI()
{
    setStyleSheet("background-color: #2c3e50; color: white;");
    
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(100, 100, 100, 100);
    
    // 标题
    titleLabel = new QLabel("玩家1 - 选择角色", this);
    QFont titleFont("华文彩云", 24, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #ecf0f1; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel);
    
    // 角色选择区域
    characterGroup = new QGroupBox("选择你的角色", this);
    characterGroup->setStyleSheet("QGroupBox { font: bold 16px '华文彩云'; color: #3498db; border: 2px solid #3498db; border-radius: 10px; margin-top: 10px; padding-top: 10px; }"
                                 "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
    
    QGridLayout *characterLayout = new QGridLayout(characterGroup);
    characterLayout->setSpacing(30);
    characterLayout->setAlignment(Qt::AlignCenter);
    characterLayout->setContentsMargins(20, 20, 20, 20);
    
    // 创建角色按钮
    QList<CharacterType> characters = {CharacterType::SPONGEBOB, CharacterType::PATRICK, CharacterType::SQUIDWARD, CharacterType::SANDY, CharacterType::MR_KRABS, CharacterType::PLANKTON};
    
    int row = 0, col = 0;
    for (CharacterType character : characters) {
        LocalCoopCharacterButton *button = new LocalCoopCharacterButton(character, this);
        characterLayout->addWidget(button, row, col, Qt::AlignCenter);
        characterButtons[character] = button;
        
        connect(button, &QPushButton::clicked, [this, character]() {
            onCharacterButtonClicked(character);
        });
        
        col++;
        if (col >= 3) {
            col = 0;
            row++;
        }
    }
    
    // 设置角色组的最大宽度以实现居中效果
    characterGroup->setMaximumWidth(600);
    characterGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mainLayout->addWidget(characterGroup, 0, Qt::AlignCenter);
    
    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    
    backButton = new QPushButton("返回", this);
    backButton->setFixedSize(120, 40);
    backButton->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; border-radius: 5px; font: bold 14px '华文彩云'; }"
                              "QPushButton:hover { background-color: #7f8c8d; }"
                              "QPushButton:pressed { background-color: #6c7b7d; }");
    connect(backButton, &QPushButton::clicked, this, &LocalCoopCharacterSelection::backClicked);
    
    nextButton = new QPushButton("下一步", this);
    nextButton->setFixedSize(120, 40);
    nextButton->setEnabled(false);
    nextButton->setStyleSheet("QPushButton { background-color: #27ae60; color: white; border: none; border-radius: 5px; font: bold 14px '华文彩云'; }"
                              "QPushButton:hover:enabled { background-color: #229954; }"
                              "QPushButton:pressed:enabled { background-color: #1e8449; }"
                              "QPushButton:disabled { background-color: #7f8c8d; }");
    connect(nextButton, &QPushButton::clicked, this, &LocalCoopCharacterSelection::onNextClicked);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(nextButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
}

void LocalCoopCharacterSelection::setupPlayer1Selection()
{
    titleLabel->setText("玩家1 - 选择角色");
    characterGroup->setStyleSheet("QGroupBox { font: bold 16px '华文彩云'; color: #3498db; border: 2px solid #3498db; border-radius: 10px; margin-top: 10px; padding-top: 10px; }"
                                 "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
    nextButton->setText("下一步");
    nextButton->setEnabled(false);
    
    // 重置所有按钮状态
    for (auto it = characterButtons.begin(); it != characterButtons.end(); ++it) {
        it.value()->setDisabled(false);
        it.value()->setChecked(false);
    }
    
    selectedButton = nullptr;
    isPlayer1Turn = true;
}

void LocalCoopCharacterSelection::setupPlayer2Selection()
{
    titleLabel->setText("玩家2 - 选择角色");
    characterGroup->setStyleSheet("QGroupBox { font: bold 16px '华文彩云'; color: #e74c3c; border: 2px solid #e74c3c; border-radius: 10px; margin-top: 10px; padding-top: 10px; }"
                                 "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
    nextButton->setText("开始游戏");
    nextButton->setEnabled(false);
    
    // 禁用玩家1已选择的角色
    for (auto it = characterButtons.begin(); it != characterButtons.end(); ++it) {
        if (it.key() == player1Character) {
            it.value()->setDisabled(true);
            it.value()->setChecked(false);
        } else {
            it.value()->setDisabled(false);
            it.value()->setChecked(false);
        }
    }
    
    selectedButton = nullptr;
    isPlayer1Turn = false;
}

void LocalCoopCharacterSelection::onCharacterButtonClicked(CharacterType character)
{
    // 取消之前选中的按钮
    if (selectedButton) {
        selectedButton->setChecked(false);
    }
    
    // 设置新选中的按钮
    selectedButton = characterButtons[character];
    selectedButton->setChecked(true);
    
    if (isPlayer1Turn) {
        player1Character = character;
    } else {
        player2Character = character;
    }
    
    nextButton->setEnabled(true);
}

void LocalCoopCharacterSelection::onNextClicked()
{
    if (isPlayer1Turn) {
        // 玩家1选择完成，切换到玩家2
        setupPlayer2Selection();
    } else {
        // 玩家2选择完成，开始游戏
        emit startLocalCoopGame(player1Character, player2Character);
    }
}

CharacterType LocalCoopCharacterSelection::getPlayer1Character() const
{
    return player1Character;
}

CharacterType LocalCoopCharacterSelection::getPlayer2Character() const
{
    return player2Character;
}

void LocalCoopCharacterSelection::resetSelection()
{
    // 重置选择状态
    isPlayer1Turn = true;
    selectedButton = nullptr;
    player1Character = CharacterType::SPONGEBOB;
    player2Character = CharacterType::PATRICK;
    
    // 重新设置为玩家1选择界面
    setupPlayer1Selection();
}