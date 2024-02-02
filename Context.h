//
// Created by floweryclover on 2024-01-28.
//

#ifndef BATTLEGAME_SERVER_CONTEXT_H
#define BATTLEGAME_SERVER_CONTEXT_H

using SerializedEndpoint = unsigned long long;
using ClientId = SerializedEndpoint;

class Context {
public:
    explicit Context(const ClientId clientId) : mClientId(clientId) {};
    ~Context() = default;

    inline ClientId GetClientId() const { return mClientId; }
private:
    const ClientId mClientId;
};


#endif //BATTLEGAME_SERVER_CONTEXT_H
