#include <WiFi.h>
#include <Loop/LoopManager.h>
#include "Connection.h"
#include "Advertiser.h"

Connection Con;

const char* Connection::directSSID = "WheelsonRC";
const char* Connection::directPass = "WheelsonRCServer";

Connection::Connection() :
		directIp(10, 0, 0, 4),
		gateway(10, 0, 0, 1),
		subnet(10, 0, 0, 0){

}

Connection::~Connection(){
	stop();
}

void Connection::setInfo(uint8_t quality, const String& ssid, const String& pass){
	this->ssid = ssid;
	this->pass = pass;
	this->quality = quality;
}

void Connection::start(){
	stop(false);

	if(isDirect()){
		WiFi.softAPConfig(directIp, gateway, subnet);
		WiFi.softAP(directSSID, directPass);

		WiFi.onEvent(Connection::onWiFi);

		setupServer();

		Advertise.setInfo(RCInfo(quality));
		Advertise.start();

		state = WAITING;
	}else{
		WiFi.begin(ssid.c_str(), pass.c_str());
		wifiTime = millis();

		state = WIFI;
	}

	LoopManager::addListener(this);
}

void Connection::setupServer(){
	feedServer.begin(5000);
	controlServer.begin(5001);

	serverRunning = true;
}

void Connection::stop(bool resetInfo){
	Advertise.stop();
	cleanupServer();
	WiFi.disconnect(true);
	WiFi.mode(WIFI_OFF);
	LoopManager::removeListener(this);

	state = IDLE;
	assignedIp = IPAddress();
	if(resetInfo){
		this->ssid = this->pass = "";
		quality = 0;
	}
}

void Connection::cleanupServer(){
	feedClient.stop();
	controlClient.stop();
	feedServer.stop();
	controlServer.stop();

	serverRunning = false;
}

void Connection::loop(uint micros){
	if(!isDirect()){
		if(WiFi.status() != WL_CONNECTED){
			if(millis() - wifiTime >= 5000){
				WiFi.begin(ssid.c_str(), pass.c_str());
				wifiTime = millis();
			}
			return;
		}else if(!serverRunning){
			wifiTime = 0;
			setupServer();

			assignedIp = WiFi.localIP();
			int ip = assignedIp.operator unsigned int();
			printf("IP: %s, %d\n", assignedIp.toString().c_str(), ip);
			Advertise.setInfo(RCInfo(ssid.c_str(), pass.c_str(), reinterpret_cast<uint8_t*>(&ip), quality));
			Advertise.start();

			state = WAITING;

			return;
		}
	}

	if(connected()){
		Advertise.stop();
		LoopManager::removeListener(this);

		feedServer.stop();
		controlServer.stop();

		feedClient.setTimeout(5);
		controlClient.setTimeout(5);

		state = CONNECTED;

		if(listener){
			listener->connected();
		}
	}

	if(!feedClient.connected()){
		feedClient = feedServer.available();
		if(!feedClient.connected()) return;
	}

	if(!controlClient.connected()){
		controlClient = controlServer.available();
		if(!controlClient.connected()) return;
	}
}

void Connection::disconnected(){
	stop();

	if(listener){
		listener->disconnected();
	}
}

void Connection::onWiFi(WiFiEvent_t e){
	if(e == SYSTEM_EVENT_AP_STADISCONNECTED){
		Con.disconnected();
	}
}

void Connection::setListener(ConnectionListener *listener){
	Connection::listener = listener;
}

bool Connection::connected(){
	return feedClient.connected() && controlClient.connected();
}

WiFiClient& Connection::getFeedClient(){
	return feedClient;
}

WiFiClient& Connection::getControlClient(){
	return controlClient;
}

bool Connection::isDirect(){
	return ssid.length() == 0 && pass.length() == 0;
}

Connection::State Connection::getState() const{
	return state;
}
