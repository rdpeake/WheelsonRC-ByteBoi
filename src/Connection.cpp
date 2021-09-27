#include <WiFi.h>
#include <Loop/LoopManager.h>
#include "Connection.h"

Connection Con;

Connection::Connection() :
		localIP(10, 0, 0, 4),
		gateway(10, 0, 0, 1),
		subnet(10, 0, 0, 0){

}

Connection::~Connection(){
	stop();
}

void Connection::start(){
	WiFi.softAPConfig(localIP, gateway, subnet);
	WiFi.softAP(ssid, password);

	feedServer.begin(5000);
	controlServer.begin(5001);

	LoopManager::addListener(this);
}

void Connection::stop(){
	feedClient.stop();
	controlClient.stop();
	feedServer.stop();
	controlServer.stop();
	WiFi.disconnect(true);
	LoopManager::removeListener(this);
}

void Connection::loop(uint micros){
	if(connected()){
		LoopManager::removeListener(this);
		return;
	}

	if(!feedClient.connected()){
		feedClient = feedServer.available();
		if(!feedClient.connected()) return;
	}

	if(!controlClient.connected()){
		controlClient = controlServer.available();
		if(!controlClient.connected()) return;
	}

	if(connected()){
		LoopManager::removeListener(this);

		if(listener){
			listener->connected();
		}
	}
}

void Connection::disconnected(){
	stop();

	if(listener){
		listener->disconnected();
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
