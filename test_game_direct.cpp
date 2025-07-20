#include <QApplication>
#include "gamewidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 直接创建并显示GameWidget
    GameWidget* gameWidget = new GameWidget();
    gameWidget->setCharacter(CharacterType::SPONGEBOB);
    gameWidget->setDifficulty(Difficulty::NORMAL);
    gameWidget->setSinglePlayerGameMode(SinglePlayerMode::CLASSIC);
    
    gameWidget->show();
    gameWidget->startSinglePlayerGame();
    gameWidget->setFocus();
    
    return app.exec();
}