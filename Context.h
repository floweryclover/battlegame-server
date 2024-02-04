//
// Created by floweryclover on 2024-01-28.
//

#ifndef BATTLEGAME_SERVER_CONTEXT_H
#define BATTLEGAME_SERVER_CONTEXT_H

#include "Enums.h"

using SerializedEndpoint = unsigned long long;
using ClientId = SerializedEndpoint;

class Context {
public:
    explicit Context(ClientId clientId, SendReliability reliability) : mClientId(clientId), mReliability(reliability) {};
    ~Context() = default;

    inline ClientId GetClientId() const { return mClientId; }
    inline SendReliability GetReliability() const { return mReliability; }
private:
    const ClientId mClientId;
    const SendReliability mReliability;
};


#endif //BATTLEGAME_SERVER_CONTEXT_H
