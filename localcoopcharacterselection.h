#ifndef LOCALCOOPCHARACTERSELECTION_H
#define LOCALCOOPCHARACTERSELECTION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include "gamestate.h"

class LocalCoopCharacterSelection : public QWidget
{
    Q_OBJECT

public:
    explicit LocalCoopCharacterSelection(QWidget *parent = nullptr);
    ~LocalCoopCharacterSelection();

private slots:
    void onPlayer1CharacterSelected(CharacterType character);
    void onPlayer2CharacterSelected(CharacterType character);
    void onStartGameClicked();
    void onBackClicked();

signals:
    void startLocalCoopGame(CharacterType player1Character, CharacterType player2Character);
    void backToModeSelection();

private:
    void setupUI();
    void createCharacterButton(QWidget* parent, QVBoxLayout* layout, CharacterType character, bool isPlayer1);
    void updateStartButtonState();
    QString getCharacterName(CharacterType character);
    QString getCharacterEmoji(CharacterType character);
    
    // UI组件
    QVBoxLayout* mainLayout;
    QLabel* titleLabel;
    QGroupBox* player1Group;
    QGroupBox* player2Group;
    QPushButton* startGameButton;
    QPushButton* backButton;
    
    // 角色选择状态
    CharacterType player1Character;
    CharacterType player2Character;
    bool player1Selected;
    bool player2Selected;
    
    // 角色按钮
    QMap<CharacterType, QPushButton*> player1Buttons;
    QMap<CharacterType, QPushButton*> player2Buttons;
};

#endif // LOCALCOOPCHARACTERSELECTION_H