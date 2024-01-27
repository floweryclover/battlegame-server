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
    Client* to;
    int headerBodySize;
    int headerType;
    std::unique_ptr<char> body;
};


#endif //BATTLEGAME_SERVER_MESSAGE_H
