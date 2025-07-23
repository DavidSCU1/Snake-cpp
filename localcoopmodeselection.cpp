#include "localcoopmodeselection.h"
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <QShowEvent>
#include <QResizeEvent>
#include <QEasingCurve>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QDebug>
#include <QDialog>
#include <QMessageBox>

LocalCoopModeSelection::LocalCoopModeSelection(QWidget *parent)
    : QWidget(parent)
    , selectedMode(SinglePlayerMode::CLASSIC)
    , buttonAnimation(nullptr)
    , opacityEffect(nullptr)
{
    setupUI();
    setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "        stop:0 #1e3c72, stop:1 #2a5298); "
        "    color: white; "
        "    font-family: '华文彩云', Arial, sans-serif; "
        "}"
    );
}

LocalCoopModeSelection::~LocalCoopModeSelection()
{
}

void LocalCoopModeSelection::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    // 标题
    titleLabel = new QLabel("🎮 本地双人模式选择 🎮", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 32px; "
        "    font-weight: bold; "
        "    color: #FFD700; "
        "    margin: 20px; "
        "}"
    );
    mainLayout->addWidget(titleLabel);
    
    // 主要内容区域
    contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(50);
    
    setupModeButtons();
    setupDetailsPanel();
    
    mainLayout->addLayout(contentLayout);
    
    // 底部按钮
    setupBottomButtons();
    
    // 默认显示经典模式
    showModeDetails(SinglePlayerMode::CLASSIC);
}

void LocalCoopModeSelection::setupModeButtons()
{
    modeSelectionWidget = new QWidget(this);
    modeSelectionWidget->setFixedWidth(350);
    modeLayout = new QVBoxLayout(modeSelectionWidget);
    modeLayout->setSpacing(20);
    
    QLabel* modeTitle = new QLabel("选择游戏模式", modeSelectionWidget);
    modeTitle->setAlignment(Qt::AlignCenter);
    modeTitle->setStyleSheet(
        "QLabel { "
        "    font-size: 24px; "
        "    font-weight: bold; "
        "    color: #FFD700; "
        "    margin-bottom: 15px; "
        "}"
    );
    modeLayout->addWidget(modeTitle);
    
    // 创建模式按钮（只包含经典、时间挑战、挑战模式）
    classicButton = new QPushButton("🐍 经典模式", modeSelectionWidget);
    timeAttackButton = new QPushButton("⏰ 时间挑战", modeSelectionWidget);
    challengeButton = new QPushButton("🧱 挑战模式", modeSelectionWidget);
    
    QList<QPushButton*> buttons = {classicButton, timeAttackButton, challengeButton};
    QList<SinglePlayerMode> modes = {SinglePlayerMode::CLASSIC, SinglePlayerMode::TIME_ATTACK, 
                                    SinglePlayerMode::CHALLENGE};
    
    for (int i = 0; i < buttons.size(); ++i) {
        QPushButton* button = buttons[i];
        SinglePlayerMode mode = modes[i];
        
        button->setFixedSize(320, 80);
        button->setProperty("mode", static_cast<int>(mode));
        updateModeButton(button, mode);
        
        connect(button, &QPushButton::clicked, this, &LocalCoopModeSelection::onModeButtonClicked);
        connect(button, &QPushButton::clicked, [this, button]() { animateButton(button); });
        
        modeLayout->addWidget(button);
        modeButtons[mode] = button;
    }
    
    modeLayout->addStretch();
    contentLayout->addWidget(modeSelectionWidget);
}

void LocalCoopModeSelection::setupDetailsPanel()
{
    detailsWidget = new QWidget(this);
    detailsWidget->setFixedWidth(400);
    detailsLayout = new QVBoxLayout(detailsWidget);
    detailsLayout->setSpacing(25);
    
    // 模式名称
    modeNameLabel = new QLabel("经典模式", detailsWidget);
    modeNameLabel->setAlignment(Qt::AlignCenter);
    modeNameLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 28px; "
        "    font-weight: bold; "
        "    color: #FFD700; "
        "    margin: 15px; "
        "}"
    );
    detailsLayout->addWidget(modeNameLabel);
    
    // 模式描述
    modeDescriptionLabel = new QLabel(detailsWidget);
    modeDescriptionLabel->setWordWrap(true);
    modeDescriptionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    modeDescriptionLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 18px; "
        "    color: #E8E8E8; "
        "    background-color: rgba(255,255,255,0.1); "
        "    border-radius: 15px; "
        "    padding: 20px; "
        "    line-height: 1.6; "
        "    min-height: 200px; "
        "}"
    );
    detailsLayout->addWidget(modeDescriptionLabel);
    
    detailsLayout->addStretch();
    contentLayout->addWidget(detailsWidget);
}

void LocalCoopModeSelection::setupBottomButtons()
{
    bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(30);
    
    // 返回按钮
    backButton = new QPushButton("🔙 返回", this);
    backButton->setFixedSize(150, 60);
    backButton->setStyleSheet(
        "QPushButton { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #6C757D, stop:1 #495057); "
        "    border: none; "
        "    border-radius: 30px; "
        "    color: white; "
        "    font-size: 18px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #5A6268, stop:1 #343A40); "
        "} "
        "QPushButton:pressed { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #495057, stop:1 #212529); "
        "}"
    );
    connect(backButton, &QPushButton::clicked, this, &LocalCoopModeSelection::onBackClicked);
    
    // 开始游戏按钮
    startGameButton = new QPushButton("🚀 开始游戏", this);
    startGameButton->setFixedSize(200, 60);
    startGameButton->setStyleSheet(
        "QPushButton { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #FF6B6B, stop:1 #FF8E53); "
        "    border: none; "
        "    border-radius: 30px; "
        "    color: white; "
        "    font-size: 20px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #FF5252, stop:1 #FF7043); "
        "} "
        "QPushButton:pressed { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #E53935, stop:1 #FF5722); "
        "}"
    );
    connect(startGameButton, &QPushButton::clicked, this, &LocalCoopModeSelection::onStartGameClicked);
    
    bottomLayout->addStretch();
    bottomLayout->addWidget(backButton);
    bottomLayout->addWidget(startGameButton);
    bottomLayout->addStretch();
    
    mainLayout->addLayout(bottomLayout);
}

void LocalCoopModeSelection::updateModeButton(QPushButton* button, SinglePlayerMode mode)
{
    QString baseStyle = 
        "QPushButton { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 %1, stop:1 %2); "
        "    border: 3px solid %3; "
        "    border-radius: 15px; "
        "    color: white; "
        "    font-size: 20px; "
        "    font-weight: bold; "
        "    text-align: left; "
        "    padding-left: 20px; "
        "} "
        "QPushButton:hover { "
        "    border: 3px solid #FFD700; "
        "    transform: scale(1.02); "
        "} "
        "QPushButton:pressed { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 %4, stop:1 %5); "
        "}";
    
    QString color = getModeColor(mode);
    QString hoverColor = "#FFD700";
    QString pressedColor1, pressedColor2;
    
    if (mode == selectedMode) {
        button->setStyleSheet(baseStyle.arg("#FFD700", "#FFA500", "#FFD700", "#DAA520", "#FF8C00"));
    } else {
        if (color == "#4ECDC4") {
            pressedColor1 = "#45B7B8";
            pressedColor2 = "#3D9970";
        } else if (color == "#45B7B8") {
            pressedColor1 = "#3D9970";
            pressedColor2 = "#2ECC71";
        } else {
            pressedColor1 = "#E74C3C";
            pressedColor2 = "#C0392B";
        }
        
        button->setStyleSheet(baseStyle.arg(color, color, color, pressedColor1, pressedColor2));
    }
}

QString LocalCoopModeSelection::getModeColor(SinglePlayerMode mode) const
{
    switch (mode) {
    case SinglePlayerMode::CLASSIC:
        return "#4ECDC4";
    case SinglePlayerMode::TIME_ATTACK:
        return "#45B7B8";
    case SinglePlayerMode::CHALLENGE:
        return "#E74C3C";
    default:
        return "#4ECDC4";
    }
}

void LocalCoopModeSelection::showModeDetails(SinglePlayerMode mode)
{
    selectedMode = mode;
    
    QString modeName;
    QString description;
    
    switch (mode) {
    case SinglePlayerMode::CLASSIC:
        modeName = "🐍 经典模式";
        description = "传统的双人贪吃蛇游戏体验。\n\n• 两名玩家同时游戏\n• 吃食物增长身体\n• 避免撞墙、撞自己和对方\n• 比拼谁能生存更久\n• 适合新手练习和朋友对战";
        break;
    case SinglePlayerMode::TIME_ATTACK:
        modeName = "⏰ 时间挑战";
        description = "在限定时间内的双人竞速挑战！\n\n• 3分钟倒计时\n• 快速决策很重要\n• 特殊食物额外加分\n• 时间结束时分数高者获胜\n• 考验反应速度和策略";
        break;
    case SinglePlayerMode::CHALLENGE:
        modeName = "🧱 挑战模式";
        description = "每吃一个食物生成5块墙的双人挑战！\n\n• 食物被吃后生成墙体\n• 难度不断增加\n• 墙体会影响双方移动\n• 考验规划和适应能力\n• 极限生存双人挑战";
        break;
    default:
        modeName = "🐍 经典模式";
        description = "传统的双人贪吃蛇游戏体验。";
        break;
    }
    
    modeNameLabel->setText(modeName);
    modeDescriptionLabel->setText(description);
    
    // 更新所有按钮的样式
    for (auto it = modeButtons.begin(); it != modeButtons.end(); ++it) {
        updateModeButton(it.value(), it.key());
    }
}

void LocalCoopModeSelection::onModeButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        SinglePlayerMode mode = static_cast<SinglePlayerMode>(button->property("mode").toInt());
        showModeDetails(mode);
    }
}

void LocalCoopModeSelection::animateButton(QPushButton* button)
{
    if (buttonAnimation) {
        buttonAnimation->stop();
        delete buttonAnimation;
    }
    
    buttonAnimation = new QPropertyAnimation(button, "geometry");
    buttonAnimation->setDuration(100);
    buttonAnimation->setEasingCurve(QEasingCurve::OutBounce);
    
    QRect originalGeometry = button->geometry();
    QRect scaledGeometry = originalGeometry.adjusted(-5, -5, 5, 5);
    
    buttonAnimation->setStartValue(originalGeometry);
    buttonAnimation->setKeyValueAt(0.5, scaledGeometry);
    buttonAnimation->setEndValue(originalGeometry);
    
    buttonAnimation->start();
}

void LocalCoopModeSelection::onBackClicked()
{
    emit backToMenu();
}

void LocalCoopModeSelection::onStartGameClicked()
{
    emit modeSelected(selectedMode);
}