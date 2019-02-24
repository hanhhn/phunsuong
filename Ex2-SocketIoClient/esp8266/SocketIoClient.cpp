#include "SocketIoClient.h"

const String getEventName(const String msg)
{
	return msg.substring(4, msg.indexOf("\"", 4));
}

const String getEventPayload(const String msg)
{
	String result = msg.substring(msg.indexOf("\"",4) + 2,msg.length() - 1);
	if(result.startsWith("\""))
	{
		result.remove(0, 1);
	}
	if(result.endsWith("\""))
	{
		result.remove(result.length() - 1);
	}
	return result;
}

void SocketIoClient::webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
	String msg;
	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[SIoC] Disconnected!\n");
			_isConnected = false;
			break;
		case WStype_CONNECTED:
			Serial.printf("[SIoC] Connected to url: %s\n",  payload);
			_webSocket.sendTXT("5");
			_isConnected = true;
			break;
		case WStype_TEXT:
			msg = String((char*)payload);
			if(msg.startsWith("42"))
			{
				trigger(getEventName(msg).c_str(), getEventPayload(msg).c_str(), length);
			}
			else if(msg.startsWith("2"))
			{
				_webSocket.sendTXT("3");
			}
			else if(msg.startsWith("40"))
			{
				trigger("connect", NULL, 0);
			}
			else if(msg.startsWith("41"))
			{
				trigger("disconnect", NULL, 0);
			}
			break;
		case WStype_BIN:
			Serial.printf("[SIoC] get binary length: %u\n", length);
			hexdump(payload, length);
			break;
	}
}

void SocketIoClient::socketEvent(WStype_t type, uint8_t * payload, size_t length)
{
  switch(type) {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!\n");
        _isConnected = false;
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to url: %s\n",  payload);
        _isConnected = true;

        // send message to server when Connected
        // socket.io upgrade confirmation message (required)
        _webSocket.sendTXT("5");
        break;
    case WStype_TEXT:
        Serial.printf("[WSc] get text: %s\n", payload);
        // send message to server
        //webSocket.sendTXT("message got");
        break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u\n", length);
        hexdump(payload, length);

        // send data to server
        // webSocket.sendBIN(payload, length);
        break;
  }
}

bool SocketIoClient::isConnected()
{
	return _isConnected;
}

void SocketIoClient::begin(const char* host, const int port)
{
	_webSocket.beginSocketIO(host, port);
	initialize();
}

void SocketIoClient::initialize()
{
    _webSocket.onEvent(std::bind(&SocketIoClient::socketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}


void SocketIoClient::loop()
{
	_webSocket.loop();
	
	if(!_isConnected)
		return;
	
	uint64_t now = millis();

    if(now - messageTimestamp > MESSAGE_INTERVAL)
    {
      messageTimestamp = now;
  		_webSocket.sendTXT("42[\"message\",{\"hello\":\"hello socketio server\"}]");
  		
  		for(auto packet = _packets.begin(); packet != _packets.end();)
  		{
  			if(_webSocket.sendTXT(*packet))
  			{
  				Serial.printf("[SIoC] packet \"%s\" emitted\n", packet->c_str());
  				packet = _packets.erase(packet);
  			}
  			else
  			{
  			  packet++;
  			}
  		}
    }
	
    if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL)
    {
        heartbeatTimestamp = now;
        //socket.io heartbeat message
        _webSocket.sendTXT("2");
    }
}

void SocketIoClient::on(const char* event, std::function<void (const char * payload, size_t length)> func)
{
	_events[event] = func;
}

void SocketIoClient::emit(const char* event, const char * payload)
{
	if(_isConnected)
	{
		String msg = String("42[\"");
		msg += event;
		msg += "\"";

		if(payload)
		{
			msg += ",";
			msg += payload;
		}
		msg += "]";
		Serial.printf("[SIoC] add packet %s\n", msg.c_str());
		_packets.push_back(msg);
	}
}

void SocketIoClient::trigger(const char* event, const char * payload, size_t length)
{
	auto e = _events.find(event);
	if(e != _events.end())
	{
		Serial.printf("[SIoC] trigger event %s\n", event);
		e->second(payload, length);
	}
	else
	{
		Serial.printf("[SIoC] event %s not found. %d events available\n", event, _events.size());
	}
}

void SocketIoClient::disconnect()
{
	_webSocket.disconnect();
	trigger("disconnect", NULL, 0);
}

void SocketIoClient::setAuthorization(const char * user, const char * password)
{
    _webSocket.setAuthorization(user, password);
}
