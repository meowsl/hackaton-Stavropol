#include <WS2tcpip.h>
#include <WinSock2.h>
#include "../hash/md5.hpp"
#include "../hash/oxorany_bak.h"
#include <string>
#include <iostream>
bool error;
bool succes_sc_legit;
bool succes_sc_rage;
std::vector<uint8_t> bytes;
std::vector<uint8_t> bytes2;
bool sendT;
bool exitth;
bool changeForm;
std::string getHWID() {

	HW_PROFILE_INFO hwProfileInfo;
	GetCurrentHwProfile(&hwProfileInfo);
	std::string hwidWString = hwProfileInfo.szHwProfileGuid;
	std::string hwid(hwidWString.begin(), hwidWString.end());
	return hwid;
}

std::vector<uint8_t> ReceiveData(SOCKET socket)
{
	std::vector<uint8_t> data;
	char buffer[1024];
	char key = oxorany('am3w');
	int bytesReceived = 0;
	int fileSize = 0;
	int bytes = recv(socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
	if (bytes != sizeof(fileSize)) {
		return data;
	}

	while (bytesReceived < fileSize) {
		bytes = recv(socket, buffer, sizeof(buffer), 0);
		if (bytes == SOCKET_ERROR) {
			break;
		}
		if (bytes == 0) {
			break;
		}
		for (int i = 0; i < bytes; i++) {
			buffer[i] ^= key;
		}

		bytesReceived += bytes;
		data.insert(data.end(), buffer, buffer + bytes);
	}
	return data;
}


std::vector<char> receiveAll(int socket, size_t dataSize) {
	std::vector<char> receivedData(dataSize);
	size_t bytesReceived = 0;
	int recvResult = 0;
	while (bytesReceived < dataSize) {
		recvResult = recv(socket, receivedData.data() + bytesReceived, dataSize - bytesReceived, 0);
		if (recvResult == -1) {
			throw std::runtime_error(oxorany("Failed to receive data."));
		}
		bytesReceived += recvResult;
	}
	return receivedData;
}
void xor_encrypt(char* data, size_t len, char key) {
	for (size_t i = 0; i < len; i++) {
		data[i] ^= key;
	}
}
void decryptString(std::string& message, char key) {
	for (size_t i = 0; i < message.length(); i++) {
		message[i] ^= key;
	}
}
bool sendFile(const char* filename, int socket) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "Unable to open file" << std::endl;
		closesocket(socket);
		WSACleanup();
		return false;
	}


	// create vector to store file contents
	std::vector<char> fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	int fileSize = fileContent.size();
	int bytesSent = 0;
	int totalBytesSent = 0;
	int bytesToWrite = fileSize;

	// send the size of the vector as the number of bytes that will be sent
	int sizeBytes = send(socket, reinterpret_cast<const char*>(&fileSize), sizeof(fileSize), 0);
	if (sizeBytes == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(socket);
		WSACleanup();
		return false;
	}
	for (auto it = fileContent.begin(); it != fileContent.end() && bytesToWrite > 0; ) {
		int bytesLeft = bytesToWrite > 1024 ? 1024 : bytesToWrite;
		std::vector<char> buffer(it, it + bytesLeft);
		int bytes = send(socket, buffer.data(), buffer.size(), 0);
		if (bytes == SOCKET_ERROR) {
			std::cerr << "send failed: " << WSAGetLastError() << std::endl;
			closesocket(socket);
			WSACleanup();
			return false;
		}
		bytesToWrite -= bytes;
		totalBytesSent += bytes;
		it += bytes;
	}
	if (totalBytesSent != fileSize) {
		std::cerr << "not all bytes were sent" << std::endl;
		closesocket(socket);
		WSACleanup();
		return false;
	}
	std::cout << "File sent successfully. " << fileSize << " bytes sent." << std::endl;
	return true;
}
bool SendFileToServer(const char* file) {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		return false; // Возвращаем false, так как не удалось инициализировать Winsock
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		WSACleanup();
		return false; // Возвращаем false, так как не удалось создать сокет
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(12345);
	inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

	result = connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
	if (result == SOCKET_ERROR) {
		closesocket(clientSocket);
		WSACleanup();
		return false; // Возвращаем false, так как не удалось установить соединение с сервером
	}

	bool success = sendFile(file, clientSocket);
	if (success) {
		changeForm = true;
	}
	

	return success;
}