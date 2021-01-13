#include "DataMirrorClient.h"

template <typename TP>
const std::time_t DataMirrorClient::to_time_t(TP tp)
{
	using namespace std::chrono;
	auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
		+ system_clock::now());
	return system_clock::to_time_t(sctp);
}

bool DataMirrorClient::isNumber(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

const std::time_t DataMirrorClient::getDateTimeFromString(std::string datetime) {
	const std::string temp = datetime;
	struct std::tm tm;
	std::stringstream ss(datetime);
	ss >> std::get_time(&tm, "%d-%m-%Y %H:%M:%S");
	std::time_t time = mktime(&tm);
	return time;
}

const std::string DataMirrorClient::put(asio::ip::tcp::iostream& server, std::string path) {
	std::string resp;
	bool done = false;
	std::string filepath;
	if (path == "") {
		std::cout << "type path:";
		if (getline(std::cin, filepath)) {
			//sendPath = filepath;
			//sendFileToServer(server);
			//req = "";
		}
		if (filepath.substr(0, 1) == "." || filepath.substr(0, 1) == "/") {
			return "Error: Permission denied";
		}
		if (!std::filesystem::exists(rootPath + "/" + filepath)) {
			return "Error: file not found";
		}

	}
	else {
		filepath = path;
		if (filepath.substr(0, 2) == "./") {
			filepath = filepath.substr(2, filepath.length());
		}
	}
	int fileSize = std::filesystem::file_size(rootPath + "/" + filepath);
	server << "put" << crlf << filepath << crlf << std::filesystem::file_size(rootPath + "/" + filepath) << crlf;
	std::ifstream input(rootPath + "/" + filepath, std::ios::binary);
	std::vector<char> buffer(fileSize);;
	input.read(buffer.data(), fileSize);

	server.write(buffer.data(), fileSize);

	//server << reply;
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);
			done = true;
		}
	}

	return resp;
}

void DataMirrorClient::get(asio::ip::tcp::iostream& server) {
	bool done = false;
	std::string resp;
	receiveFile = true;
	std::string par1;
	std::cout << "type path:";
	if (getline(std::cin, par1)) {
		//req += crlf;
		//req += par1;
	}

	server << "get" << crlf << par1 << crlf;
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);//remove r
			if (!isNumber(resp)) {
				std::cout << resp << lf;
				return;
			}
			int byteAmount = std::stoi(resp);
			std::vector<char> buffer(byteAmount);
			server.read(buffer.data(), byteAmount);

			std::ofstream ofs;
			ofs.open(rootPath + "/" + par1, std::ios::out | std::ios::trunc | std::ios::binary);
			ofs.write(buffer.data(), byteAmount);
			ofs.close();
			done = true;

		}
	}
	std::cout << "file received" << lf;
}

const std::string DataMirrorClient::del(asio::ip::tcp::iostream& server, std::string path) {
	bool done = false;
	std::string resp;
	std::string par1;
	if (path == "") {
		std::cout << "type path:";
		if (getline(std::cin, par1)) {
			//req += crlf;
			//req += par1;
		}
	}
	else {
		par1 = path;
		if (par1.substr(0, 2) == "./") {
			par1 = par1.substr(2, par1.length());
		}
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

const std::string DataMirrorClient::ren(asio::ip::tcp::iostream& server) {
	std::string resp;
	bool done = false;
	std::string par1;
	std::string par2;
	std::cout << "type path to change:";
	if (getline(std::cin, par1)) {
		//req += crlf;
		//req += par1;
	}
	std::cout << "type new name:";
	if (getline(std::cin, par2)) {
		//req += crlf;
		//req += par2;
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

const std::vector<std::string> DataMirrorClient::dir(asio::ip::tcp::iostream& server, std::string path) {
	std::string resp;
	bool done = false;
	std::string pathparam;
	int linesAmount = 0;
	std::vector<std::string> records;

	if (path == "") {
		std::string par1;
		std::cout << "type path:";
		if (getline(std::cin, pathparam)) {
			//req += crlf;
			//req += par1;
		}
	}
	else {
		pathparam = path;
	}
	server << "dir" << crlf << pathparam << crlf;
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);
			if (!isNumber(resp)) {
				std::cout << resp << lf;
				return records;
			}
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

const std::string DataMirrorClient::mkdir(asio::ip::tcp::iostream& server, std::string path) {
	std::string resp;
	bool done = false;
	std::string par1;
	std::string par2;
	if (path == "") {
		std::cout << "type path:";
		if (getline(std::cin, par1)) {
			//req += crlf;
			//req += par1;
		}
		std::cout << "type new dirname:";
		if (getline(std::cin, par2)) {
			//req += crlf;
			//req += par2;
		}
		server << "mkdir" << crlf << par1 << crlf << par2 << crlf;
	}
	else {
		std::string pathPart = path.substr(0, path.find_last_of("/"));
		std::string namePart = path.substr(path.find_last_of("/") + 1, path.length());
		server << "mkdir" << crlf << pathPart << crlf << namePart << crlf;
	}
	while (!done) {
		if (getline(server, resp)) {
			resp.erase(resp.end() - 1);//remove r
			done = true;
		}
	}
	return resp;
}

const std::string DataMirrorClient::info(asio::ip::tcp::iostream& server) {
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

void DataMirrorClient::checkServerFiles(asio::ip::tcp::iostream& server, const std::string& path, std::vector<File>& files) {
	std::string printString = "";
	std::vector<std::string> filestructure = dir(server, path);

	for (int i = 0; i < filestructure.size(); i++) {
		std::string record = filestructure[i];
		std::string delimiter = "|";

		std::string type = record.substr(0, record.find(delimiter));
		record.erase(0, record.find(delimiter) + 1);
		std::string filepath = record.substr(0, record.find(delimiter));
		record.erase(0, record.find(delimiter) + 1);
		std::string datetime = record.substr(0, record.find(delimiter));
		record.erase(0, record.find(delimiter) + 1);
		std::string filesize = record.substr(0, record.length());
		record.erase(0, record.find(delimiter) + 1);

		File file = { type, path + "/" + filepath, getDateTimeFromString(datetime), std::stoi(filesize) };
		files.push_back(file);

		if (type == "D") {
			checkServerFiles(server, path + "/" + filepath, files);
		}

	}

}

void DataMirrorClient::checkLocalFiles(const std::string& rootpath, const std::string& path, std::vector<File>& files) {
	using directory_iterator = std::filesystem::directory_iterator;

	for (const auto& dirEntry : directory_iterator(rootPath + "/" + path)) {
		std::string type;
		std::string filepath;
		std::time_t last_modified;
		int size;

		if (dirEntry.is_directory()) {
			type = "D";
		}
		else if (dirEntry.is_regular_file()) {
			type = "F";
		}
		else {
			type = "*";
		}
		struct tm newTime;
		last_modified = to_time_t<decltype(dirEntry.last_write_time())>(dirEntry.last_write_time());
		filepath = dirEntry.path().filename().string();


		if (dirEntry.file_size() < 0) {
			size = 0;
		}
		else {
			size = dirEntry.file_size();
		}
		File file = { type, path + "/" + filepath, last_modified, size };
		files.push_back(file);

		if (type == "D") {
			checkLocalFiles(rootPath, path + "/" + filepath, files);
		}
	}


}

void DataMirrorClient::sync(asio::ip::tcp::iostream& server) {
	//find files in own space
	std::vector<File> serverFiles;
	std::vector<File> localFiles;
	//std::vector<File> mkdirQueue;
	std::vector<File> putQueue;
	std::vector<File> deleteQueue;
	checkServerFiles(server, ".", serverFiles);
	checkLocalFiles(rootPath, ".", localFiles);

	//loop through local files
	for (int i = 0; i < localFiles.size(); i++) {
		bool found = false;
		//loop through server files
		for (int j = 0; j < serverFiles.size(); j++) {
			//if file found on both
			if (localFiles[i].path == serverFiles[j].path) {
				found = true;
				//check if client is newer
				if (localFiles[i].lastModified > serverFiles[j].lastModified) {
					//if newer put in queue to update
					putQueue.push_back(localFiles[i]);
				}
			}
		}
		//if local file not on server, put in queue to send it;
		if (!found) {
			putQueue.push_back(localFiles[i]);
		}
	}

	//loop through server files
	for (int x = 0; x < serverFiles.size(); x++) {
		bool found = false;
		//loop through server files
		for (int y = 0; y < localFiles.size(); y++) {
			//if file found on both
			if (localFiles[y].path == serverFiles[x].path) {
				found = true;

			}
		}
		//if local file not on server, put in queue to send it;
		if (!found) {
			deleteQueue.push_back(serverFiles[x]);
		}
	}

	//create or overwrite file/directory's in the putqueue
	for (File f : putQueue) {
		if (f.type == "D") {
			mkdir(server, f.path);
		}
		else {
			put(server, f.path);
		}
	}

	//delete files or directories in the deletequeue
	for (int k = deleteQueue.size() - 1; k >= 0; k--) {
		del(server, deleteQueue[k].path);
	}
	std::cout << "Done Syncing" << lf;
}



void DataMirrorClient::startLoop()
{
	asio::ip::tcp::iostream server{ server_address, server_port };
	if (!server) throw std::runtime_error("could not connect to server");
	bool first = true;
	std::string req;
	while (server) {
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
			}
			else if (req == "mkdir") {
				std::cout << mkdir(server) << lf;
			}
			else if (req == "ren") {
				std::cout << ren(server) << lf;
			}
			else if (req == "del") {
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
			else if (req == "sync") {
				sync(server);
			}
			else if (req == "quit") {
				server << "quit" << crlf;
				server.close();
				break;
			}

		}
	}
}
