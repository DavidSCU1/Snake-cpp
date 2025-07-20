#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>
#include <QApplication>
#include <QSettings>
#include <QHostAddress>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , selectedCharacter(CharacterType::SPONGEBOB)
    , selectedDifficulty(Difficulty::NORMAL)
    , isMultiplayerHost(false)
{
    ui->setupUi(this);
    setupUI();
    
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
    // 创建堆叠窗口部件
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    
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
    
    // 创建游戏页面
    gameWidget = new GameWidget(this);
    connect(gameWidget, &GameWidget::gameOver, this, &MainWindow::onGameOver);
    connect(gameWidget, &GameWidget::backToMenu, this, &MainWindow::showMainMenu);
    stackedWidget->addWidget(gameWidget);
    
    // 创建网络管理器
    networkManager = new NetworkManager(this);
    connect(networkManager, &NetworkManager::connectionError, this, &MainWindow::onNetworkError);
    
    // 设置样式
    setStyleSheet(
        "QMainWindow { background-color: #F0F8FF; }"
        "QPushButton { "
        "    background-color: #4169E1; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 8px; "
        "    padding: 12px 24px; "
        "    font-size: 14px; "
        "    font-weight: bold; "
        "}"
        "QPushButton:hover { background-color: #6495ED; }"
        "QPushButton:pressed { background-color: #0000CD; }"
        "QLabel { color: #2F4F4F; }"
    );
}

void MainWindow::setupMainMenu()
{
    mainMenuWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(mainMenuWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);
    
    // 标题
    titleLabel = new QLabel("🧽 海绵宝宝贪吃蛇 🧽", mainMenuWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(36);
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
    connect(singlePlayerButton, &QPushButton::clicked, this, &MainWindow::showCharacterSelection);
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
    QVBoxLayout* layout = new QVBoxLayout(difficultyMenuWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);
    
    // 标题
    difficultyLabel = new QLabel("选择难度", difficultyMenuWidget);
    difficultyLabel->setAlignment(Qt::AlignCenter);
    QFont font = difficultyLabel->font();
    font.setPointSize(24);
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
    QVBoxLayout* layout = new QVBoxLayout(multiplayerMenuWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);
    
    // 标题
    multiplayerLabel = new QLabel("多人游戏", multiplayerMenuWidget);
    multiplayerLabel->setAlignment(Qt::AlignCenter);
    QFont font = multiplayerLabel->font();
    font.setPointSize(24);
    font.setBold(true);
    multiplayerLabel->setFont(font);
    multiplayerLabel->setStyleSheet("color: #FF6347; margin: 20px;");
    layout->addWidget(multiplayerLabel);
    
    // 按钮容器
    QWidget* buttonContainer = new QWidget(multiplayerMenuWidget);
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(20);
    
    // 创建房间按钮
    hostGameButton = new QPushButton("🏠 创建房间", buttonContainer);
    hostGameButton->setFixedSize(200, 50);
    connect(hostGameButton, &QPushButton::clicked, this, &MainWindow::startMultiplayerHost);
    buttonLayout->addWidget(hostGameButton);
    
    // 服务器地址输入
    QLabel* addressLabel = new QLabel("服务器地址:", buttonContainer);
    addressLabel->setAlignment(Qt::AlignCenter);
    buttonLayout->addWidget(addressLabel);
    
    serverAddressEdit = new QLineEdit("127.0.0.1", buttonContainer);
    serverAddressEdit->setFixedSize(200, 30);
    serverAddressEdit->setAlignment(Qt::AlignCenter);
    serverAddressEdit->setStyleSheet(
        "QLineEdit { "
        "    background-color: white; "
        "    border: 2px solid #4169E1; "
        "    border-radius: 8px; "
        "    padding: 8px; "
        "    font-size: 14px; "
        "}"
    );
    buttonLayout->addWidget(serverAddressEdit);
    
    // 加入房间按钮
    joinGameButton = new QPushButton("🚪 加入房间", buttonContainer);
    joinGameButton->setFixedSize(200, 50);
    connect(joinGameButton, &QPushButton::clicked, this, &MainWindow::startMultiplayerJoin);
    buttonLayout->addWidget(joinGameButton);
    
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
    QVBoxLayout* layout = new QVBoxLayout(highScoresWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    
    // 标题
    highScoresLabel = new QLabel("🏆 高分榜 🏆", highScoresWidget);
    highScoresLabel->setAlignment(Qt::AlignCenter);
    QFont font = highScoresLabel->font();
    font.setPointSize(24);
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

void MainWindow::startSinglePlayerGame()
{
    gameWidget->setCharacter(selectedCharacter);
    gameWidget->setDifficulty(selectedDifficulty);
    stackedWidget->setCurrentWidget(gameWidget);
    gameWidget->startSinglePlayerGame();
    gameWidget->setFocus();
}

void MainWindow::startMultiplayerHost()
{
    isMultiplayerHost = true;
    
    // 先选择角色
    characterSelection->setSelectedCharacter(selectedCharacter);
    connect(characterSelection, &CharacterSelection::startGame, this, [this]() {
        gameWidget->setCharacter(selectedCharacter);
        gameWidget->setDifficulty(Difficulty::NORMAL); // 多人游戏固定普通难度
        stackedWidget->setCurrentWidget(gameWidget);
        gameWidget->startMultiPlayerGame(true);
        gameWidget->setFocus();
    }, Qt::SingleShotConnection);
    
    stackedWidget->setCurrentWidget(characterSelection);
}

void MainWindow::startMultiplayerJoin()
{
    isMultiplayerHost = false;
    QString serverAddress = serverAddressEdit->text().trimmed();
    
    if (serverAddress.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入服务器地址！");
        return;
    }
    
    // 先选择角色
    characterSelection->setSelectedCharacter(selectedCharacter);
    connect(characterSelection, &CharacterSelection::startGame, this, [this, serverAddress]() {
        // 连接到服务器
        networkManager->connectToServer(serverAddress);
        
        gameWidget->setCharacter(selectedCharacter);
        gameWidget->setDifficulty(Difficulty::NORMAL); // 多人游戏固定普通难度
        stackedWidget->setCurrentWidget(gameWidget);
        gameWidget->startMultiPlayerGame(false);
        gameWidget->setFocus();
    }, Qt::SingleShotConnection);
    
    stackedWidget->setCurrentWidget(characterSelection);
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

void MainWindow::connectToServer()
{
    QString serverAddress = serverAddressEdit->text().trimmed();
    if (!serverAddress.isEmpty()) {
        networkManager->connectToServer(serverAddress);
    }
}

void MainWindow::onNetworkError(const QString& error)
{
    QMessageBox::warning(this, "网络错误", error);
    showMainMenu();
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
    // 将按键事件传递给当前活动的窗口部件
    if (stackedWidget->currentWidget() == gameWidget) {
        // 直接发送事件给gameWidget
        QApplication::sendEvent(gameWidget, event);
    } else {
        QMainWindow::keyPressEvent(event);
    }
}
