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

void StcRpc::SetTimer(ClientId to, unsigned short seconds, const std::string& text) const noexcept
{
    if (text.length() > 30)
    {
        std::cerr << "SetTimer()의 text 길이가 너무 깁니다." << std::endl;
        return;
    }

    char serialized[32];
    memcpy(serialized, &seconds, 2);
    memcpy(serialized+2, text.data(), text.length());
    Message message(2+text.length(), STC_SET_TIMER, serialized);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::SignalGameState(ClientId to, int signal) const noexcept
{
    Message message(4, STC_SIGNAL_GAME_STATE, reinterpret_cast<char*>(&signal));
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::RespawnEntity(ClientId to, int entityId, const Vector &location, double direction) const noexcept
{
    char serialized[36];
    memcpy(serialized, &entityId, 4);
    memcpy(serialized+4, &location.X, 8);
    memcpy(serialized+12, &location.Y, 8);
    memcpy(serialized+20, &location.Z, 8);
    memcpy(serialized+28, &direction, 8);
    Message message(36, STC_RESPAWN_ENTITY, serialized);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));

}

void StcRpc::SetScore(ClientId to, int team, int score) const noexcept
{
    char serialized[8];
    memcpy(serialized, &team, 4);
    memcpy(serialized+4, &score, 4);
    Message message(8, STC_SET_SCORE, serialized);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::AssignTeamId(ClientId to, int teamId) const noexcept
{
    Message message(4, STC_ASSIGN_TEAM_ID, reinterpret_cast<char*>(&teamId));
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::AssignEntityNickname(ClientId to, int entityId, const std::string& nickname) const noexcept
{
    char serialized[4+24];
    memcpy(serialized, &entityId, 4);
    memcpy(serialized+4, nickname.c_str(), nickname.length());
    Message message(4+nickname.length(), STC_ASSIGN_ENTITY_NICKNAME, serialized);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::GetMyNickname(ClientId to, const std::string& nickname) const noexcept
{
    char serialized[24];
    memcpy(serialized, nickname.c_str(), nickname.length());
    Message message(nickname.length(), STC_GET_MY_NICKNAME, serialized);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::SendGameData(ClientId to, const std::string &yourNickname, const std::string &opponentNickname) const noexcept
{
    char serialized[4+25+4+25];
    int yourLen = yourNickname.length()+1;
    int opponentLen = opponentNickname.length()+1;
    memcpy(serialized, &yourLen, 4);
    memcpy(serialized+4, yourNickname.c_str(), yourLen);
    memcpy(serialized+4+yourLen, &opponentLen, 4);
    memcpy(serialized+4+yourLen+4, opponentNickname.c_str(), opponentLen);

    Message message(4+yourLen+4+opponentLen, STC_SEND_GAME_DATA, serialized);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}

void StcRpc::SendGameResult(ClientId to, bool isGoodGame, bool isWinner, int myScore, int opponentScore) const noexcept
{
    char serialized[10];
    memcpy(serialized, &isGoodGame, 1);
    memcpy(serialized+1, &isWinner, 1);
    memcpy(serialized+2, &myScore, 4);
    memcpy(serialized+6, &opponentScore, 4);

    Message message(10, STC_SEND_GAME_RESULT, serialized);
    BattleGameServer::GetInstance().GetClientManager().RequestSendMessage(MessageReliability::RELIABLE, to, std::move(message));
}