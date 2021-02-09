//
// Created by wind on 2021/2/9.
//
#include <signal.h>
#include "MediaServer.h"

int main(){
    //start media server
    MediaServer::Ptr server = make_shared<MediaServer>();
    server->start();

    //set interrupt signal
    static semaphore sem;
    signal(SIGINT, [](int) {
        InfoL << "SIGINT:exit";
        exit(0);
    });
}