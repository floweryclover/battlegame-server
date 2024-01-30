//
// Created by floweryclover on 2024-01-28.
//

#ifndef BATTLEGAME_SERVER_IORESULT_H
#define BATTLEGAME_SERVER_IORESULT_H

enum class IoResult
{
    IO_SUCCESSFUL,
    IO_WOULD_BLOCK,
    IO_FATAL_ERROR,
    IO_DISCONNECTED,
};

#endif //BATTLEGAME_SERVER_IORESULT_H