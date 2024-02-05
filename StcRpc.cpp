//
// Created by floweryclover on 2024-01-27.
//

#include "StcRpc.h"
#include "Message.h"
#include "BattleGameServer.h"
#include "ClientManager.h"
#include <iostream>

void StcRpc::JoinedGameRoom(ClientId to) const noexcept
{
    Message message(0, STC_JOINED_GAME_ROOM, nullptr);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::DisconnectedFromGame(ClientId to) const noexcept
{
    Message message(0, STC_DISCONNECTED_FROM_GAME, nullptr);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::AssignUdpToken(ClientId to) const noexcept
{
    Message message(8, STC_ASSIGN_UDP_TOKEN, reinterpret_cast<char*>(&to));
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}