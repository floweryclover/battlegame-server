//
// Created by floweryclover on 2024-01-30.
//

#include "GameRoomManager.h"
#include "GameData.h"
#include "BattleGameServer.h"
#include "ClientManager.h"
#include <iostream>

GameRoomManager::GameRoomManager() noexcept : mNewRoomId(ROOM_MAINMENU+1)
{
    mGameRooms.emplace(ROOM_MAINMENU, std::make_unique<MainMenuRoom>(ROOM_MAINMENU));
}

void GameRoomManager::Tick()
{
    // 매치메이킹
    if (mMatchMakingQueue.size() >= 2)
    {
        auto playerA = mMatchMakingQueue.front();
        mMatchMakingQueue.pop();
        if (mMatchMakingSet.contains(playerA))
        {
            mMatchMakingSet.erase(playerA);
            auto playerB = mMatchMakingQueue.front();
            mMatchMakingQueue.pop();
            if (mMatchMakingSet.contains(playerB))
            {
                mMatchMakingSet.erase(playerB);

                mGameRooms.emplace(mNewRoomId, std::make_unique<OneVsOneGameRoom>(mNewRoomId, playerA, playerB));
                JoinPlayer(playerA, mNewRoomId);
                JoinPlayer(playerB, mNewRoomId);
                mNewRoomId++;
            }
            else // 플레이어 A는 실제로 매치메이킹 중이지만, B는 매치메이킹중이지 않음. A를 다시 큐에 넣음
            {
                mMatchMakingQueue.emplace(playerA);
                mMatchMakingSet.emplace(playerA);
            }
        }
    }

    // 삭제할 방이 mRoomDestoryQueue에 있다면 삭제
    if (!mRoomDestroyQueue.empty())
    {
        auto room = mRoomDestroyQueue.front();
        mRoomDestroyQueue.pop();

        if (!mGameRooms.contains(room) || room == ROOM_MAINMENU)
        {std::cerr << "존재하지 않는 방 또는 메인메뉴용 방을 삭제하려고 시도했습니다." << std::endl;}
        else
        {
            auto pPlayerCountableRoom = dynamic_cast<IPlayerCountable*>(mGameRooms.at(room).get());
            if (pPlayerCountableRoom != nullptr)
            {
                auto players = pPlayerCountableRoom->GetAllPlayers();
                for (auto player : players)
                { JoinPlayer(player, ROOM_MAINMENU);}
            }
            mGameRooms.erase(room);
        }
    }

    // 모든 게임방 틱
    for (auto iter = mGameRooms.begin(); iter != mGameRooms.end(); iter++)
    {
        auto pTickable = dynamic_cast<ITickable*>(iter->second.get());
        //auto pTickable = dynamic_cast<ITickable*>(dynamic_cast<OneVsOneGameRoom*>(iter->second.get()));
        if (pTickable != nullptr)
        {pTickable->Tick();}
    }
}

void GameRoomManager::JoinPlayer(ClientId clientId, GameRoomId roomId)
{
    if (!mRoomOfPlayers.contains(clientId)
    || !mGameRooms.contains(roomId))
    {return;}

    auto pPreviousRoom = mGameRooms.at(mRoomOfPlayers.at(clientId)).get();
    if (pPreviousRoom != nullptr)
    {pPreviousRoom->OnPlayerLeft(clientId);}

    auto pNewRoom = mGameRooms.at(roomId).get();
    if (pNewRoom != nullptr)
    {pNewRoom->OnPlayerJoined(clientId);}
    else
    {std::cerr << "클라이언트 " << clientId << "을 " << roomId << "번 게임으로 이동하려 했으나 존재하지 않았습니다." << std::endl; }

    mRoomOfPlayers.at(clientId) = roomId;
}

void GameRoomManager::StartMatchMaking(ClientId clientId)
{
    if (!mRoomOfPlayers.contains(clientId)
    || mRoomOfPlayers.at(clientId) != ROOM_MAINMENU)
    {return;}

    mMatchMakingQueue.emplace(clientId);
    mMatchMakingSet.emplace(clientId);
}

void GameRoomManager::StopMatchMaking(ClientId clientId)
{mMatchMakingSet.erase(clientId);}

void GameRoomManager::OnPlayerConnected(ClientId clientId)
{
    mRoomOfPlayers.emplace(clientId, ROOM_MAINMENU);
    JoinPlayer(clientId, ROOM_MAINMENU);
}

void GameRoomManager::OnPlayerDisconnected(ClientId clientId)
{
    auto roomId = mRoomOfPlayers.at(clientId);
    mRoomOfPlayers.erase(clientId);
    mGameRooms.at(roomId)->OnPlayerLeft(clientId);
}

std::optional<GameRoomId> GameRoomManager::GetPlayerJoinedRoomId(ClientId clientId) const noexcept
{
    if (!mRoomOfPlayers.contains(clientId))
    {return std::nullopt;}

    return mRoomOfPlayers.at(clientId);
}

BaseRoom* GameRoomManager::GetGameRoom(GameRoomId roomId) noexcept
{return const_cast<BaseRoom*>(this->GetConstGameRoom(roomId));}

const BaseRoom* GameRoomManager::GetConstGameRoom(GameRoomId roomId) const noexcept
{
    if (!mGameRooms.contains(roomId)
    || roomId == ROOM_MAINMENU)
    {return nullptr;}
    return mGameRooms.at(roomId).get();
}

void GameRoomManager::DestroyRoom(GameRoomId roomId) noexcept
{mRoomDestroyQueue.emplace(roomId);}