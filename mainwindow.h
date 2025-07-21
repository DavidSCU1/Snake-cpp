#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QInputDialog>
#include "gamestate.h"
#include "gamewidget.h"
#include "characterselection.h"
#include "networkmanager.h"
#include "multiplayerlobby.h"
#include "singlemodeselection.h"
#include "oceanbackground.h"
#include "multiplayermodeselection.h"
#include "localcoopcharacterselection.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void showMainMenu();
    void showCharacterSelection();
    void showDifficultySelection();
    void showMultiplayerMenu();
    void showMultiplayerLobby();
    void showMultiplayerModeSelection();
    void showLocalCoopCharacterSelection();
    void showHighScores();
    void showSingleModeSelection();
    void startSinglePlayerGame();
    void startMultiplayerHost();
    void startMultiplayerJoin();
    void onGameOver(int finalScore);
    void onCharacterSelected(CharacterType character);
    void onDifficultySelected();
    void connectToServer();
    void onNetworkError(const QString& error);
    void onSingleModeSelected(SinglePlayerMode mode);
    void onMultiplayerModeSelected(MultiplayerMode mode);
    void onLocalCoopCharactersSelected(CharacterType player1Character, CharacterType player2Character);
    
private:
    void setupUI();
    void setupMainMenu();
    void setupDifficultyMenu();
    void setupMultiplayerMenu();
    void setupHighScoresMenu();
    void updateHighScoresList();
    
    Ui::MainWindow *ui;
    
    // 主要组件
    QStackedWidget* stackedWidget;
    GameWidget* gameWidget;
    CharacterSelection* characterSelection;
    NetworkManager* networkManager;
    MultiPlayerLobby* multiPlayerLobby;
    SingleModeSelection* singleModeSelection;
    MultiplayerModeSelection* multiplayerModeSelection;
    LocalCoopCharacterSelection* localCoopCharacterSelection;
    OceanBackground* oceanBackground;
    
    // 菜单页面
    QWidget* mainMenuWidget;
    QWidget* difficultyMenuWidget;
    QWidget* multiplayerMenuWidget;
    QWidget* highScoresWidget;
    
    // 主菜单组件
    QLabel* titleLabel;
    QPushButton* singlePlayerButton;
    QPushButton* multiplayerButton;
    QPushButton* highScoresButton;
    QPushButton* exitButton;
    
    // 难度选择组件
    QLabel* difficultyLabel;
    QComboBox* difficultyComboBox;
    QPushButton* startGameButton;
    QPushButton* backFromDifficultyButton;
    
    // 多人游戏组件
    QLabel* multiplayerLabel;
    QPushButton* hostGameButton;
    QPushButton* joinGameButton;
    QLineEdit* serverAddressEdit;
    QPushButton* backFromMultiplayerButton;
    
    // 高分榜组件
    QLabel* highScoresLabel;
    QListWidget* highScoresList;
    QPushButton* backFromHighScoresButton;
    
    // 游戏状态
    CharacterType selectedCharacter;
    Difficulty selectedDifficulty;
    bool isMultiplayerHost;
};

#endif // MAINWINDOW_H
