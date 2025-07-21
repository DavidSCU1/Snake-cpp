#include "characterselection.h"
#include <QPainter>
#include <QFont>
#include <QEnterEvent>
#include <QDebug>

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
    
    switch (character) {
    case CharacterType::SPONGEBOB:
        characterName = "海绵宝宝";
        characterPixmap = QPixmap(basePath + "spongebob_portrait.png");
        break;
    case CharacterType::PATRICK:
        characterName = "派大星";
        characterPixmap = QPixmap(basePath + "patrick_portrait.png");
        break;
    case CharacterType::SQUIDWARD:
        characterName = "章鱼哥";
        characterPixmap = QPixmap(basePath + "squidward_portrait.png");
        break;
    case CharacterType::SANDY:
        characterName = "珊迪";
        characterPixmap = QPixmap(basePath + "sandy_portrait.png");
        break;
    case CharacterType::MR_KRABS:
        characterName = "蟹老板";
        characterPixmap = QPixmap(basePath + "mrkribs_portrait.png");
        break;
    case CharacterType::PLANKTON:
        characterName = "痞老板";
        characterPixmap = QPixmap(basePath + "plankton_portrait.png");
        break;
    }
    
    // 如果资源文件不存在，创建默认图片
    if (characterPixmap.isNull()) {
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
{
    setupUI();
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
    
    startButton = new QPushButton("开始游戏", this);
    startButton->setFixedSize(120, 40);
    startButton->setStyleSheet("QPushButton { background-color: #28a745; color: white; border: none; border-radius: 5px; font-size: 14px; font-family: '华文彩云'; }"
                               "QPushButton:hover { background-color: #218838; }"
                               "QPushButton:pressed { background-color: #1e7e34; }");
    connect(startButton, &QPushButton::clicked, this, &CharacterSelection::onStartButtonClicked);
    
    buttonsLayout->addWidget(backButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(startButton);
    
    mainLayout->addWidget(buttonsWidget);
    
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
    
    // 取消之前的选择
    if (selectedButton) {
        selectedButton->setChecked(false);
    }
    
    // 设置新的选择
    button->setChecked(true);
    selectedButton = button;
    selectedCharacter = button->getCharacter();
    
    emit characterSelected(selectedCharacter);
}

void CharacterSelection::onBackButtonClicked()
{
    emit backToMenu();
}

void CharacterSelection::onStartButtonClicked()
{
    emit startGame();
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