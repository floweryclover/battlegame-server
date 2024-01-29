//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_MESSAGE_H
#define BATTLEGAME_SERVER_MESSAGE_H

#include <memory>

class Message {
public:
    Message(const int headerBodySize, const int headerMessageType, const char* const bodyOnlySource);
    ~Message();
    int mHeaderBodySize;
    int mHeaderMessageType;
    char* mBodyBuffer;
};


#endif //BATTLEGAME_SERVER_MESSAGE_H
