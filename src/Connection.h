#ifndef WHEELSONRC_BYTEBOI_CONNECTION_H
#define WHEELSONRC_BYTEBOI_CONNECTION_H


#include <WiFiServer.h>
#include <Loop/LoopListener.h>
#include <JPEGDecoder.h>

class ConnectionListener {
public:
	virtual void connected() = 0;
	virtual void disconnected() = 0;
};

class Connection : public LoopListener {
public:
	Connection();
	virtual ~Connection();

	void start();
	void stop();
	WiFiClient& getFeedClient();
	WiFiClient& getControlClient();
	void setListener(ConnectionListener *listener);
	bool connected();
	void disconnected();

	void loop(uint micros) override;

private:
	WiFiServer feedServer;
	WiFiServer controlServer;
	WiFiClient feedClient;
	WiFiClient controlClient;

	IPAddress localIP;
	IPAddress gateway;
	IPAddress subnet;

	ConnectionListener* listener = nullptr;

	const char* ssid = "WheelsonRC";
	const char* password = "WheelsonRCServer";
};

extern Connection Con;

#endif //WHEELSONRC_BYTEBOI_CONNECTION_H
