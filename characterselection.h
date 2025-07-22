#ifndef CHARACTERSELECTION_H
#define CHARACTERSELECTION_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QTimer>
#include <QListWidget>
#include <QMap>
#include <QStringList>
#include <QList>
#include <QEnterEvent>
#include <QPaintEvent>
#include <QEvent>
#include <QString>
#include "gamestate.h"

class CharacterButton : public QPushButton
{
    Q_OBJECT
    
public:
    explicit CharacterButton(CharacterType character, QWidget *parent = nullptr);
    CharacterType getCharacter() const { return character; }
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    
private:
    CharacterType character;
    QPixmap characterPixmap;
    QString characterName;
    bool hovered;
    
    void loadCharacterInfo();
};

class CharacterSelection : public QWidget
{
    Q_OBJECT

public:
    explicit CharacterSelection(QWidget *parent = nullptr);
    
    void setSelectedCharacter(CharacterType character);
    CharacterType getSelectedCharacter() const { return selectedCharacter; }
    
    // 准备机制相关函数
    void setPlayerReady(const QString& playerName, bool ready);
    void updatePlayerList();
    bool isPlayerReady(const QString& playerName) const;
    void updatePlayerCharacter(const QString& playerName, CharacterType character);
    void setPlayerNames(const QStringList& names);
    void showStartButton();
    void setIsHost(bool isHost);
    void setCurrentPlayerName(const QString& name);
    
    // 角色占用检查
    bool isCharacterTaken(CharacterType character) const;
    void updateCharacterAvailability();
    
    // 检查所有玩家准备状态
    bool checkAllPlayersReady();
    
signals:
    void characterSelected(CharacterType character);
    void playerReadyChanged(bool ready);
    void backToMenu();
    void startGame();
    void allPlayersReady();
    
private slots:
    void onCharacterButtonClicked();
    void onBackButtonClicked();
    void onStartButtonClicked();
    
private:
    void setupUI();
    void updateSelection();
    void updateReadyButton();
    
    QVBoxLayout* mainLayout;
    QLabel* titleLabel;
    QGridLayout* charactersLayout;
    QHBoxLayout* buttonsLayout;
    
    QList<CharacterButton*> characterButtons;
    QLabel* playersStatusLabel;
    QListWidget* playersListWidget;
    QPushButton* backButton;
    QPushButton* readyButton;
    QPushButton* startButton;
    QLabel* countdownLabel;
    
    CharacterType selectedCharacter;
    CharacterButton* selectedButton;
    
    // 准备机制相关
    bool playerReady;
    QMap<QString, bool> playerReadyStatus;
    QMap<QString, CharacterType> playerCharacters;
    QStringList playerNames;
    QString currentPlayerName;
    bool isHost;
    QTimer* countdownTimer;
    int countdownValue;
};

#endif // CHARACTERSELECTION_H