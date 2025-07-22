#include "singlemodeselection.h"
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <QShowEvent>
#include <QResizeEvent>
#include <QEasingCurve>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QDebug>

SingleModeSelection::SingleModeSelection(QWidget *parent)
    : QWidget(parent)
    , gameManager(nullptr)
    , selectedMode(SinglePlayerMode::CLASSIC)
    , selectedCharacter(CharacterType::SPONGEBOB)
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

SingleModeSelection::~SingleModeSelection()
{
}

void SingleModeSelection::setGameManager(SinglePlayerGameManager* manager)
{
    if (gameManager) {
        disconnect(gameManager, nullptr, this, nullptr);
    }
    
    gameManager = manager;
    
    if (gameManager) {
        connect(gameManager, &SinglePlayerGameManager::achievementUnlocked,
                this, &SingleModeSelection::onAchievementUnlocked);
        updateAchievements();
    }
}

void SingleModeSelection::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    // 标题
    titleLabel = new QLabel("🎮 选择游戏模式 🎮", this);
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
    contentLayout->setSpacing(30);
    
    setupModeButtons();
    setupDetailsPanel();
    setupCharacterSelection();
    setupAchievementPanel();
    
    mainLayout->addLayout(contentLayout);
    
    // 底部按钮
    bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(20);
    
    backButton = new QPushButton("🔙 返回主菜单", this);
    backButton->setFixedSize(150, 45);
    backButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #6C757D; "
        "    border: none; "
        "    border-radius: 8px; "
        "    color: white; "
        "    font-size: 14px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #5A6268; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #495057; "
        "}"
    );
    connect(backButton, &QPushButton::clicked, this, &SingleModeSelection::backToMenu);
    
    achievementButton = new QPushButton("🏆 查看成就", this);
    achievementButton->setFixedSize(150, 45);
    achievementButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #FFC107; "
        "    border: none; "
        "    border-radius: 8px; "
        "    color: #212529; "
        "    font-size: 14px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #FFB300; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #FFA000; "
        "}"
    );
    connect(achievementButton, &QPushButton::clicked, this, &SingleModeSelection::showAchievements);
    
    bottomLayout->addWidget(backButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(achievementButton);
    
    mainLayout->addLayout(bottomLayout);
}

void SingleModeSelection::setupModeButtons()
{
    modeSelectionWidget = new QWidget(this);
    modeSelectionWidget->setFixedWidth(300);
    modeLayout = new QVBoxLayout(modeSelectionWidget);
    modeLayout->setSpacing(15);
    
    QLabel* modeTitle = new QLabel("游戏模式", modeSelectionWidget);
    modeTitle->setAlignment(Qt::AlignCenter);
    modeTitle->setStyleSheet(
        "QLabel { "
        "    font-size: 20px; "
        "    font-weight: bold; "
        "    color: #FFD700; "
        "    margin-bottom: 10px; "
        "}"
    );
    modeLayout->addWidget(modeTitle);
    
    // 创建模式按钮
    classicButton = new QPushButton("🐍 经典模式", modeSelectionWidget);
    timeAttackButton = new QPushButton("⏰ 时间挑战", modeSelectionWidget);
    challengeButton = new QPushButton("🧱 挑战模式", modeSelectionWidget);
    speedRunButton = new QPushButton("⚡ 极速模式", modeSelectionWidget);
    aiBattleButton = new QPushButton("🤖 人机对战", modeSelectionWidget);
    
    QList<QPushButton*> buttons = {classicButton, timeAttackButton, challengeButton, speedRunButton, aiBattleButton};
    QList<SinglePlayerMode> modes = {SinglePlayerMode::CLASSIC, SinglePlayerMode::TIME_ATTACK, 
                                    SinglePlayerMode::CHALLENGE, SinglePlayerMode::SPEED_RUN, 
                                    SinglePlayerMode::AI_BATTLE};
    
    for (int i = 0; i < buttons.size(); ++i) {
        QPushButton* button = buttons[i];
        SinglePlayerMode mode = modes[i];
        
        button->setFixedSize(280, 60);
        button->setProperty("mode", static_cast<int>(mode));
        updateModeButton(button, mode);
        
        connect(button, &QPushButton::clicked, this, &SingleModeSelection::onModeButtonClicked);
        connect(button, &QPushButton::clicked, [this, button]() { animateButton(button); });
        
        modeLayout->addWidget(button);
        modeButtons[mode] = button;
    }
    
    modeLayout->addStretch();
    contentLayout->addWidget(modeSelectionWidget);
}

void SingleModeSelection::setupDetailsPanel()
{
    detailsWidget = new QWidget(this);
    detailsWidget->setFixedWidth(400);
    detailsLayout = new QVBoxLayout(detailsWidget);
    detailsLayout->setSpacing(20);
    
    // 模式名称
    modeNameLabel = new QLabel("经典模式", detailsWidget);
    modeNameLabel->setAlignment(Qt::AlignCenter);
    modeNameLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 24px; "
        "    font-weight: bold; "
        "    color: #FFD700; "
        "    margin: 10px; "
        "}"
    );
    detailsLayout->addWidget(modeNameLabel);
    
    // 模式描述
    modeDescriptionLabel = new QLabel(detailsWidget);
    modeDescriptionLabel->setWordWrap(true);
    modeDescriptionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    modeDescriptionLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 16px; "
        "    color: #E8E8E8; "
        "    background-color: rgba(255,255,255,0.1); "
        "    border-radius: 10px; "
        "    padding: 15px; "
        "    line-height: 1.5; "
        "}"
    );
    detailsLayout->addWidget(modeDescriptionLabel);
    
    // 统计信息
    modeStatsLabel = new QLabel(detailsWidget);
    modeStatsLabel->setWordWrap(true);
    modeStatsLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 14px; "
        "    color: #B8B8B8; "
        "    background-color: rgba(0,0,0,0.3); "
        "    border-radius: 8px; "
        "    padding: 10px; "
        "}"
    );
    detailsLayout->addWidget(modeStatsLabel);
    
    // 开始游戏按钮
    startModeButton = new QPushButton("🚀 开始游戏", detailsWidget);
    startModeButton->setFixedSize(200, 50);
    startModeButton->setStyleSheet(
        "QPushButton { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #FF6B6B, stop:1 #FF8E53); "
        "    border: none; "
        "    border-radius: 25px; "
        "    color: white; "
        "    font-size: 18px; "
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
    connect(startModeButton, &QPushButton::clicked, [this]() {
        emit modeSelected(selectedMode, selectedCharacter);
    });
    
    detailsLayout->addWidget(startModeButton, 0, Qt::AlignCenter);
    detailsLayout->addStretch();
    
    contentLayout->addWidget(detailsWidget);
    
    // 默认显示经典模式
    showModeDetails(SinglePlayerMode::CLASSIC);
}

void SingleModeSelection::setupCharacterSelection()
{
    characterSelectionWidget = new QWidget(this);
    characterSelectionWidget->setFixedWidth(300);
    characterLayout = new QVBoxLayout(characterSelectionWidget);
    characterLayout->setSpacing(15);
    
    characterTitleLabel = new QLabel("选择角色", characterSelectionWidget);
    characterTitleLabel->setAlignment(Qt::AlignCenter);
    characterTitleLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 20px; "
        "    font-weight: bold; "
        "    color: #FFD700; "
        "    margin-bottom: 10px; "
        "}"
    );
    characterLayout->addWidget(characterTitleLabel);
    
    // 创建角色按钮容器
    characterButtonContainer = new QWidget(characterSelectionWidget);
    characterButtonLayout = new QGridLayout(characterButtonContainer);
    characterButtonLayout->setSpacing(10);
    
    // 创建角色按钮
    spongebobButton = new QPushButton("🧽 海绵宝宝", characterButtonContainer);
    patrickButton = new QPushButton("⭐ 派大星", characterButtonContainer);
    squidwardButton = new QPushButton("🦑 章鱼哥", characterButtonContainer);
    sandyButton = new QPushButton("🐿️ 珊迪", characterButtonContainer);
    mrcrabsButton = new QPushButton("🦀 蟹老板", characterButtonContainer);
    planktonButton = new QPushButton("🦠 痞老板", characterButtonContainer);
    
    QList<QPushButton*> buttons = {spongebobButton, patrickButton, squidwardButton, sandyButton, mrcrabsButton, planktonButton};
    QList<CharacterType> characters = {CharacterType::SPONGEBOB, CharacterType::PATRICK, CharacterType::SQUIDWARD, 
         CharacterType::SANDY, CharacterType::MR_KRABS, CharacterType::PLANKTON};
    
    for (int i = 0; i < buttons.size(); ++i) {
        QPushButton* button = buttons[i];
        CharacterType character = characters[i];
        
        button->setFixedSize(130, 50);
        button->setProperty("character", static_cast<int>(character));
        updateCharacterButton(button, character);
        
        connect(button, &QPushButton::clicked, this, &SingleModeSelection::onCharacterButtonClicked);
        
        int row = i / 2;
        int col = i % 2;
        characterButtonLayout->addWidget(button, row, col);
        characterButtons[character] = button;
    }
    
    characterLayout->addWidget(characterButtonContainer);
    
    // 当前选择的角色显示
    selectedCharacterLabel = new QLabel("当前角色: 🧽 海绵宝宝", characterSelectionWidget);
    selectedCharacterLabel->setAlignment(Qt::AlignCenter);
    selectedCharacterLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #4CAF50; "
        "    background-color: rgba(255,255,255,0.1); "
        "    border-radius: 8px; "
        "    padding: 10px; "
        "    margin-top: 10px; "
        "}"
    );
    characterLayout->addWidget(selectedCharacterLabel);
    
    characterLayout->addStretch();
    contentLayout->addWidget(characterSelectionWidget);
    
    // 默认选择海绵宝宝
    updateCharacterButton(spongebobButton, CharacterType::SPONGEBOB);
}

void SingleModeSelection::setupAchievementPanel()
{
    achievementWidget = new QWidget(this);
    achievementWidget->setFixedWidth(250);
    achievementLayout = new QVBoxLayout(achievementWidget);
    achievementLayout->setSpacing(10);
    
    achievementTitleLabel = new QLabel("🏆 最近成就", achievementWidget);
    achievementTitleLabel->setAlignment(Qt::AlignCenter);
    achievementTitleLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 18px; "
        "    font-weight: bold; "
        "    color: #FFD700; "
        "    margin-bottom: 10px; "
        "}"
    );
    achievementLayout->addWidget(achievementTitleLabel);
    
    achievementScrollArea = new QScrollArea(achievementWidget);
    achievementScrollArea->setWidgetResizable(true);
    achievementScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    achievementScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    achievementScrollArea->setStyleSheet(
        "QScrollArea { "
        "    border: none; "
        "    background-color: rgba(255,255,255,0.05); "
        "    border-radius: 10px; "
        "}"
    );
    
    achievementContainer = new QWidget();
    achievementContainerLayout = new QVBoxLayout(achievementContainer);
    achievementContainerLayout->setSpacing(5);
    achievementContainerLayout->addStretch();
    
    achievementScrollArea->setWidget(achievementContainer);
    achievementLayout->addWidget(achievementScrollArea);
    
    contentLayout->addWidget(achievementWidget);
}

void SingleModeSelection::updateModeButton(QPushButton* button, SinglePlayerMode mode)
{
    QString baseStyle = 
        "QPushButton { "
        "    border: 2px solid %1; "
        "    border-radius: 15px; "
        "    background-color: rgba(255,255,255,0.1); "
        "    color: white; "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    text-align: left; "
        "    padding-left: 20px; "
        "} "
        "QPushButton:hover { "
        "    background-color: %1; "
        "} "
        "QPushButton:pressed { "
        "    background-color: %2; "
        "}";
    
    QString color = getModeColor(mode);
    QString hoverColor = color;
    hoverColor.replace("0.8", "0.6");
    
    button->setStyleSheet(baseStyle.arg(color, hoverColor));
}

QString SingleModeSelection::getModeColor(SinglePlayerMode mode) const
{
    switch (mode) {
    case SinglePlayerMode::CLASSIC:
        return "rgba(76, 175, 80, 0.8)";  // 绿色
    case SinglePlayerMode::TIME_ATTACK:
        return "rgba(255, 152, 0, 0.8)";   // 橙色
    case SinglePlayerMode::CHALLENGE:
        return "rgba(244, 67, 54, 0.8)";   // 红色
    case SinglePlayerMode::SPEED_RUN:
        return "rgba(156, 39, 176, 0.8)";  // 紫色
    case SinglePlayerMode::AI_BATTLE:
        return "rgba(255, 193, 7, 0.8)";   // 金色
    default:
        return "rgba(96, 125, 139, 0.8)"; // 灰色
    }
}

void SingleModeSelection::showModeDetails(SinglePlayerMode mode)
{
    selectedMode = mode;
    
    QString modeName;
    QString description;
    
    switch (mode) {
    case SinglePlayerMode::CLASSIC:
        modeName = "🐍 经典模式";
        description = "传统的贪吃蛇游戏体验。\n\n• 吃食物增长身体\n• 避免撞墙和自己\n• 分数越高越好\n• 适合新手练习";
        break;
    case SinglePlayerMode::TIME_ATTACK:
        modeName = "⏰ 时间挑战";
        description = "在限定时间内获得最高分数！\n\n• 3分钟倒计时\n• 快速决策很重要\n• 特殊食物额外加分\n• 考验反应速度";
        break;
    case SinglePlayerMode::CHALLENGE:
        modeName = "🧱 挑战模式";
        description = "每吃一个食物生成5块墙！\n\n• 食物被吃后生成墙体\n• 难度不断增加\n• 考验规划能力\n• 极限生存挑战";
        break;
    case SinglePlayerMode::SPEED_RUN:
        modeName = "⚡ 极速模式";
        description = "速度不断增加的挑战！\n\n• 游戏速度持续提升\n• 考验极限反应\n• 高风险高回报\n• 适合高手挑战";
        break;
    case SinglePlayerMode::AI_BATTLE:
        modeName = "🤖 人机对战";
        description = "与智能AI比拼积分！\n\n• AI自动寻路觅食\n• 比较双方得分\n• 考验策略技巧\n• 挑战人工智能";
        break;
    }
    
    modeNameLabel->setText(modeName);
    modeDescriptionLabel->setText(description);
    
    // 更新统计信息
    if (gameManager) {
        QString stats = "📊 模式统计:\n";
        // 这里可以添加从gameManager获取的统计信息
        stats += "• 最佳分数: 待实现\n";
        stats += "• 游戏次数: 待实现\n";
        stats += "• 完成成就: 待实现";
        modeStatsLabel->setText(stats);
    }
}

void SingleModeSelection::onModeButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        SinglePlayerMode mode = static_cast<SinglePlayerMode>(button->property("mode").toInt());
        showModeDetails(mode);
    }
}

void SingleModeSelection::animateButton(QPushButton* button)
{
    if (buttonAnimation) {
        buttonAnimation->stop();
        delete buttonAnimation;
    }
    
    buttonAnimation = new QPropertyAnimation(button, "geometry");
    buttonAnimation->setDuration(200);
    buttonAnimation->setEasingCurve(QEasingCurve::OutBounce);
    
    QRect startGeometry = button->geometry();
    QRect endGeometry = startGeometry;
    endGeometry.translate(10, 0);
    
    buttonAnimation->setStartValue(startGeometry);
    buttonAnimation->setEndValue(endGeometry);
    
    connect(buttonAnimation, &QPropertyAnimation::finished, [this, button, startGeometry]() {
        QPropertyAnimation* backAnimation = new QPropertyAnimation(button, "geometry");
        backAnimation->setDuration(200);
        backAnimation->setStartValue(button->geometry());
        backAnimation->setEndValue(startGeometry);
        backAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    });
    
    buttonAnimation->start();
}

void SingleModeSelection::updateAchievements()
{
    if (!gameManager) return;
    
    // 清除现有的成就显示
    for (QWidget* widget : achievementWidgets) {
        widget->deleteLater();
    }
    achievementWidgets.clear();
    
    // 获取最近解锁的成就
    QList<Achievement> unlockedAchievements = gameManager->getUnlockedAchievements();
    
    // 显示最近的5个成就
    int count = qMin(5, unlockedAchievements.size());
    for (int i = unlockedAchievements.size() - count; i < unlockedAchievements.size(); ++i) {
        AchievementWidget* achievementWidget = new AchievementWidget(unlockedAchievements[i], achievementContainer);
        achievementContainerLayout->insertWidget(achievementContainerLayout->count() - 1, achievementWidget);
        achievementWidgets.append(achievementWidget);
    }
}

void SingleModeSelection::onAchievementUnlocked(const Achievement& achievement)
{
    // 显示成就解锁动画或通知
    updateAchievements();
}

void SingleModeSelection::showAchievements()
{
    emit achievementsRequested();
}

void SingleModeSelection::onCharacterButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        CharacterType character = static_cast<CharacterType>(button->property("character").toInt());
        selectedCharacter = character;
        
        // 更新所有角色按钮的样式
        for (auto it = characterButtons.begin(); it != characterButtons.end(); ++it) {
            updateCharacterButton(it.value(), it.key());
        }
        
        // 更新选择的角色显示
        selectedCharacterLabel->setText(QString("当前角色: %1 %2")
                                       .arg(getCharacterIcon(character))
                                       .arg(getCharacterName(character)));
        
        emit characterSelected(character);
    }
}

void SingleModeSelection::updateCharacterButton(QPushButton* button, CharacterType character)
{
    bool isSelected = (character == selectedCharacter);
    
    QString baseStyle = 
        "QPushButton { "
        "    border: 2px solid %1; "
        "    border-radius: 10px; "
        "    background-color: %2; "
        "    color: white; "
        "    font-size: 14px; "
        "    font-weight: bold; "
        "    text-align: center; "
        "} "
        "QPushButton:hover { "
        "    background-color: %3; "
        "} "
        "QPushButton:pressed { "
        "    background-color: %4; "
        "}";
    
    QString borderColor = isSelected ? "#FFD700" : "#4CAF50";
    QString bgColor = isSelected ? "rgba(255, 215, 0, 0.3)" : "rgba(255,255,255,0.1)";
    QString hoverColor = isSelected ? "rgba(255, 215, 0, 0.5)" : "rgba(76, 175, 80, 0.3)";
    QString pressedColor = isSelected ? "rgba(255, 215, 0, 0.7)" : "rgba(76, 175, 80, 0.5)";
    
    button->setStyleSheet(baseStyle.arg(borderColor, bgColor, hoverColor, pressedColor));
}

QString SingleModeSelection::getCharacterName(CharacterType character) const
{
    switch (character) {
    case CharacterType::SPONGEBOB:
        return "海绵宝宝";
    case CharacterType::PATRICK:
        return "派大星";
    case CharacterType::SQUIDWARD:
        return "章鱼哥";
    case CharacterType::SANDY:
        return "珊迪";
    case CharacterType::MR_KRABS:
        return "蟹老板";
    case CharacterType::PLANKTON:
        return "痞老板";
    default:
        return "未知角色";
    }
}

QString SingleModeSelection::getCharacterIcon(CharacterType character) const
{
    switch (character) {
    case CharacterType::SPONGEBOB:
        return "🧽";
    case CharacterType::PATRICK:
        return "⭐";
    case CharacterType::SQUIDWARD:
        return "🦑";
    case CharacterType::SANDY:
        return "🐿️";
    case CharacterType::MR_KRABS:
        return "🦀";
    case CharacterType::PLANKTON:
        return "🦠";
    default:
        return "❓";
    }
}

void SingleModeSelection::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateAchievements();
}

void SingleModeSelection::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // 可以在这里添加响应式布局调整
}

// AchievementWidget 实现
AchievementWidget::AchievementWidget(const Achievement& achievement, QWidget* parent)
    : QFrame(parent)
    , achievementId(achievement.id)
{
    setupUI(achievement);
}

void AchievementWidget::setupUI(const Achievement& achievement)
{
    setFixedHeight(60);
    setStyleSheet(
        "QFrame { "
        "    background-color: rgba(255,255,255,0.1); "
        "    border-radius: 8px; "
        "    margin: 2px; "
        "} "
        "QFrame:hover { "
        "    background-color: rgba(255,255,255,0.2); "
        "}"
    );
    
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(10);
    
    // 成就图标
    iconLabel = new QLabel(achievement.unlocked ? "🏆" : "🔒", this);
    iconLabel->setFixedSize(30, 30);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 20px; font-family: '华文彩云';");
    layout->addWidget(iconLabel);
    
    // 文本信息
    textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);
    
    nameLabel = new QLabel(achievement.name, this);
    nameLabel->setStyleSheet(
        QString("QLabel { "
        "    font-size: 12px; "
        "    font-weight: bold; "
        "    color: %1; "
        "}").arg(achievement.unlocked ? "#FFD700" : "#888888")
    );
    textLayout->addWidget(nameLabel);
    
    descriptionLabel = new QLabel(achievement.description, this);
    descriptionLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 10px; "
        "    color: #CCCCCC; "
        "}"
    );
    textLayout->addWidget(descriptionLabel);
    
    layout->addLayout(textLayout);
    
    // 进度条（如果未解锁）
    if (!achievement.unlocked && achievement.target > 0) {
        progressBar = new QProgressBar(this);
        progressBar->setFixedSize(40, 10);
        progressBar->setMaximum(achievement.target);
        progressBar->setValue(achievement.progress);
        progressBar->setStyleSheet(
            "QProgressBar { "
            "    border: 1px solid #555; "
            "    border-radius: 5px; "
            "    background-color: #333; "
            "} "
            "QProgressBar::chunk { "
            "    background-color: #4CAF50; "
            "    border-radius: 4px; "
            "}"
        );
        layout->addWidget(progressBar);
    } else {
        progressBar = nullptr;
    }
}

void AchievementWidget::updateProgress(const Achievement& achievement)
{
    if (achievement.id != achievementId) return;
    
    iconLabel->setText(achievement.unlocked ? "🏆" : "🔒");
    nameLabel->setStyleSheet(
        QString("QLabel { "
        "    font-size: 12px; "
        "    font-weight: bold; "
        "    color: %1; "
        "}").arg(achievement.unlocked ? "#FFD700" : "#888888")
    );
    
    if (progressBar && !achievement.unlocked) {
        progressBar->setValue(achievement.progress);
    }
}