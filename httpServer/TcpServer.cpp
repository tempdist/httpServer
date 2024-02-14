#include "http_TcpServer.h"
#include <iostream>
#include <sstream>


namespace {

	const int BUFFER_SIZE = 30720;
	void log(const std::string& message) {
		std::cout << message << std::endl;
	}

	void exitWithError(const std::string& errorMessage) {
		std::cout << WSAGetLastError() << std::endl;
		log("Error: " + errorMessage);
		exit(1);
	}
}

namespace http {
	TcpServer::TcpServer(std::string ip_address, int port)
		: m_ip_address(ip_address), m_port(port), m_socket(), m_new_socket(),
		m_incomingMessage(), m_socketAddress(), m_socketAddress_len(sizeof(m_socketAddress)),
		m_serverMessage(buildResponse())
	{
		m_socketAddress.sin_fam = AF_INET;
		m_socketAddress.sin_port = htons(m_port);
		m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());
		startServer();
	}
	TcpServer::~TcpServer(){
		closeServer();
	}
	int TcpServer::startServer() {
		if (WSAStartup(MAKEWORD(0, 2), &m_wsaData) != 0) {
			exitWithError("WSA Startup Failed!");
		}

		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (m_socket < 0) {
			exitWithError("Failed to open Socket!");
			return 1;
		}

		if (bind(m_socket, (sockaddr*)&m_socketAddress, m_socketAddress_len) < 0) {
			exitWithError("Cannot connect socket to address.");
			return 1;
		}
		return 0;
	}

	void TcpServer::startListen() {
		if (listen(m_socket, 20) < 0)
			exitWithError("Socket listen failed.");

		std::ostringstream ss;
		ss << "\n*** Listening on ADDRESS: "
			<< inet_ntoa(m_socketAddress.sin_addr)
			<< "PORT: " << ntohs(m_socketAddress.sin_port)
			<< "***\n\n";
		log(ss.str());

		int bytesReceived;
		while (true) {
			log("===== Waiting for new Connection====\n\n\n");
			acceptConnection(m_new_socket);

			char buffer[BUFFER_SIZE] = { 0 };
			bytesReceived = recv(m_new_socket, buffer, BUFFER_SIZE, 0);
			if (bytesReceived < 0)
				exitWithError("Failed to receive bytes from client socket Connection.");
			std::ostringstream ss;
			ss << "------- Received data from client -----\n\n";
			log(ss.str());

			sendResponse();

			closesocket(m_new_socket);
		}
	}

	void TcpServer::acceptConnection(SOCKET& new_socket) {
		new_socket = accept(m_socket, reinterpret_cast<sockaddr *>(&m_socketAddress), &m_socketAddress_len);
		if (new_socket < 0) {
			std::ostringstream ss;
			ss <<
				"Server failed to connect the incoming conncection from ADDRESS: "
				<< inet_ntoa(m_socketAddress.sin_addr) << "; PORT: "
				<< ntohs(m_socketAddress.sin_port);
			exitWithError(ss.str());
		}
	}

	std::string TcpServer::buildResponse() {
		std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
		std::ostringstream ss;
		ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
			<< htmlFile;
		return ss.str();
	}

	void TcpServer::sendResponse() {
		int byteSent;
		long totalBytesSent = 0;
		while (totalBytesSent < m_serverMessage.size()) {
			byteSent = send(m_new_socket, m_serverMessage.c_str(), m_serverMessage.size(), 0);

			if (byteSent < 0)
				break;
			totalBytesSent += byteSent;

			if (totalBytesSent == m_serverMessage.size()) {
				log("----- Server Response sent to the client-------");
			}
			else {
				log("Error sending response to Client.");
			}
		}
	}

	void TcpServer::closeServer() {
		closesocket(m_socket);
		closesocket(m_new_socket);
		WSACleanup();
		exit(0);
	}
	
}
