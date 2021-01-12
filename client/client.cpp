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

bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

int main() {
	try {
		const char* server_address{ "localhost" };
		const char* server_port{ "12345" };
		const char* prompt{ "avansync> " };
		const char* lf{ "\n" };
		const char* crlf{ "\r\n" };
		const std::string rootPath = "E:/datamirror/client";
		int expectedRows = 1;
		bool expectRowAmount = false;
		asio::ip::tcp::iostream server{ server_address, server_port };
		if (!server) throw std::runtime_error("could not connect to server");

		while (server) {
			std::string resp;
			while (expectedRows > 0) {
				if (getline(server, resp)) {
					if (resp.substr(resp.length() - 1, 1) == "\r") {
						resp.erase(resp.end() - 1); // remove '\r'
						expectedRows--;;
						if (expectRowAmount) {
							if (is_number(resp)) {
								expectedRows = std::stoi(resp);
							}
							expectRowAmount = false;
						}

					}
					
					std::cout << resp << lf;
					if (resp == "Bye.") break;

				}
				
			}
			expectedRows = 1;
			expectRowAmount = false;
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
					expectRowAmount = true;

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
				else if (req == "ren") {
					std::string par1;
					std::string par2;
					std::cout << "type path to change:";
					if (getline(std::cin, par1)) {
						req += crlf;
						req += par1;
					}
					std::cout << "type new name:";
					if (getline(std::cin, par2)) {
						req += crlf;
						req += par2;
					}
				} else if (req == "del") {
					std::string par1;
					std::cout << "type path:";
					if (getline(std::cin, par1)) {
						req += crlf;
						req += par1;
					}
				}
				else if (req == "get") {
					std::string par1;
					std::cout << "type path:";
					if (getline(std::cin, par1)) {
						req += crlf;
						req += par1;
					}
				}

				//finished = false;
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
