//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_CTSRPC_H
#define BATTLEGAME_SERVER_CTSRPC_H

#include <string>
#include "Context.h"
#include "Client.h"
#include "Message.h"

class StcRpc;

class CtsRpc {
public:
    CtsRpc(const StcRpc* const pStcRpc) : mpStcRpc(pStcRpc) {}
    ~CtsRpc() = default;
    void HandleMessage(const Context& context, const Message& message) const;

    static constexpr int CTS_REQUEST_JOIN_GAME = 1;
private:
    void RequestJoinGame(Context context) const;
    const StcRpc* const mpStcRpc;
};


#endif //BATTLEGAME_SERVER_CTSRPC_H
