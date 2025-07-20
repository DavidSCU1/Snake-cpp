#ifndef GAMESTATE_H
#define GAMESTATE_H

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
};

struct PlayerInfo {
    std::string name;
    int score;
    CharacterType character;
    bool isAlive;
    
    PlayerInfo() : name(""), score(0), character(CharacterType::SPONGEBOB), isAlive(true) {}
};

#endif // GAMESTATE_H