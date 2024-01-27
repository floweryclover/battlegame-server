//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_CONSTANTS_H
#define BATTLEGAME_SERVER_CONSTANTS_H

#ifdef linux
constexpr int INVALID_SOCKET = -1;
#endif

constexpr int MAX_MESSAGE_SIZE = 1024;
constexpr int HEADER_SIZE = 8;

#endif //BATTLEGAME_SERVER_CONSTANTS_H
