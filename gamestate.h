#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QString>
#include <QStringList>

enum class GameState {
    MENU,
    CHARACTER_SELECTION,
    PLAYING,
    PAUSED,
    GAME_OVER,
    MULTIPLAYER_LOBBY,
    MULTIPLAYER_GAME
};

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum class CharacterType {
    SPONGEBOB,
    PATRICK,
    SQUIDWARD,
    SANDY,
    MR_KRABS,
    PLANKTON
};

enum class Difficulty {
    EASY,
    NORMAL,
    HARD
};

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

// 为Point类型添加qHash函数支持
inline size_t qHash(const Point& point, size_t seed = 0)
{
    return qHash(static_cast<int>(point.x), seed) ^ qHash(static_cast<int>(point.y), seed << 1);
}

struct PlayerInfo {
    std::string name;
    int score;
    CharacterType character;
    bool isAlive;
    
    PlayerInfo() : name(""), score(0), character(CharacterType::SPONGEBOB), isAlive(true) {}
};

struct GameRoom {
    QString roomId;
    QString hostName;
    QStringList playerNames;
    int maxPlayers;
    int currentPlayers;
    bool isGameStarted;
    
    GameRoom() : maxPlayers(4), currentPlayers(0), isGameStarted(false) {}
};

#endif // GAMESTATE_H