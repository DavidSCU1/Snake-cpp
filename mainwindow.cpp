#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>
#include <QApplication>
#include <QSettings>
#include <QHostAddress>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , selectedCharacter(CharacterType::SPONGEBOB)
    , selectedDifficulty(Difficulty::NORMAL)
    , isMultiplayerHost(false)
    , oceanBackground(nullptr)
{
    qDebug() << "MainWindow constructor started";
    ui->setupUi(this);
    qDebug() << "UI setup completed";
    setupUI();
    qDebug() << "Custom UI setup completed";
    
    setWindowTitle("海绵宝宝贪吃蛇");
    setMinimumSize(1000, 700);
    resize(1200, 800);
    
    // 显示主菜单
    showMainMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 创建海洋背景
    oceanBackground = new OceanBackground(this);
    oceanBackground->setGeometry(0, 0, width(), height());
    
    // 创建堆叠窗口部件
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setStyleSheet("QStackedWidget { background: transparent; }");
    setCentralWidget(stackedWidget);
    
    // 确保背景在最底层
    oceanBackground->lower();
    stackedWidget->raise();
    
    // 创建各个页面
    setupMainMenu();
    setupDifficultyMenu();
    setupMultiplayerMenu();
    setupHighScoresMenu();
    
    // 创建角色选择页面
    characterSelection = new CharacterSelection(this);
    connect(characterSelection, &CharacterSelection::characterSelected, this, &MainWindow::onCharacterSelected);
    connect(characterSelection, &CharacterSelection::backToMenu, this, &MainWindow::showMainMenu);
    connect(characterSelection, &CharacterSelection::startGame, this, &MainWindow::showDifficultySelection);
    stackedWidget->addWidget(characterSelection);
    
    // 创建准备界面
    preparationWidget = new QWidget(this);
    QVBoxLayout* prepLayout = new QVBoxLayout(preparationWidget);
    QLabel* prepLabel = new QLabel("准备界面 - 等待游戏开始...", preparationWidget);
    prepLabel->setAlignment(Qt::AlignCenter);
    prepLabel->setStyleSheet("font-size: 24px; color: #FF6347;");
    prepLayout->addWidget(prepLabel);
    stackedWidget->addWidget(preparationWidget);

    // 创建游戏页面
    gameWidget = new GameWidget(this);
    connect(gameWidget, &GameWidget::gameOver, this, &MainWindow::onGameOver);
    connect(gameWidget, &GameWidget::backToMenu, this, &MainWindow::showMainMenu);
    stackedWidget->addWidget(gameWidget);
    

    
    // 创建单人游戏管理器
    singlePlayerGameManager = new SinglePlayerGameManager(this);
    
    // 创建单人模式选择界面
    singleModeSelection = new SingleModeSelection(this);
    singleModeSelection->setGameManager(singlePlayerGameManager);
    connect(singleModeSelection, QOverload<SinglePlayerMode, CharacterType>::of(&SingleModeSelection::modeSelected), this, &MainWindow::onSingleModeSelected);
    connect(singleModeSelection, &SingleModeSelection::backToMenu, this, &MainWindow::showMainMenu);
    stackedWidget->addWidget(singleModeSelection);
    

    
    // 创建本地双人角色选择界面
    localCoopCharacterSelection = new LocalCoopCharacterSelection(this);
    connect(localCoopCharacterSelection, &LocalCoopCharacterSelection::startLocalCoopGame, this, &MainWindow::onLocalCoopCharactersSelected);
    connect(localCoopCharacterSelection, &LocalCoopCharacterSelection::backClicked, this, &MainWindow::showMainMenu);
    stackedWidget->addWidget(localCoopCharacterSelection);
    
    // 创建热点网络管理器
    hotspotNetworkManager = new HotspotNetworkManager(this);
    connect(hotspotNetworkManager, &HotspotNetworkManager::networkError, this, &MainWindow::onHotspotNetworkError);
    
    // 创建热点游戏管理器
    hotspotGameManager = new HotspotGameManager(this);
    hotspotGameManager->setNetworkManager(hotspotNetworkManager);
    
    // 创建热点大厅
    hotspotLobby = new HotspotLobby(this);
    hotspotLobby->setNetworkManager(hotspotNetworkManager);
    hotspotLobby->setGameManager(hotspotGameManager);
    // 设置gameWidget的热点游戏管理器
    gameWidget->setHotspotGameManager(hotspotGameManager);
    
    // 热点大厅相关信号连接已简化
    stackedWidget->addWidget(hotspotLobby);
    
    // 设置样式
    setStyleSheet(
        "QMainWindow { background: transparent; }"
        "QPushButton { "
        "    background-color: #4169E1; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 8px; "
        "    padding: 12px 24px; "
        "    font-size: 14px; "
        "    font-weight: bold; "
        "    font-family: '华文彩云'; "
        "}"
        "QPushButton:hover { background-color: #6495ED; }"
        "QPushButton:pressed { background-color: #0000CD; }"
        "QLabel { color: #2F4F4F; }"
    );
}

void MainWindow::setupMainMenu()
{
    mainMenuWidget = new QWidget(this);
    mainMenuWidget->setStyleSheet("QWidget#mainMenuWidget { background: transparent; }");
    mainMenuWidget->setObjectName("mainMenuWidget");
    QVBoxLayout* layout = new QVBoxLayout(mainMenuWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);
    
    // 标题
    titleLabel = new QLabel("🧽 海绵宝宝贪吃蛇 🧽", mainMenuWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("华文彩云", 48);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #FF6347; margin: 30px;");
    layout->addWidget(titleLabel);
    
    // 按钮容器
    QWidget* buttonContainer = new QWidget(mainMenuWidget);
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(20);
    
    // 单人游戏按钮
    singlePlayerButton = new QPushButton("🎮 单人游戏", buttonContainer);
    singlePlayerButton->setFixedSize(200, 50);
    connect(singlePlayerButton, &QPushButton::clicked, this, &MainWindow::showSingleModeSelection);
    buttonLayout->addWidget(singlePlayerButton);
    
    // 多人游戏按钮
    multiplayerButton = new QPushButton("👥 多人游戏", buttonContainer);
    multiplayerButton->setFixedSize(200, 50);
    connect(multiplayerButton, &QPushButton::clicked, this, &MainWindow::showMultiplayerMenu);
    buttonLayout->addWidget(multiplayerButton);
    
    // 高分榜按钮
    highScoresButton = new QPushButton("🏆 高分榜", buttonContainer);
    highScoresButton->setFixedSize(200, 50);
    connect(highScoresButton, &QPushButton::clicked, this, &MainWindow::showHighScores);
    buttonLayout->addWidget(highScoresButton);
    
    // 退出按钮
    exitButton = new QPushButton("❌ 退出游戏", buttonContainer);
    exitButton->setFixedSize(200, 50);
    exitButton->setStyleSheet("QPushButton { background-color: #DC143C; }"
                              "QPushButton:hover { background-color: #B22222; }"
                              "QPushButton:pressed { background-color: #8B0000; }");
    connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);
    buttonLayout->addWidget(exitButton);
    
    layout->addWidget(buttonContainer);
    
    stackedWidget->addWidget(mainMenuWidget);
}

void MainWindow::setupDifficultyMenu()
{
    difficultyMenuWidget = new QWidget(this);
    difficultyMenuWidget->setStyleSheet("QWidget#difficultyMenuWidget { background: transparent; }");
    difficultyMenuWidget->setObjectName("difficultyMenuWidget");
    QVBoxLayout* layout = new QVBoxLayout(difficultyMenuWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);
    
    // 标题
    difficultyLabel = new QLabel("选择难度", difficultyMenuWidget);
    difficultyLabel->setAlignment(Qt::AlignCenter);
    QFont font("华文彩云", 24);
    font.setBold(true);
    difficultyLabel->setFont(font);
    difficultyLabel->setStyleSheet("color: #FF6347; margin: 20px;");
    layout->addWidget(difficultyLabel);
    
    // 难度选择
    difficultyComboBox = new QComboBox(difficultyMenuWidget);
    difficultyComboBox->addItem("🐌 简单 (慢速)", static_cast<int>(Difficulty::EASY));
    difficultyComboBox->addItem("🚶 普通 (中速)", static_cast<int>(Difficulty::NORMAL));
    difficultyComboBox->addItem("🏃 困难 (快速)", static_cast<int>(Difficulty::HARD));
    difficultyComboBox->setCurrentIndex(1); // 默认普通难度
    difficultyComboBox->setFixedSize(250, 40);
    difficultyComboBox->setStyleSheet(
        "QComboBox { "
        "    background-color: white; "
        "    border: 2px solid #4169E1; "
        "    border-radius: 8px; "
        "    padding: 8px; "
        "    font-size: 14px; "
        "}"
        "QComboBox::drop-down { border: none; }"
        "QComboBox::down-arrow { image: none; }"
    );
    layout->addWidget(difficultyComboBox, 0, Qt::AlignCenter);
    
    // 按钮容器
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    
    // 返回按钮
    backFromDifficultyButton = new QPushButton("返回", difficultyMenuWidget);
    backFromDifficultyButton->setFixedSize(100, 40);
    backFromDifficultyButton->setStyleSheet("QPushButton { background-color: #6C757D; }"
                                            "QPushButton:hover { background-color: #5A6268; }");
    connect(backFromDifficultyButton, &QPushButton::clicked, this, &MainWindow::showCharacterSelection);
    buttonLayout->addWidget(backFromDifficultyButton);
    
    buttonLayout->addStretch();
    
    // 开始游戏按钮
    startGameButton = new QPushButton("开始游戏", difficultyMenuWidget);
    startGameButton->setFixedSize(120, 40);
    startGameButton->setStyleSheet("QPushButton { background-color: #28A745; }"
                                   "QPushButton:hover { background-color: #218838; }");
    connect(startGameButton, &QPushButton::clicked, this, &MainWindow::onDifficultySelected);
    buttonLayout->addWidget(startGameButton);
    
    layout->addLayout(buttonLayout);
    
    stackedWidget->addWidget(difficultyMenuWidget);
}

void MainWindow::setupMultiplayerMenu()
{
    multiplayerMenuWidget = new QWidget(this);
    multiplayerMenuWidget->setStyleSheet("QWidget#multiplayerMenuWidget { background: transparent; }");
    multiplayerMenuWidget->setObjectName("multiplayerMenuWidget");
    QVBoxLayout* layout = new QVBoxLayout(multiplayerMenuWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);
    
    // 标题
    multiplayerLabel = new QLabel("多人游戏", multiplayerMenuWidget);
    multiplayerLabel->setAlignment(Qt::AlignCenter);
    QFont font("华文彩云", 24);
    font.setBold(true);
    multiplayerLabel->setFont(font);
    multiplayerLabel->setStyleSheet("color: #FF6347; margin: 20px;");
    layout->addWidget(multiplayerLabel);
    
    // 按钮容器
    QWidget* buttonContainer = new QWidget(multiplayerMenuWidget);
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(20);
    
    // 热点模式按钮
    QPushButton* hotspotModeButton = new QPushButton("📶 热点模式", buttonContainer);
    hotspotModeButton->setFixedSize(200, 50);
    hotspotModeButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #007BFF; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 10px; "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "    background-color: #0056B3; "
        "}"
    );
    connect(hotspotModeButton, &QPushButton::clicked, this, &MainWindow::showHotspotLobby);
    buttonLayout->addWidget(hotspotModeButton);
    
    // 本地合作模式按钮
    QPushButton* localCoopButton = new QPushButton("🎮 本地合作", buttonContainer);
    localCoopButton->setFixedSize(200, 50);
    localCoopButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #28A745; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 10px; "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "    background-color: #1E7E34; "
        "}"
    );
    connect(localCoopButton, &QPushButton::clicked, this, &MainWindow::showLocalCoopCharacterSelection);
    buttonLayout->addWidget(localCoopButton);
    
    layout->addWidget(buttonContainer);
    
    // 返回按钮
    backFromMultiplayerButton = new QPushButton("返回主菜单", multiplayerMenuWidget);
    backFromMultiplayerButton->setFixedSize(150, 40);
    backFromMultiplayerButton->setStyleSheet("QPushButton { background-color: #6C757D; }"
                                             "QPushButton:hover { background-color: #5A6268; }");
    connect(backFromMultiplayerButton, &QPushButton::clicked, this, &MainWindow::showMainMenu);
    layout->addWidget(backFromMultiplayerButton, 0, Qt::AlignCenter);
    
    stackedWidget->addWidget(multiplayerMenuWidget);
}

void MainWindow::setupHighScoresMenu()
{
    highScoresWidget = new QWidget(this);
    highScoresWidget->setStyleSheet("QWidget#highScoresWidget { background: transparent; }");
    highScoresWidget->setObjectName("highScoresWidget");
    QVBoxLayout* layout = new QVBoxLayout(highScoresWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    
    // 标题
    highScoresLabel = new QLabel("🏆 高分榜 🏆", highScoresWidget);
    highScoresLabel->setAlignment(Qt::AlignCenter);
    QFont font("华文彩云", 24);
    font.setBold(true);
    highScoresLabel->setFont(font);
    highScoresLabel->setStyleSheet("color: #FF6347; margin: 20px;");
    layout->addWidget(highScoresLabel);
    
    // 高分列表
    highScoresList = new QListWidget(highScoresWidget);
    highScoresList->setFixedSize(400, 300);
    highScoresList->setStyleSheet(
        "QListWidget { "
        "    background-color: white; "
        "    border: 2px solid #4169E1; "
        "    border-radius: 10px; "
        "    font-size: 16px; "
        "    padding: 10px; "
        "}"
        "QListWidget::item { "
        "    padding: 8px; "
        "    border-bottom: 1px solid #E0E0E0; "
        "}"
        "QListWidget::item:hover { "
        "    background-color: #F0F8FF; "
        "}"
    );
    layout->addWidget(highScoresList, 0, Qt::AlignCenter);
    
    // 返回按钮
    backFromHighScoresButton = new QPushButton("返回主菜单", highScoresWidget);
    backFromHighScoresButton->setFixedSize(150, 40);
    backFromHighScoresButton->setStyleSheet("QPushButton { background-color: #6C757D; }"
                                            "QPushButton:hover { background-color: #5A6268; }");
    connect(backFromHighScoresButton, &QPushButton::clicked, this, &MainWindow::showMainMenu);
    layout->addWidget(backFromHighScoresButton, 0, Qt::AlignCenter);
    
    stackedWidget->addWidget(highScoresWidget);
}

void MainWindow::showMainMenu()
{
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

void MainWindow::showCharacterSelection()
{
    stackedWidget->setCurrentWidget(characterSelection);
}

void MainWindow::showDifficultySelection()
{
    stackedWidget->setCurrentWidget(difficultyMenuWidget);
}

void MainWindow::showMultiplayerMenu()
{
    stackedWidget->setCurrentWidget(multiplayerMenuWidget);
}



void MainWindow::showHighScores()
{
    updateHighScoresList();
    stackedWidget->setCurrentWidget(highScoresWidget);
}

void MainWindow::showSingleModeSelection()
{
    stackedWidget->setCurrentWidget(singleModeSelection);
}

void MainWindow::startSinglePlayerGame()
{
    showSingleModeSelection();
}

void MainWindow::onSingleModeSelected(SinglePlayerMode mode, CharacterType character)
{
    selectedCharacter = character; // 更新选中的角色
    gameWidget->setCharacter(character);
    gameWidget->setDifficulty(selectedDifficulty);
    gameWidget->setSinglePlayerGameMode(mode);
    stackedWidget->setCurrentWidget(gameWidget);
    gameWidget->startSinglePlayerGame();
    gameWidget->setFocus();
}





void MainWindow::onGameOver(int finalScore)
{
    QString message = QString("游戏结束！\n最终分数: %1\n\n按 R 键重新开始\n按 ESC 键返回菜单").arg(finalScore);
    // 游戏结束处理在GameWidget中完成
}

void MainWindow::onCharacterSelected(CharacterType character)
{
    selectedCharacter = character;
}

void MainWindow::onDifficultySelected()
{
    int difficultyIndex = difficultyComboBox->currentIndex();
    selectedDifficulty = static_cast<Difficulty>(difficultyComboBox->itemData(difficultyIndex).toInt());
    startSinglePlayerGame();
}





void MainWindow::updateHighScoresList()
{
    highScoresList->clear();
    
    QSettings settings("SnakeGame", "SpongeBobSnake");
    QList<int> scores;
    
    int size = settings.beginReadArray("highScores");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        scores.append(settings.value("score", 0).toInt());
    }
    settings.endArray();
    
    if (scores.isEmpty()) {
        highScoresList->addItem("暂无记录");
        return;
    }
    
    // 排序并显示
    std::sort(scores.begin(), scores.end(), std::greater<int>());
    
    for (int i = 0; i < qMin(10, scores.size()); ++i) {
        QString rankText;
        if (i == 0) rankText = "🥇";
        else if (i == 1) rankText = "🥈";
        else if (i == 2) rankText = "🥉";
        else rankText = QString("%1.").arg(i + 1);
        
        highScoresList->addItem(QString("%1 %2 分").arg(rankText).arg(scores[i]));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 检查事件是否有效
    if (!event) {
        QMainWindow::keyPressEvent(event);
        return;
    }
    
    // 检查事件是否已被接受
    if (event->isAccepted()) {
        QMainWindow::keyPressEvent(event);
        return;
    }
    
    // 将按键事件传递给当前活动的窗口部件
    if (stackedWidget && gameWidget && stackedWidget->currentWidget() == gameWidget) {
        // 确保gameWidget有焦点并且可以接收键盘事件
        if (gameWidget->isVisible() && gameWidget->isEnabled()) {
            // 使用QApplication::sendEvent来正确传递事件
            QApplication::sendEvent(gameWidget, event);
            // 如果事件被gameWidget处理，直接返回
            if (event->isAccepted()) {
                return;
            }
        }
    }
    
    // 如果事件未被处理，调用基类实现
    QMainWindow::keyPressEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (oceanBackground) {
        oceanBackground->setGeometry(0, 0, width(), height());
    }
}



void MainWindow::showLocalCoopCharacterSelection()
{
    localCoopCharacterSelection->resetSelection();
    stackedWidget->setCurrentWidget(localCoopCharacterSelection);
}



void MainWindow::showHotspotLobby()
{
    stackedWidget->setCurrentWidget(hotspotLobby);
}

void MainWindow::onHotspotNetworkError(const QString& error)
{
    QMessageBox::warning(this, "热点网络错误", error);
    showMainMenu();
}

void MainWindow::onLocalCoopCharactersSelected(CharacterType player1Character, CharacterType player2Character)
{
    // 设置本地双人游戏模式
    gameWidget->setLocalCoopMode(player1Character, player2Character);
    gameWidget->setDifficulty(Difficulty::NORMAL); // 本地双人游戏固定普通难度
    stackedWidget->setCurrentWidget(gameWidget);
    gameWidget->startLocalCoopGame();
    gameWidget->setFocus();
}
