//
// server.cpp
//
// Created by Bob Polis at 2020-12-02
//

#include <iostream>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <asio.hpp>
#include <filesystem>

const std::string rootPath = "E:/datamirror/server";
const int server_port{ 12345 };
const char* lf{ "\n" };
const char* crlf{ "\r\n" };

template<typename T>
std::string toString(const T& t) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}

template<typename T>
T fromString(const std::string& s) {
    std::istringstream stream(s);
    T t;
    stream >> t;
    return t;
}

template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
        + system_clock::now());
    return system_clock::to_time_t(sctp);
}

std::string mkdir(const std::string& path, const std::string& dirname) {
    std::string fullPath = rootPath;
    if (path != "" && path != ".") {
        fullPath = rootPath + "/" + path;
    }
    if (dirname == "" || dirname == ".") {
        std::string returnString = "Error: Invalid directory name!";
        return returnString;
    }
    if (!std::filesystem::exists(fullPath)) {
        std::string returnString = "Error: No such directory!";
        return returnString;
    }
    try
    {
        std::filesystem::create_directory(fullPath + "/" + dirname);
        std::string returnString = "OK";
        return returnString;
    }
    catch (const std::exception&)
    {
        std::string returnString = "Error: Failed to create directory";
        return returnString;
    }
}

std::string dir(const std::string& path) {
    using directory_iterator = std::filesystem::directory_iterator;
    std::string resultString = "";
    int counter = 0;
    std::string fullPath = rootPath;
    if (path != "" && path != ".") {
        fullPath = rootPath + "/" + path;
    }
    if (!std::filesystem::exists(fullPath)) {
        std::string returnString = "Error: No such directory!";
        return returnString;
    }
    try {

    
    for (const auto& dirEntry : directory_iterator(fullPath)) {
        counter++;
        std::string type;

        if (dirEntry.is_directory()) {
            resultString += "D|";
        }
        else if(dirEntry.is_regular_file()){
            resultString += "F|";
        }
        else {
            resultString += "*|";
        }
        struct tm newTime;
        std::time_t timestamp = to_time_t<decltype(dirEntry.last_write_time())>(dirEntry.last_write_time());
        localtime_s(&newTime, &timestamp);
        
        resultString += dirEntry.path().filename().string() + "|";
        std::string day = std::to_string(newTime.tm_mday);
        std::string month = std::to_string(newTime.tm_mon + 1);
        std::string year = std::to_string(newTime.tm_year + 1900);

        std::string hour = std::to_string(newTime.tm_hour);
        std::string min = std::to_string(newTime.tm_min);
        std::string sec = std::to_string(newTime.tm_sec);
        resultString += day + "-" + month + "-" + year + " " + hour + ":" + min + ":" + sec + "|";

        std::string filesize;
        if (dirEntry.file_size() < 0) {
            filesize = "0";
        }
        else {
            filesize = std::to_string(dirEntry.file_size());
        }
        resultString += filesize + "";
        resultString += lf;
    }
    }
    catch (const std::exception& ex) {
        std::string returnString = "Error: No such directory!";
        return returnString;
    }
    std::string prefix = "items found: " + std::to_string(counter) + lf;
    resultString = prefix + resultString;
    //remove last newline
    resultString.erase(resultString.end() - 1);
    return resultString;
}

int main() {
    try {



        asio::io_context io_context;
        asio::ip::tcp::acceptor server{ io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), server_port) };
        for (;;) {
            std::cerr << "waiting for client to connect\n";
            asio::ip::tcp::iostream client;
            server.accept(client.socket());
            std::cerr << "client connected from " << client.socket().local_endpoint() << lf;
            client << "Welcome to AvanSync server 1.0" << crlf;
                for (;;) {
                    std::string request;
                    getline(client, request);
                    request.erase(request.end() - 1); // remove '\r'
                    //if (request.substr(request.length() - 1, 1) == "\r") {
                    //    request.erase(request.end() - 1); // remove '\r'
                    //    finished = true;
                    //}

                    std::cerr << "client says: " << request << lf;

                    if (request == "quit") {
                        client << "Bye." << crlf;
                        std::cerr << "will disconnect from client " << client.socket().local_endpoint() << lf;
                        break;
                    }else if (request == "info") {
                        client << "Data Mirror server 1.0, copyright (c) 2021 Chiel Arts." << crlf;
                    }
                    else if (request == "dir") {
                        getline(client, request);
                        request.erase(request.end() - 1);
                        client << dir(request) << crlf;
                    }
                    else if (request == "mkdir") {
                        getline(client, request);
                        request.erase(request.end() - 1);
                        std::string parent = request;
                        getline(client, request);
                        request.erase(request.end() - 1);
                        std::string dirname = request;
                        client << mkdir(parent, dirname) << crlf;
                    }
                    else {
                        client << request << crlf; // simply echo the request
                    }
            }
            

        }

    }
    catch (const std::exception& ex) {
        std::cerr << "server: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
