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

#include "singlemodeselection.h"
#include "oceanbackground.h"

#include "localcoopcharacterselection.h"
#include "localcoopmodeselection.h"
#include "singleplayergamemanager.h"
#include "hotspotnetworkmanager.h"
#include "hotspotgamemanager.h"
#include "hotspotlobby.h"

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
    void showLocalCoopModeSelection();
    void showLocalCoopCharacterSelection();
    void showHighScores();
    void showSingleModeSelection();
    void startSinglePlayerGame();
    void onGameOver(int finalScore);
    void onCharacterSelected(CharacterType character);
    void onDifficultySelected();

    void onSingleModeSelected(SinglePlayerMode mode, CharacterType character);
    void onLocalCoopModeSelected(SinglePlayerMode mode);
    void onLocalCoopCharactersSelected(CharacterType player1Character, CharacterType player2Character, SinglePlayerMode mode);
    void showHotspotLobby();
    void onHotspotNetworkError(const QString& error);
    void onHotspotGameStartRequested();
    
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
    QWidget* preparationWidget;
    GameWidget* gameWidget;
    CharacterSelection* characterSelection;

    SingleModeSelection* singleModeSelection;

    LocalCoopModeSelection* localCoopModeSelection;
    LocalCoopCharacterSelection* localCoopCharacterSelection;
    OceanBackground* oceanBackground;
    SinglePlayerGameManager* singlePlayerGameManager;
    
    // 热点相关组件
    HotspotNetworkManager* hotspotNetworkManager;
    HotspotGameManager* hotspotGameManager;
    HotspotLobby* hotspotLobby;
    
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
