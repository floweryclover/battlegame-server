//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_MESSAGE_H
#define BATTLEGAME_SERVER_MESSAGE_H

#include <memory>

#ifdef _WIN32
#elifdef linux
using SOCKET = int;
#endif

class Client;

class Message {
public:
    Client* mTo;
    int mHeaderBodySize;
    int mHeaderType;
    std::unique_ptr<char> mBodyBuffer;
};


#endif //BATTLEGAME_SERVER_MESSAGE_H
