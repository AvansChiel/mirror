#include <asio.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#pragma once
class DataMirrorServer
{
public:
	void startLoop();
private:
	template <typename TP>
	const std::time_t to_time_t(TP tp);
	void put(const std::string& path, const std::string& size, asio::ip::tcp::iostream& client);
	void get(const std::string& path, asio::ip::tcp::iostream& client);
	void del(asio::ip::tcp::iostream& client, const std::string& path);
	void ren(asio::ip::tcp::iostream& client, const std::string& path, const std::string& newname);
	void mkdir(asio::ip::tcp::iostream& client, const std::string& path, const std::string& dirname);
	void dir(asio::ip::tcp::iostream& client, const std::string& path);

	const std::string rootPath = "E:/datamirror/server";
	const int server_port{ 12345 };
	const std::string lf{ "\n" };
	const std::string crlf{ "\r\n" };



};

