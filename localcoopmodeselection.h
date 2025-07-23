#ifndef LOCALCOOPMODESELECTION_H
#define LOCALCOOPMODESELECTION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "gamestate.h"
#include "singleplayergamemanager.h"

class LocalCoopModeSelection : public QWidget
{
    Q_OBJECT
    
public:
    explicit LocalCoopModeSelection(QWidget *parent = nullptr);
    ~LocalCoopModeSelection();
    
signals:
    void modeSelected(SinglePlayerMode mode);
    void backToMenu();
    
private slots:
    void onModeButtonClicked();
    void onBackClicked();
    void onStartGameClicked();
    
private:
    void setupUI();
    void setupModeButtons();
    void setupDetailsPanel();
    void setupBottomButtons();
    void showModeDetails(SinglePlayerMode mode);
    void updateModeButton(QPushButton* button, SinglePlayerMode mode);
    QString getModeColor(SinglePlayerMode mode) const;
    void animateButton(QPushButton* button);
    
    // UI组件
    QVBoxLayout* mainLayout;
    QHBoxLayout* contentLayout;
    QHBoxLayout* bottomLayout;
    QLabel* titleLabel;
    
    // 模式选择
    QWidget* modeSelectionWidget;
    QVBoxLayout* modeLayout;
    QPushButton* classicButton;
    QPushButton* timeAttackButton;
    QPushButton* challengeButton;
    QMap<SinglePlayerMode, QPushButton*> modeButtons;
    
    // 详情面板
    QWidget* detailsWidget;
    QVBoxLayout* detailsLayout;
    QLabel* modeNameLabel;
    QLabel* modeDescriptionLabel;
    
    // 底部按钮
    QPushButton* backButton;
    QPushButton* startGameButton;
    
    // 状态
    SinglePlayerMode selectedMode;
    
    // 动画
    QPropertyAnimation* buttonAnimation;
    QGraphicsOpacityEffect* opacityEffect;
};

#endif // LOCALCOOPMODESELECTION_H