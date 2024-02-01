//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEROOM_H
#define BATTLEGAME_SERVER_GAMEROOM_H

#include <set>

using ConnectionId = unsigned int;
using GameRoomId = unsigned int;

class GameRoom {
public:
    static constexpr GameRoomId ROOM_MAINMENU = 0;
    static constexpr GameRoomId ROOM_MATCHMAKING = 1;

    GameRoom(const GameRoom& rhs) = delete;
    GameRoom& operator=(const GameRoom& rhs) = delete;
    explicit GameRoom(unsigned int roomId) noexcept;
    GameRoom(GameRoom&& rhs) noexcept;
    ~GameRoom() = default;

    inline bool IsEmpty() const { return mJoinedPlayers.size() == 0; }
    inline bool IsFull() const { return mJoinedPlayers.size() == mMaxPlayerCount; }
    inline bool IsPlayerJoined(ConnectionId id) const { return mJoinedPlayers.find(id) != mJoinedPlayers.end(); }
    inline unsigned short GetMaxPlayerCount() const { return mMaxPlayerCount; }
    inline unsigned short GetCurrentPlayerCount() const { return mJoinedPlayers.size(); }

    void OnPlayerJoined(ConnectionId id) noexcept;
    void OnPlayerLeft(ConnectionId id) noexcept;

private:
    unsigned const int mRoomId;
    unsigned short mMaxPlayerCount;
    std::set<ConnectionId> mJoinedPlayers;
};


#endif //BATTLEGAME_SERVER_GAMEROOM_H
