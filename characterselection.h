#ifndef CHARACTERSELECTION_H
#define CHARACTERSELECTION_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
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
    
    CharacterType getSelectedCharacter() const { return selectedCharacter; }
    void setSelectedCharacter(CharacterType character);
    
signals:
    void characterSelected(CharacterType character);
    void backToMenu();
    void startGame();
    
private slots:
    void onCharacterButtonClicked();
    void onBackButtonClicked();
    void onStartButtonClicked();
    
private:
    void setupUI();
    void updateSelection();
    
    QVBoxLayout* mainLayout;
    QLabel* titleLabel;
    QGridLayout* charactersLayout;
    QHBoxLayout* buttonsLayout;
    
    QList<CharacterButton*> characterButtons;
    QPushButton* backButton;
    QPushButton* startButton;
    
    CharacterType selectedCharacter;
    CharacterButton* selectedButton;
};

#endif // CHARACTERSELECTION_H