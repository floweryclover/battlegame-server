//
// Created by floweryclover on 2024-01-28.
//

#ifndef BATTLEGAME_SERVER_CONTEXT_H
#define BATTLEGAME_SERVER_CONTEXT_H


class Context {
public:
    explicit Context(const unsigned int connectionId) : mConnectionId(connectionId) {};
    ~Context() = default;

    inline unsigned int GetConnectionId() const { return mConnectionId; }
private:
    const unsigned int mConnectionId;
};


#endif //BATTLEGAME_SERVER_CONTEXT_H
