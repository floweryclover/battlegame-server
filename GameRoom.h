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

    inline bool IsEmpty() const { return mCurrentPlayerCount == 0; }
    inline bool IsFull() const { return mMaxPlayerCount == mCurrentPlayerCount; }
    inline unsigned short GetFreePlayerSlotCount() const { return mMaxPlayerCount - mCurrentPlayerCount; }

private:
    unsigned const int mRoomId;
    unsigned short mMaxPlayerCount;
    unsigned short mCurrentPlayerCount;
};


#endif //BATTLEGAME_SERVER_GAMEROOM_H
