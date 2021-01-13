//
// server.cpp
//
// Created by Bob Polis at 2020-12-02
//

#include "DataMirrorServer.h"

int main() {
    try {
        DataMirrorServer server;
        server.startLoop();
    }
    catch (const std::exception& ex) {
        std::cerr << "server: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
