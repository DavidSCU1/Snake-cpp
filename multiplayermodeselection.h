#ifndef MULTIPLAYERMODESELECTION_H
#define MULTIPLAYERMODESELECTION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "gamestate.h"

enum class MultiplayerMode {
    LOCAL_COOP,     // 本地联机
    NETWORK         // 局域网联机
};

class MultiplayerModeSelection : public QWidget
{
    Q_OBJECT

public:
    explicit MultiplayerModeSelection(QWidget *parent = nullptr);
    ~MultiplayerModeSelection();

private slots:
    void onLocalCoopClicked();
    void onNetworkClicked();
    void onBackClicked();

signals:
    void modeSelected(MultiplayerMode mode);
    void backToMainMenu();

private:
    void setupUI();
    
    // UI组件
    QVBoxLayout* mainLayout;
    QLabel* titleLabel;
    QPushButton* localCoopButton;
    QPushButton* networkButton;
    QPushButton* backButton;
};

#endif // MULTIPLAYERMODESELECTION_H