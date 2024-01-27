//
// Created by floweryclover on 2024-01-25.
//

#ifndef BATTLEGAME_SERVER_WSA_H
#define BATTLEGAME_SERVER_WSA_H

#ifdef _WIN32

class Wsa {
private:
    static bool isWsaStartedUp;
public:
    Wsa();
    ~Wsa();
};


#endif //BATTLEGAME_SERVER_WSA_H
#endif
