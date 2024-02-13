//
// Created by floweryclover on 2024-01-30.
//

#include "GameRoom.h"
#include "GameData.h"
#include "BattleGameServer.h"
#include "StcRpc.h"
#include "ClientManager.h"
#include <iostream>

GameRoom::GameRoom(unsigned int roomId) noexcept : mRoomId(roomId), mNewEntityId(0)
{
    this->mMaxPlayerCount = 4;
}

GameRoom::GameRoom(GameRoom &&rhs) noexcept : GameRoom(rhs.mRoomId)
{

}

void GameRoom::InvokeOnPlayerJoined(ClientId clientId) noexcept
{
    mJoinedPlayers.insert(clientId);
    if (mRoomId == ROOM_MAINMENU)
    {
        return;
    }
    else if (mRoomId == ROOM_MATCHMAKING)
    {
        std::cout << "플레이어 " << clientId << " 매치메이킹 시작" << std::endl;
        return;
    }
    // 캐릭터 엔티티 생성
    mEntities.emplace(mNewEntityId);
    mPlayerControllingPawnIds.emplace(clientId, mNewEntityId);
    mEntityLocations.emplace(mNewEntityId, Vector {0.0, 0.0, 400.0});
    mEntityDirections.emplace(mNewEntityId, 0.0);
    // 방 내 모든 플레이어에게 엔티티 생성 멀티캐스트
    for (auto cIter = mOnlinePlayers.cbegin(); cIter != mOnlinePlayers.cend(); cIter++)
    {
        BattleGameServer::GetInstance()
                .GetConstStcRpc()
                .SpawnEntity(*cIter, mNewEntityId, mEntityLocations.at(mNewEntityId), mEntityDirections.at(mNewEntityId));
    }
    mNewEntityId++;
    std::cout << "플레이어 " << clientId << " 방 " << mRoomId << " 참여" << std::endl;
    BattleGameServer::GetInstance()
    .GetConstStcRpc()
    .JoinedGameRoom(clientId);
}

void GameRoom::InvokeOnPlayerLeft(ClientId clientId) noexcept
{
    mJoinedPlayers.erase(clientId);
    if (mRoomId == ROOM_MAINMENU)
    {
        return;
    }
    else if (mRoomId == ROOM_MATCHMAKING)
    {
        std::cout << "플레이어 " << clientId << " 매치메이킹 종료" << std::endl;
        return;
    }
    // 방 내 모든 플레이어에게 해당 플레이어 삭제를 멀티캐스트
    mOnlinePlayers.erase(clientId);
    if (mPlayerControllingPawnIds.contains(clientId))
    {
        EntityId playerPawnId = mPlayerControllingPawnIds.at(clientId);
        for (auto cIter = mOnlinePlayers.cbegin(); cIter != mOnlinePlayers.cend(); cIter++)
        {
            BattleGameServer::GetInstance()
                    .GetConstStcRpc()
                    .DespawnEntity(*cIter, playerPawnId);
        }
        mEntities.erase(playerPawnId);
        mEntityLocations.erase(playerPawnId);
        mEntityDirections.erase(playerPawnId);
        mPlayerControllingPawnIds.erase(clientId);
    }

    std::cout << "플레이어 " << clientId << " 방 " << mRoomId << " 퇴장" << std::endl;
    BattleGameServer::GetInstance()
    .GetConstStcRpc()
    .DisconnectedFromGame(clientId);
}

void GameRoom::InvokeOnPlayerPrepared(ClientId clientId) noexcept
{
    // 온라인 플레이어에 등록
    mOnlinePlayers.emplace(clientId);

    // 이 클라이언트에게 모든 정보 보내기
    for (auto cIter = mEntities.cbegin(); cIter != mEntities.cend(); cIter++)
    {
        BattleGameServer::GetInstance()
        .GetConstStcRpc()
        .SpawnEntity(clientId, *cIter, mEntityLocations.at(*cIter), mEntityDirections.at(*cIter));
    }

    // 소유 폰 지정
    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .PossessEntity(clientId, mPlayerControllingPawnIds.at(clientId));
}

void GameRoom::InvokeOnPlayerMove(ClientId clientId, const Vector& location, double direction) noexcept
{
    if (!mOnlinePlayers.contains(clientId))
    {
        std::cerr << mRoomId << "번 게임에 " << clientId << "클라이언트 이동이 요청되었으나 해당 클라이언트는 없습니다." << std::endl;
        return;
    }

    InvokeOnEntityMove(mPlayerControllingPawnIds.at(clientId), location, direction);
}

void GameRoom::InvokeOnEntityMove(EntityId entityId, const Vector& location, double direction) noexcept
{
    if (!mEntities.contains(entityId))
    {
        std::cerr << mRoomId << "번 게임에 ID " << entityId << " 엔티티 이동이 요청되었으나 해당 엔티티는 없습니다." << std::endl;
        return;
    }

    mEntityLocations.at(entityId) = location;
    mEntityDirections.at(entityId) = direction;

    for (auto cIter = mOnlinePlayers.cbegin(); cIter != mOnlinePlayers.cend(); cIter++)
    {
        BattleGameServer::GetConstInstance()
        .GetConstStcRpc()
        .MoveEntity(*cIter, entityId, location, direction);
    }
}

void GameRoom::Tick() noexcept
{

}