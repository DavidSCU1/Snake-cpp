#ifndef LOCALCOOPCHARACTERSELECTION_H
#define LOCALCOOPCHARACTERSELECTION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include "gamestate.h"

class LocalCoopCharacterButton : public QPushButton
{
    Q_OBJECT
    
public:
    explicit LocalCoopCharacterButton(CharacterType character, QWidget *parent = nullptr);
    CharacterType getCharacter() const { return character; }
    void setDisabled(bool disabled);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    
private:
    CharacterType character;
    QPixmap characterPixmap;
    QString characterName;
    bool hovered;
    bool isDisabledCustom;
    
    void loadCharacterInfo();
};

class LocalCoopCharacterSelection : public QWidget
{
    Q_OBJECT

public:
    explicit LocalCoopCharacterSelection(QWidget *parent = nullptr);
    ~LocalCoopCharacterSelection();
    
    CharacterType getPlayer1Character() const;
    CharacterType getPlayer2Character() const;
    void resetSelection();

private slots:
    void onCharacterButtonClicked(CharacterType character);
    void onNextClicked();

signals:
    void startLocalCoopGame(CharacterType player1Character, CharacterType player2Character);
    void backClicked();

private:
    void setupUI();
    void setupPlayer1Selection();
    void setupPlayer2Selection();
    
    // UI组件
    QVBoxLayout* mainLayout;
    QLabel* titleLabel;
    QGroupBox* characterGroup;
    QPushButton* backButton;
    QPushButton* nextButton;
    
    // 角色选择状态
    CharacterType player1Character;
    CharacterType player2Character;
    bool isPlayer1Turn;
    LocalCoopCharacterButton* selectedButton;
    
    // 角色按钮映射
    QMap<CharacterType, LocalCoopCharacterButton*> characterButtons;
};

#endif // LOCALCOOPCHARACTERSELECTION_H