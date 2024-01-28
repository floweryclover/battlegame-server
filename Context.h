//
// Created by floweryclover on 2024-01-28.
//

#ifndef BATTLEGAME_SERVER_CONTEXT_H
#define BATTLEGAME_SERVER_CONTEXT_H


class Context {
public:
    Context(const int connectionId) : mConnectionId(connectionId);
    ~Context() = default;
private:
    const int mConnectionId;
};


#endif //BATTLEGAME_SERVER_CONTEXT_H
