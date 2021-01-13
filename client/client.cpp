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
#include <filesystem>
#include <fstream>

const char* server_address{ "localhost" };
const char* server_port{ "12345" };
const char* prompt{ "avansync> " };
const char* lf{ "\n" };
const char* crlf{ "\r\n" };
const std::string rootPath = "E:/datamirror/client";
int expectedRows = 1;
bool expectRowAmount = false;
bool receiveFile = false;
std::string writePath = "";
std::string sendPath = "";
std::string req;


bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();


}

void sendFileToServer(asio::ip::tcp::iostream& server) {
	if (sendPath.substr(0, 1) == "." || sendPath.substr(0, 1) == "/") {
		expectedRows = 0;

		std::cout << "Error: Permission denied" << lf;
		return;
	}
	if (!std::filesystem::exists(rootPath + "/" + sendPath)) {
		expectedRows = 0;

		std::cout << "Error: file not found" << lf;
		return;
	}

	try
	{
		server << "put" << crlf;
		server << sendPath << crlf;
		server << std::filesystem::file_size(rootPath + "/" + sendPath) << crlf;
		std::ifstream input(rootPath + "/" + sendPath, std::ios::binary);
		std::string reply;
		char buf[512];
		while (input.read(buf, sizeof(buf)).gcount() > 0)
			reply.append(buf, input.gcount());

		server << reply;

	}
	catch (const std::exception& e)
	{
		server << "Error: Failed to get file" << crlf;
		return;
	}
}

std::string put(asio::ip::tcp::iostream& server) {
	std::string resp;
	bool done = false;
	std::string path;
	std::cout << "type path:";
	if (getline(std::cin, path)) {
		sendPath = path;
		//sendFileToServer(server);
		//req = "";
	}

	server << "put" << crlf << sendPath << crlf << std::filesystem::file_size(rootPath + "/" + sendPath) << crlf;
	std::ifstream input(rootPath + "/" + sendPath, std::ios::binary);
	std::string reply;
	char buf[512];
	while (input.read(buf, sizeof(buf)).gcount() > 0)
		reply.append(buf, input.gcount());

	server << reply;
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);
			done = true;
		}
	}

	return resp;
}

void get(asio::ip::tcp::iostream& server) {
	bool done = false;
	std::string resp;
	receiveFile = true;
	std::string par1;
	std::cout << "type path:";
	if (getline(std::cin, par1)) {
		req += crlf;
		req += par1;
	}

	server << "get" << crlf << par1 << crlf;
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);//remove r
			int byteAmount = std::stoi(resp) + 1;
			char* buffer = new char[byteAmount];
			byteAmount--;
			buffer[byteAmount] = '\0';

			server.read(buffer, byteAmount);

			std::ofstream ofs;
			ofs.open(rootPath + "/" +  par1);
			ofs << buffer;	
			ofs.close();
			done = true;

		}
	}
	std::cout << "file received" << lf;
}

std::string del(asio::ip::tcp::iostream& server) {
	bool done = false;
	std::string resp;
	std::string par1;
	std::cout << "type path:";
	if (getline(std::cin, par1)) {
		req += crlf;
		req += par1;
	}
	server << "del" << crlf << par1 << crlf;
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);
			done = true;
		}
	}

	return resp;
}

std::string ren(asio::ip::tcp::iostream& server) {
	std::string resp;
	bool done = false;
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
	server << "ren" << crlf << par1 << crlf << par2 << crlf;
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);
			done = true;
		}
	}
	return resp;
}

std::vector<std::string> dir(asio::ip::tcp::iostream& server) {
	std::string resp;
	bool done = false;
	std::string par1;
	std::cout << "type path:";
	int linesAmount = 0;
	std::vector<std::string> records;
	if (getline(std::cin, par1)) {
		//req += crlf;
		//req += par1;
	}
	server << "dir" << crlf << par1 << crlf;
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);

			linesAmount = std::stoi(resp);
			while (linesAmount > 0) {
				if (getline(server, resp)) {
					resp.erase(resp.end() - 1);//remove r
					records.push_back(resp);
					linesAmount--;
				}
			}
			done = true;
		}
	}
	return records;
}

std::string mkdir(asio::ip::tcp::iostream& server) {
	std::string resp;
	bool done = false;
	std::string par1;
	std::string par2;
	std::cout << "type path:";
	if (getline(std::cin, par1)) {
		//req += crlf;
		req += par1;
	}
	std::cout << "type new dirname:";
	if (getline(std::cin, par2)) {
		//req += crlf;
		req += par2;
	}
	server << "mkdir" << crlf << par1 << crlf << par2 << crlf;
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);//remove r
			done = true;
		}
	}
	return resp;
}

std::string info(asio::ip::tcp::iostream& server) {
	std::string resp;
	bool done = false;
	server << "info" << crlf;
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);//remove r
			done = true;
		}
	}
	return resp;
}

void checkFolder(const std::string& path) {
	//using directory_iterator = std::filesystem::directory_iterator;
	//std::string fullPath = rootPath;

	//try {
	//	for (const auto& dirEntry : directory_iterator(fullPath)) {
	//		counter++;
	//		std::string type;

	//		if (dirEntry.is_directory()) {
	//			resultString += "D|";
	//		}
	//		else if (dirEntry.is_regular_file()) {
	//			resultString += "F|";
	//		}
	//		else {
	//			resultString += "*|";
	//		}
	//		struct tm newTime;
	//		std::time_t timestamp = to_time_t<decltype(dirEntry.last_write_time())>(dirEntry.last_write_time());
	//		localtime_s(&newTime, &timestamp);

	//		resultString += dirEntry.path().filename().string() + "|";
	//		

	//		std::string filesize;
	//		if (dirEntry.file_size() < 0) {
	//			filesize = "0";
	//		}
	//		else {
	//			filesize = std::to_string(dirEntry.file_size());
	//		}
	//		resultString += filesize + "";
	//		resultString += crlf;
	//	}
	//}
	//catch (const std::exception& ex) {
	//	client << "Error: failed" << crlf;
	//	return;
	//}
}

void sync() {
	//find files in own space
	checkFolder(rootPath);

}

int main() {
	try {
		
		asio::ip::tcp::iostream server{ server_address, server_port };
		if (!server) throw std::runtime_error("could not connect to server");
		bool first = true;
		
		while (server) {
			//std::string resp;
			//while (expectedRows > 0) {
			//	if (writePath != "") {
			//		std::string binaryData;
			//		if (getline(server, resp)) {
			//			resp.erase(resp.end() - 1);//remove r
			//			int byteAmount = std::stoi(resp) + 1;
			//			char* buffer = new char[byteAmount];
			//			byteAmount--;
			//			buffer[byteAmount] = '\0';

			//			server.read(buffer, byteAmount);

			//			std::ofstream ofs;
			//			ofs.open(rootPath + "/" +  writePath);
			//			ofs << buffer;	
			//			ofs.close();
			//			//expectedRows
			//			expectedRows--;
			//			//getline(server, resp);
			//			writePath = "";
			//		}
			//	} else if (getline(server, resp)) {
			//		resp.erase(resp.end() - 1); // remove '\r'
			//		expectedRows--;
			//		if (expectRowAmount) {
			//			if (is_number(resp)) {
			//				expectedRows = std::stoi(resp);
			//			}
			//			expectRowAmount = false;
			//		}
			//		std::cout << resp << lf;
			//		if (resp == "Bye.") break;
			//	}
			//	
			//}
			//expectedRows = 1;
			//expectRowAmount = false;
			if (first) {
				std::string welcome;
				if (getline(server, welcome)) {
					welcome.erase(welcome.end() - 1);
					std::cout << welcome << lf;
					first = false;
				}
			}
			std::cout << prompt;
			if (getline(std::cin, req)) {
				if (req == "dir") {
					for (std::string record : dir(server)) {
						std::cout << record << lf;
					}

				}else if (req == "mkdir") {
					std::cout << mkdir(server) << lf;
				}
				else if (req == "ren") {
					std::cout << ren(server) << lf;
				} else if (req == "del") {
					std::cout << del(server) << lf;
				}
				else if (req == "get") {
					get(server);
				}
				else if (req == "put") {
					std::cout << put(server) << lf;
				}
				else if (req == "info") {
					std::cout << info(server) << lf;
				}
				
			}
		}

	}
	catch (const std::exception& ex) {
		std::cerr << "client: " << ex.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
