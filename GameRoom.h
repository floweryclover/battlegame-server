//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEROOM_H
#define BATTLEGAME_SERVER_GAMEROOM_H


class GameRoom {
public:
    GameRoom(const GameRoom& rhs) = delete;
    GameRoom& operator=(const GameRoom& rhs) = delete;
    explicit GameRoom(unsigned int roomId) noexcept;
    GameRoom(GameRoom&& rhs) noexcept;
    ~GameRoom() = default;

private:
    unsigned const int mRoomId;
};


#endif //BATTLEGAME_SERVER_GAMEROOM_H
