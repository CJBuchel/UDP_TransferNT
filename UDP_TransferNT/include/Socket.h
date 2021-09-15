#ifndef SOCKET_H
#define SOCKET_H

#include "nt_headers.h"

namespace UDP_TransferNT {

	/**
	 * Cross platform socket wrapper. Used as main socket for network.
	 */
	class Socket {
	 public:
		void setPort(int port) {
			_port = port;
		}

		void setIP(const char *ip) {
			_ip = ip;
		}

		int getPort() {
			return _port;
		}

		const char *getIP() {
			return _ip;
		}

		void setRecvTimeout(int ms) {
			_recvTimeout = ms;
		}

		int getRecvTimeout() {
			return _recvTimeout;
		}

		struct sockaddr_in &getLocalAddress() {
			return _si_local;
		}

		
		struct sockaddr_in &getOtherAddress() {
			return _si_other;
		}
		
		#ifdef NT_UDP_PLATFORM_WINDOWS
			int *getLocalAddressLength() { _si_local_len = sizeof(_si_local_len); return &_si_local_len; }
			int *getOtherAddressLength() { _si_other_len = sizeof(_si_other); return &_si_other_len; }
		#elif defined(NT_UDP_PLATFORM_UNIX)
			socklen_t *getLocalAddressLength() { _si_local_len = sizeof(_si_local_len); return &_si_local_len; }
			socklen_t *getOtherAddressLength() { _si_other_len = sizeof(_si_other); return &_si_other_len; }
		#endif

		#ifdef NT_UDP_PLATFORM_WINDOWS
			SOCKET &getSocket() { return _socket; }
		#elif defined(NT_UDP_PLATFORM_UNIX)
			int &getSocket() { return _socket; }
		#endif

		int createSocket(bool client = false) {
			_si_other_len = sizeof(_si_other);

			// windows create socket
			#ifdef NT_UDP_PLATFORM_WINDOWS
				if (WSAStartup(MAKEWORD(2,2), &_wsa) != 0) {
					DEFAULT_NT_LOGGER("WSA Startup Failed with error: " + std::to_string(WSAGetLastError()));
					return 1;
				}

				if (client) {
					if ((_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
						DEFAULT_NT_LOGGER("Could not create client socket, error: " + std::to_string(WSAGetLastError()));
						return 1;
					}
				} else {
					std::cout << "Using server windows shit" << std::endl;
					if ((_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
						DEFAULT_NT_LOGGER("Could not create server socket, error: " + std::to_string(WSAGetLastError()));
						return 1;
					}
				}

			// unix create socket
			#elif defined(NT_UDP_PLATFORM_UNIX)
				if ((_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
					DEFAULT_NT_LOGGER("Could not create client socket");
					return 1;
				}
			#endif
			return 0;
		}

		void prepSocketStructure(bool client = false, bool ipSpecific = false) {
			// Resolve address
			if (client) {
				_si_other.sin_family = AF_INET;
				_si_other.sin_port = htons(_port);
				if (ipSpecific) {
					#ifdef NT_UDP_PLATFORM_WINDOWS
						inet_pton(AF_INET, _ip, &_si_other.sin_addr);
					#elif defined(NT_UDP_PLATFORM_UNIX)
						_host = (struct hostent *)gethostbyname((char *)_ip);
						_si_other.sin_addr = *((struct in_addr *)_host->h_addr);
					#endif
				} else {
					_si_other.sin_addr.s_addr = INADDR_ANY;
				}
			} else {
				_si_local.sin_family = AF_INET;
				_si_local.sin_port = htons(_port);
				_si_local.sin_addr.s_addr = htonl(INADDR_ANY);

				_si_other.sin_family = AF_INET;
				_si_other.sin_port = htons(_port);

				if (ipSpecific) {
					#ifdef NT_UDP_PLATFORM_WINDOWS
						inet_pton(AF_INET, _ip, &_si_other.sin_addr);
					#elif defined(NT_UDP_PLATFORM_UNIX)
						_host = (struct hostent *)gethostbyname((char *)_ip);
						_si_other.sin_addr = *((struct in_addr *)_host->h_addr);
					#endif
				} else {
					_si_other.sin_addr.s_addr = INADDR_ANY;
				}
			}
		}

		int bindSocket(bool client = false) {
			#ifdef NT_UDP_PLATFORM_WINDOWS
				if (bind(_socket, (struct sockaddr *)&_si_local, sizeof(_si_local)) == SOCKET_ERROR) {
					DEFAULT_NT_LOGGER("Bind failed, error: " + std::to_string(WSAGetLastError()));
					return 1;
				}
			#elif defined(NT_UDP_PLATFORM_UNIX)
				if (bind(_socket, (struct sockaddr *)&_si_local, sizeof(_si_local)) < 0) {
					DEFAULT_NT_LOGGER("Bind failed");
					return 1;
				}
			#endif
			return 0;
		}

		int connectSocket(bool client = false) {
			#ifdef NT_UDP_PLATFORM_WINDOWS
			#elif defined(NT_UDP_PLATFORM_UNIX)
				if (connect(_socket, (struct sockaddr *)&_si_other, sizeof(_si_other)) < 0) {
					DEFAULT_NT_LOGGER("Socket Connect failed");
					return 1;
				}
			#endif
			return 0;
		}

		void killSocket() {
			#ifdef NT_UDP_PLATFORM_WINDOWS
				closesocket(_socket);
				WSACleanup();
			#elif defined(NT_UDP_PLATFORM_UNIX)
				close(_socket);
			#endif
		}

	 private:
		int _port;
		const char *_ip;

		int _recvTimeout = 1000; // in ms

		struct sockaddr_in _si_local, _si_other; // Server uses both socket addresses. Client uses si_other

		#ifdef NT_UDP_PLATFORM_WINDOWS // windows
		SOCKET _socket;
		WSADATA _wsa;
		int _si_local_len, _si_other_len;
		#elif defined(NT_UDP_PLATFORM_UNIX) // unix
		int _socket;
		struct hostent *_host;
		socklen_t _si_local_len, _si_other_len;
		#endif
	};
}

#endif