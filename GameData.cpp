//
// Created by floweryclover on 2024-01-30.
//

#include "GameData.h"
#include "Message.h"
#include "Context.h"
#include "CtsRpc.h"
#include <iostream>

GameData& GameData::GetInstance()
{
    static GameData instance;
    return instance;
}

void GameData::OnPlayerDisconnected(ConnectionId id)
{
    mClients.erase(id);
    mNicknames.erase(id);
}

void GameData::OnPlayerConnected(ConnectionId id, Client&& client)
{
    mClients.emplace(id, std::move(client));
    mNicknames.emplace(id, std::string("플레이어"));
}

bool GameData::SetPlayerNickname(ConnectionId id, const std::string& nickname)
{
    if (mNicknames.contains(id))
    {
        mNicknames[id] = nickname;
        return true;
    }
    else
    {
        return false;
    }
}

bool GameData::JoinRandomGameRoom(ConnectionId id)
{
    for (const auto& roomPair : mGameRooms)
    {
        if (roomPair.second.IsPlayerJoined(id))
        {
            return false;
        }
    }

    return false;
}

void GameData::AllClientReceive(CtsRpc& ctsRpc)
{

    for (auto iter = mClients.begin(); iter != mClients.end();)
    {
        Client& client = iter->second;

        auto receiveResult = client.Receive();
        if (receiveResult.has_value())
        {
            if (receiveResult.value().has_value())
            {
                Context context(client.GetConnectionId());
                ctsRpc.HandleMessage(context, *receiveResult.value().value());
            }
        }
        else
        {
            if (receiveResult.error().has_value())
            {
                std::cerr << "[에러] 클라이언트 " << client.GetConnectionId() << " 에게서 데이터를 수신하던 도중 에러가 발생하였습니다: 에러 코드 " << receiveResult.error().value() << "." << std::endl;
            }
            std::cout << "[접속 해제] 클라이언트 " << client.GetConnectionId() << std::endl;
            OnPlayerDisconnected(client.GetConnectionId());
            continue;
        }
        iter++;
    }
}