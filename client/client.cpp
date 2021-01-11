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
					if (resp.substr(resp.length() - 1, 1) == "\r") {
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
				if (req == "dir") {
					std::string par1;
					std::cout << "type path:";
					if (getline(std::cin, par1)) {
						req += crlf;
						req += par1;
					}
				}else if (req == "mkdir") {
					std::string par1;
					std::string par2;
					std::cout << "type path:";
					if (getline(std::cin, par1)) {
						req += crlf;
						req += par1;
					}
					std::cout << "type new dirname:";
					if (getline(std::cin, par2)) {
						req += crlf;
						req += par2;
					}
				}
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
