#pragma once
#include <string>
#include <stdio.h>
#include <winsock.h>
#include <stdlib.h>


namespace http {
	class TcpServer {
	public:
		
		TcpServer(std::string ip_address, int port);
		~TcpServer();
		void startListen();

	private:
		int startServer();
		std::string m_ip_address;
		int m_port;
		SOCKET m_socket;
		SOCKET m_new_socket;
		long m_incomingMessage;
		struct sockaddr_in {
			short			sin_fam;
			unsigned short	sin_port;
			struct in_addr	sin_addr;
			char			sin_zero[8];
		};
		//struct in_addr {
		//	unsigned long s_addr;
		//};
		struct sockaddr_in m_socketAddress;
		int m_socketAddress_len;
		std::string m_serverMessage;
		WSADATA m_wsaData;
		std::string buildResponse();
		void acceptConnection(SOCKET& new_socket);
		void sendResponse();

		void closeServer();
	};
}
