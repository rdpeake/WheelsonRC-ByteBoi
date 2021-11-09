#ifndef WHEELSONRC_BYTEBOI_ADVERTISER_H
#define WHEELSONRC_BYTEBOI_ADVERTISER_H

#include <Arduino.h>
#include <BLEServer.h>
#include "Connection.h"

class BLEServer;
class BLEService;
class BLECharacteristic;

struct RCInfo {
	bool direct = true;
	std::string ssid = Connection::directSSID;
	std::string pass = Connection::directPass;
	uint8_t ip[4] = { 10, 0, 0, 4 };
	uint8_t quality = 0;

	RCInfo();
	RCInfo(uint8_t quality);
	RCInfo(const std::string& ssid, const std::string& pass, uint8_t* ip, uint8_t quality = 0);
};

class Advertiser {
public:
	Advertiser();

	void start();
	void stop();

	void setInfo(const RCInfo& info = {});

private:
	RCInfo info;

	BLEServer* server = nullptr;
	BLEService* service = nullptr;
	BLECharacteristic* charDirect = nullptr;
	BLECharacteristic* charWifi = nullptr;
	BLECharacteristic* charIp = nullptr;
	BLECharacteristic* charQuality = nullptr;

};

extern Advertiser Advertise;

#endif //WHEELSONRC_BYTEBOI_ADVERTISER_H
