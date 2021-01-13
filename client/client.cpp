//
// client.cpp
//
// Created by Bob Polis at 2020-12-02
//
#include "DataMirrorClient.h"


int main() {
	try {
		DataMirrorClient client;
		client.startLoop();
	}
	catch (const std::exception& ex) {
		std::cerr << "client: " << ex.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
