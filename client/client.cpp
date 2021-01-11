//
// client.cpp
//
// Created by Bob Polis at 2020-12-02
//

#include <iostream>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <asio.hpp>

int main() {
	try {
		const char* server_address{ "localhost" };
		const char* server_port{ "12345" };
		const char* prompt{ "avansync> " };
		const char* lf{ "\n" };
		const char* crlf{ "\r\n" };
		const std::string rootPath = "E:/datamirror/client";
		bool finished = false;
		asio::ip::tcp::iostream server{ server_address, server_port };
		if (!server) throw std::runtime_error("could not connect to server");

		while (server) {
			std::string resp;
			while (!finished) {
				if (getline(server, resp)) {
					if (resp.substr(resp.length() - 1, 1) == "\r" ) {
						resp.erase(resp.end() - 1); // remove '\r'
						finished = true;
					}
					
					std::cout << resp << lf;
					if (resp == "Bye.") break;

				}
				
			}

			std::cout << prompt;
			std::string req;
			if (getline(std::cin, req)) {
				finished = false;
				server << req << crlf;
			}
		}

	}
	catch (const std::exception& ex) {
		std::cerr << "client: " << ex.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
