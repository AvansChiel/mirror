#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <asio.hpp>
#include <filesystem>
#include <fstream>
#include "File.h"
class DataMirrorClient
{

public:
	void startLoop();
private:
	template <typename TP>
	const std::time_t to_time_t(TP tp);
	bool isNumber(const std::string& s);
	const std::time_t getDateTimeFromString(std::string datetime);
	const std::string put(asio::ip::tcp::iostream& server, std::string path = "");
	void get(asio::ip::tcp::iostream& server);
	const std::string del(asio::ip::tcp::iostream& server, std::string path = "");
	const std::string ren(asio::ip::tcp::iostream& server);
	const std::vector<std::string> dir(asio::ip::tcp::iostream& server, std::string path = "");
	const std::string mkdir(asio::ip::tcp::iostream& server, std::string path = "");
	const std::string info(asio::ip::tcp::iostream& server);
	void checkServerFiles(asio::ip::tcp::iostream& server, const std::string& path, std::vector<File>& files);
	void checkLocalFiles(const std::string& rootpath, const std::string& path, std::vector<File>& files);
	void sync(asio::ip::tcp::iostream& server);

	const std::string server_address{ "localhost" };
	const std::string server_port{ "12345" };
	const std::string prompt{ "avansync> " };
	const std::string lf{ "\n" };
	const std::string crlf{ "\r\n" };
	const std::string rootPath = "E:/datamirror/client";
	int expectedRows = 1;
	bool expectRowAmount = false;
	bool receiveFile = false;
	//std::string writePath = "";
	//std::string sendPath = "";
	//std::string req;
};

