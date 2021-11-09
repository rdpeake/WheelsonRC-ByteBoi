#ifndef WHEELSONRC_BYTEBOI_CONNECTION_H
#define WHEELSONRC_BYTEBOI_CONNECTION_H


#include <WiFi.h>
#include <WiFiServer.h>
#include <Loop/LoopListener.h>
#include <JPEGDecoder.h>
#include <esp_event_legacy.h>

class ConnectionListener {
public:
	virtual void connected() = 0;
	virtual void disconnected() = 0;
};

class Connection : public LoopListener {
public:
	Connection();
	virtual ~Connection();

	void setInfo(uint8_t quality = 0, const String& ssid = "", const String& pass = "");
	void start();
	void stop(bool resetInfo = true);

	WiFiClient& getFeedClient();
	WiFiClient& getControlClient();

	void setListener(ConnectionListener *listener);

	bool connected();
	void disconnected();

	void loop(uint micros) override;

	static void onWiFi(WiFiEvent_t e);

	static const char* directSSID;
	static const char* directPass;

	enum State { IDLE, WIFI, WAITING, CONNECTED };
	State getState() const;

private:
	bool isDirect();
	void setupServer();
	void cleanupServer();
	bool serverRunning = false;
	State state = IDLE;

	String ssid;
	String pass;
	uint8_t quality = 0;

	uint32_t wifiTime = 0;

	WiFiServer feedServer;
	WiFiServer controlServer;
	WiFiClient feedClient;
	WiFiClient controlClient;

	IPAddress directIp;
	IPAddress assignedIp;
	IPAddress gateway;
	IPAddress subnet;

	ConnectionListener* listener = nullptr;
};

extern Connection Con;

#endif //WHEELSONRC_BYTEBOI_CONNECTION_H
