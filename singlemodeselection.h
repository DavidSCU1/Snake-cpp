#ifndef SINGLEMODESELECTION_H
#define SINGLEMODESELECTION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QProgressBar>
#include <QFrame>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "singleplayergamemanager.h"

class SingleModeSelection : public QWidget
{
    Q_OBJECT

public:
    explicit SingleModeSelection(QWidget *parent = nullptr);
    ~SingleModeSelection();
    
    void setGameManager(SinglePlayerGameManager* manager);
    void updateAchievements();
    void showModeDetails(SinglePlayerMode mode);
    
signals:
    void modeSelected(SinglePlayerMode mode);
    void backToMenu();
    void achievementsRequested();
    
protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    
private slots:
    void onModeButtonClicked();
    void onAchievementUnlocked(const Achievement& achievement);
    void showAchievements();
    void animateButton(QPushButton* button);
    
private:
    void setupUI();
    void setupModeButtons();
    void setupAchievementPanel();
    void setupDetailsPanel();
    void updateModeButton(QPushButton* button, SinglePlayerMode mode);
    void updateAchievementProgress();
    QString getModeIcon(SinglePlayerMode mode) const;
    QString getModeColor(SinglePlayerMode mode) const;
    
    // UI组件
    QVBoxLayout* mainLayout;
    QHBoxLayout* contentLayout;
    
    // 左侧模式选择
    QWidget* modeSelectionWidget;
    QVBoxLayout* modeLayout;
    QLabel* titleLabel;
    QScrollArea* modeScrollArea;
    QWidget* modeButtonContainer;
    QGridLayout* modeButtonLayout;
    
    // 模式按钮
    QPushButton* classicButton;
    QPushButton* timeAttackButton;
    QPushButton* survivalButton;
    QPushButton* speedRunButton;
    QPushButton* aiBattleButton;
    
    // 右侧详情面板
    QWidget* detailsWidget;
    QVBoxLayout* detailsLayout;
    QLabel* modeNameLabel;
    QLabel* modeDescriptionLabel;
    QLabel* modeStatsLabel;
    QPushButton* startModeButton;
    
    // 成就面板
    QWidget* achievementWidget;
    QVBoxLayout* achievementLayout;
    QLabel* achievementTitleLabel;
    QScrollArea* achievementScrollArea;
    QWidget* achievementContainer;
    QVBoxLayout* achievementContainerLayout;
    QPushButton* achievementButton;
    
    // 底部按钮
    QHBoxLayout* bottomLayout;
    QPushButton* backButton;
    QPushButton* settingsButton;
    
    // 数据
    SinglePlayerGameManager* gameManager;
    SinglePlayerMode selectedMode;
    QMap<SinglePlayerMode, QPushButton*> modeButtons;
    QList<QWidget*> achievementWidgets;
    
    // 动画
    QPropertyAnimation* buttonAnimation;
    QGraphicsOpacityEffect* opacityEffect;
};

// 成就显示组件
class AchievementWidget : public QFrame
{
    Q_OBJECT
    
public:
    explicit AchievementWidget(const Achievement& achievement, QWidget* parent = nullptr);
    void updateProgress(const Achievement& achievement);
    
private:
    void setupUI(const Achievement& achievement);
    
    QHBoxLayout* layout;
    QLabel* iconLabel;
    QVBoxLayout* textLayout;
    QLabel* nameLabel;
    QLabel* descriptionLabel;
    QProgressBar* progressBar;
    QLabel* statusLabel;
    
    QString achievementId;
};

#endif // SINGLEMODESELECTION_H