#ifndef __SOCKET_IO_CLIENT_H__
#define __SOCKET_IO_CLIENT_H__

#include <Arduino.h>
#include <map>
#include <vector>
#include <WebSocketsClient.h>

#ifdef SOCKETIOCLIENT_USE_SSL
	#define DEFAULT_PORT 443
#else
	#define DEFAULT_PORT 80
#endif
#define DEFAULT_URL "/"
#define DEFAULT_FINGERPRINT ""


#define MESSAGE_INTERVAL 20000
#define HEARTBEAT_INTERVAL 1000


class SocketIoClient {
private:
	bool _isConnected = false;
	uint64_t messageTimestamp = 0;
	uint64_t heartbeatTimestamp = 0;

	std::vector<String> _packets;
	WebSocketsClient _webSocket;
	std::map<String, std::function<void (const char * payload, size_t length)>> _events;
	void trigger(const char* event, const char * payload, size_t length);
	void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
	void socketEvent(WStype_t type, uint8_t * payload, size_t length);
  void initialize();
	
public:
	bool isConnected();
  void begin(const char* host, const int port = DEFAULT_PORT);
	void loop();
	void on(const char* event, std::function<void (const char * payload, size_t length)>);
	void emit(const char* event, const char * payload = NULL);
	void disconnect();
	void setAuthorization(const char * user, const char * password);
};

#endif
