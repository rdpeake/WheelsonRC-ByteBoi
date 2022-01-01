#include <Loop/LoopManager.h>
#include <Pins.hpp>
#include "RemoteControl.h"
#include "../Connection.h"
#include "Connecting.h"
#include <Input/Input.h>
#include <TJpg_Decoder.h>
#include <ByteBoi.h>
#include <FS/RamFile.h>

#define printCenter(canvas, y, text) do { canvas->setCursor((canvas->width() - canvas->textWidth(text)) / 2, y); canvas->print(text); } while(0)

RemoteControl::RemoteControl(Display& display) : Context(display), feedTask("RC-Feed", RemoteControl::feedTaskFunc, 2048, this){
	RemoteControl::pack();
}

RemoteControl::~RemoteControl(){
	RemoteControl::deinit();
}

#define FRAME_LEN 8

const uint8_t frameStart[FRAME_LEN] = { 0x18, 0x20, 0x55, 0xf2, 0x5a, 0xc0, 0x4d, 0xaa };
const uint8_t frameEnd[FRAME_LEN] = { 0x42, 0x2c, 0xd9, 0xe3, 0xff, 0xa0, 0x11, 0x01 };

void RemoteControl::start(){
	if(!Con.connected()){
		pop();
		return;
	}

	Input::getInstance()->addListener(this);
	LoopManager::addListener(this);

	Con.setListener(this);

	frameReceiveTime = 0;
}

void RemoteControl::stop(){
	Input::getInstance()->removeListener(this);
	LoopManager::removeListener(this);

	Con.setListener(nullptr);
	Con.stop();
}

void RemoteControl::draw(){
	Sprite* canvas = screen.getSprite();
	canvas->clear(TFT_BLACK);
	canvas->setTextFont(0);
	canvas->setTextSize(1);
	canvas->setTextColor(TFT_WHITE);
	printCenter(canvas, 55, "Waiting for feed...");
	Battery.drawIcon(*canvas,143,4);
}

void RemoteControl::loop(uint micros){
	if(!Con.connected()){
		pop();
		return;
	}

	WiFiClient& controlClient = Con.getControlClient();
	WiFiClient& feedClient = Con.getFeedClient();
	if(!feedClient.connected() || !controlClient.connected()){
		printf("dc4\n");
		Con.disconnected();
		LoopManager::removeListener(this);
		return;
	}

	if(!feedFunc()){
		printf("dc5\n");
		Con.disconnected();
		LoopManager::removeListener(this);
		return;
	}

	if(frameReceiveTime != 0 && millis() - frameReceiveTime >= 3000){
		printf("timeout\n");
		Con.disconnected();
		LoopManager::removeListener(this);
		return;
	}

	Battery.drawIcon(*screen.getSprite(),143,4);

	Sprite* canvas = screen.getSprite();
	canvas->drawIcon(bbIcon, 126, 4, 15, 6, 1, TFT_TRANSPARENT);
	canvas->drawIcon(whIcon, 126, 13, 15, 6, 1, TFT_TRANSPARENT);

	screen.commit();
}

void RemoteControl::buttonPressed(uint i){
	if(i == BTN_B){
		pop();
		return;
	}

	switch(i){
		case BTN_UP:
			command |= 0b0001;
			break;
		case BTN_DOWN:
			command |= 0b0010;
			break;
		case BTN_LEFT:
			command |= 0b0100;
			break;
		case BTN_RIGHT:
			command |= 0b1000;
			break;
		case BTN_C:
			command ^= 0b100000; //toggle bit 6
			break;
		case BTN_A:
			command |= 0b10000;
			sendCommand();
			command &= ~0b10000;
			return;
	}

	sendCommand();
}

void RemoteControl::buttonReleased(uint i){
	if(i == BTN_A || i == BTN_B || i == BTN_C) return;

	switch(i){
		case BTN_UP:
			command &= ~0b0001;
			break;
		case BTN_DOWN:
			command &= ~0b0010;
			break;
		case BTN_LEFT:
			command &= ~0b0100;
			break;
		case BTN_RIGHT:
			command &= ~0b1000;
			break;
	}

	sendCommand();
}

void RemoteControl::sendCommand(){
	printf("Sending %x\n", command);

	WiFiClient& client = Con.getControlClient();
	if(!client.connected()){
		printf("dc2\n");
		Con.disconnected();
		return;
	}

	if(client.write(&command, 1) != 1){
		printf("dc3\n");
		Con.disconnected();
		return;
	}
}

Sprite* canvas = nullptr;
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){
	if(x+w > 160 || y+h > 120) return true;

	for(int i = 0; i < w*h; i++){
		bitmap[i] = ((bitmap[i] << 8) & 0xff00) | ((bitmap[i] >> 8) & 0x00ff);
	}

	if(canvas == nullptr) return true;
	canvas->pushImage(x, y, w, h, bitmap);

	return true;
}

bool RemoteControl::feedFunc(){
	RemoteControl* rc = this;
	WiFiClient& client = Con.getFeedClient();

	if(!client.connected()) return false;

	uint8_t *feedBuff = nullptr;
#define error() printf("out\n"); free(feedBuff); return false

	uint8_t packetFrame[FRAME_LEN];
	if(!rc->read(packetFrame, FRAME_LEN)){
		error();
	}

	if(memcmp(packetFrame, frameStart, FRAME_LEN) != 0){
		printf("Frame header mismatch. Searching for frame... ");

		size_t match = 0, bytes = 0;
		while(match != FRAME_LEN){
			if(!rc->read(packetFrame + match, 1)){
				error();
			}

			bytes++;

			if(packetFrame[match] == frameStart[match]){
				match++;
			}else{
				match = 0;
			}
		}

		printf("Found after %zu bytes\n", bytes);
	}

	uint32_t frameSize;
	if(!rc->read(reinterpret_cast<uint8_t*>(&frameSize), sizeof(frameSize))){
		error();
	}

	if(frameSize > 12000){
		printf("yuge framesize\n");
		return true;
	}

	feedBuff = static_cast<uint8_t *>(malloc(frameSize));
	if(!feedBuff){
		printf("feedBuff malloc err. frame size: %d B\n", frameSize);
		return true;
	}

	if(!rc->read(feedBuff, frameSize)){
		error();
	}

	if(!rc->read(packetFrame, FRAME_LEN)){
		error();
	}

	if(memcmp(packetFrame, frameEnd, FRAME_LEN) != 0){
		printf("Missed frame footer.\n");
	}

	uint8_t level;
	if(!rc->read(&level, 1)){
		error();
	}

	canvas = rc->screen.getSprite();
	TJpgDec.setJpgScale(1);
	TJpgDec.setCallback(tft_output);
	TJpgDec.drawJpg(0, 0, feedBuff, frameSize);
	canvas->fillRect(0, 0, 160, 2, TFT_BLACK);
	canvas->fillRect(118, 0, 160, 2, TFT_BLACK);

	free(feedBuff);

	frameReceiveTime = millis();

	Battery.drawIcon(*screen.getSprite(),143,13, level);

	return true;
}

void RemoteControl::feedTaskFunc(Task* task){
	RemoteControl* rc = static_cast<RemoteControl*>(task->arg);
	WiFiClient& client = Con.getFeedClient();

	while(task->running){
		if(!rc->feedFunc()) return;
	}
}

bool RemoteControl::read(uint8_t* buffer, size_t size){
	WiFiClient& feedClient = Con.getFeedClient();

	if(!feedClient.connected()) return false;

	size_t read = 0;
	while(read < size){
		/*if(WiFi.softAPgetStationNum() == 0){
			printf("dc6\n");
			return false;
		}*/
		if(!feedClient.connected()) return false;
		size_t available = feedClient.available();
		if(available == 0){
			delayMicroseconds(100);
			yield();
			continue;
		}

		size_t pread = feedClient.read(buffer + read, min(256L, min(available, size - read)));
		if(pread == -1){
			if(!feedClient.connected()) return false;

			printf("-1\n");

			delayMicroseconds(500);
		}else{
			read += pread;
		}
		yield();
	}
	return true;
}

void RemoteControl::init(){
	auto read = [](const char* path, bool bg = false) -> Color* {
		fs::File file = ByteBoi.openResource(path, "r");

		if(!file){
			printf("Error opening SPIFFS file: %s\n", path);
			return nullptr;
		}

		Color* buffer = static_cast<Color *>(ps_malloc(file.size()));
		file.read(reinterpret_cast<uint8_t *>(buffer), file.size());
		file.close();

		return buffer;
	};

	bbIcon = read("/mini_bb.raw");
	whIcon = read("/mini_wh.raw");
}

void RemoteControl::deinit(){
	free(bbIcon);
	free(whIcon);
	bbIcon = whIcon = nullptr;
}

void RemoteControl::connected(){

}

void RemoteControl::disconnected(){
	pop();
}
