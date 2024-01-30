//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEROOM_H
#define BATTLEGAME_SERVER_GAMEROOM_H

#include <set>

using ConnectionId = unsigned int;

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
    inline bool IsPlayerJoined(ConnectionId id) const { return mJoinedPlayers.find(id) != mJoinedPlayers.end(); }

    bool PlayerJoin(ConnectionId id);

private:
    unsigned const int mRoomId;
    unsigned short mMaxPlayerCount;
    unsigned short mCurrentPlayerCount;
    std::set<ConnectionId> mJoinedPlayers;
};


#endif //BATTLEGAME_SERVER_GAMEROOM_H
