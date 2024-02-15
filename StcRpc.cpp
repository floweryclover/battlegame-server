//
// Created by floweryclover on 2024-01-27.
//

#include "StcRpc.h"
#include "Message.h"
#include "BattleGameServer.h"
#include "ClientManager.h"
#include <iostream>

void StcRpc::OpenLevel(ClientId to, int level) const noexcept
{
    Message message(4, STC_OPEN_LEVEL, reinterpret_cast<char*>(&level));
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::DisconnectedFromGame(ClientId to) const noexcept
{
    Message message(0, STC_DISCONNECTED_FROM_GAME, nullptr);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::SpawnEntity(ClientId to, int entityId, const Vector& location, double direction) const noexcept
{
    char serialized[36];
    memcpy(serialized, &entityId, 4);
    memcpy(serialized+4, &location.X, 8);
    memcpy(serialized+12, &location.Y, 8);
    memcpy(serialized+20, &location.Z, 8);
    memcpy(serialized+28, &direction, 8);
    Message message(36, STC_SPAWN_ENTITY, serialized);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::DespawnEntity(ClientId to, int entityId) const noexcept
{
    Message message(4, STC_DESPAWN_ENTITY, reinterpret_cast<char*>(&entityId));
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::PossessEntity(ClientId to, int entityId) const noexcept
{
    Message message(4, STC_POSSESS_ENTITY, reinterpret_cast<char*>(&entityId));
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::MoveEntity(ClientId to, int entityId, const Vector &location, double direction) const noexcept
{
    char serialized[36];
    memcpy(serialized, &entityId, 4);
    memcpy(serialized+4, &location.X, 8);
    memcpy(serialized+12, &location.Y, 8);
    memcpy(serialized+20, &location.Z, 8);
    memcpy(serialized+28, &direction, 8);
    Message message(36, STC_MOVE_ENTITY, serialized);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::UNRELIABLE, to, std::move(message));
}

void StcRpc::SetTimer(ClientId to, unsigned short seconds) const noexcept
{
    Message message(2, STC_SET_TIMER, reinterpret_cast<char*>(&seconds));
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}