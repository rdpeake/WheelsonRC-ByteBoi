#include "Advertiser.h"
#include <BLEServer.h>
#include <BLEDevice.h>

#define SERVICE_UUID     "d8771990-683b-4bc7-9651-8601be142e94"
#define CHAR_DIRECT_UUID "8084b55f-d8b3-46c5-acaa-5c99809ed448"
#define CHAR_WIFI_UUID   "bc137c7c-408b-402d-ae4d-009d6e008bcf"
#define CHAR_IP_UUID     "a8a92cf9-3c1a-48c9-8079-29a0a334b879"
#define CHAR_QUALITY_UUID     "7d9917e0-aeee-44c4-958e-684b08dd76e2"

RCInfo::RCInfo(){}

RCInfo::RCInfo(uint8_t quality) : quality(quality){ }

RCInfo::RCInfo(const std::string& ssid, const std::string& pass, uint8_t* ip, uint8_t quality) : direct(false), ssid(ssid), pass(pass), quality(quality){
	memcpy(this->ip, ip, 4);
}

Advertiser Advertise;

Advertiser::Advertiser(){

}

void Advertiser::start(){
	stop();

	if(!BLEDevice::getInitialized()){
		BLEDevice::init("Wheelson RC");
	}

	if(!server){
		server = BLEDevice::createServer();
		server->setCallbacks(nullptr);
	}

	service = server->createService(SERVICE_UUID);

	charDirect = service->createCharacteristic(CHAR_DIRECT_UUID, BLECharacteristic::PROPERTY_READ);
	charWifi = service->createCharacteristic(CHAR_WIFI_UUID, BLECharacteristic::PROPERTY_READ);
	charIp = service->createCharacteristic(CHAR_IP_UUID, BLECharacteristic::PROPERTY_READ);
	charQuality = service->createCharacteristic(CHAR_QUALITY_UUID, BLECharacteristic::PROPERTY_READ);

	setInfo(info);

	service->start();

	// BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
	/*BLEAdvertising* advertising = BLEDevice::getAdvertising();
	advertising->addServiceUUID(SERVICE_UUID);
	advertising->setScanResponse(true);
	advertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
	advertising->setMinPreferred(0x12);
	BLEDevice::startAdvertising();*/

	server->getAdvertising()->start();
}

void Advertiser::stop(){
	if(!BLEDevice::getInitialized()) return;

	if(server){
		server->getAdvertising()->stop();
	}

	if(service){
		service->stop();

		if(server){
			server->removeService(service);
		}
	}

	delete service;
	delete charDirect;
	delete charWifi;
	delete charIp;
	delete charQuality;

	service = nullptr;
	charDirect = charWifi = charIp = charQuality = nullptr;
}

void Advertiser::setInfo(const RCInfo& newInfo){
	Advertiser::info = newInfo;

	if(service){
		uint8_t direct = info.direct;
		charDirect->setValue(&direct, 1);
		charIp->setValue(info.ip, sizeof(info.ip));

		// poor man's encryption
		char key[] = "abc";
		std::string data = info.ssid + ";:" + info.pass;
		for(int i = 0, j = 0; i < data.size(); i++, j = (j+1) % sizeof(key)){
			data[i] = data[i] ^ key[j];
		}

		charWifi->setValue(data);
		charQuality->setValue(&info.quality, 1);
	}
}
