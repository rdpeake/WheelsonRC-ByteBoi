#include <Loop/LoopManager.h>
#include <Pins.hpp>
#include "RemoteControl.h"
#include "../Connection.h"
#include "Connecting.h"
#include <Input/Input.h>
#include <TJpg_Decoder.h>

RemoteControl::RemoteControl(Display& display) : Context(display), feedTask("RC-Feed", RemoteControl::feedFunc, 2048, this){
	RemoteControl::pack();
}

RemoteControl::~RemoteControl(){
	free(image);
}

void RemoteControl::start(){
	if(!Con.connected()){
		pop();
		return;
	}

	Con.setListener(this);
	feedTask.start(1, 0);
	LoopManager::addListener(this);
	Input::getInstance()->addListener(this);
}

void RemoteControl::stop(){
	feedTask.stop(true);
	LoopManager::removeListener(this);
	Con.stop();
	Con.setListener(nullptr);
	Input::getInstance()->removeListener(this);
}

void RemoteControl::draw(){
	Sprite* canvas = screen.getSprite();
	canvas->clear(TFT_BLACK);
	canvas->drawIcon(image, 0, 4, 160, 120);
}

void RemoteControl::loop(uint micros){
	WiFiClient& client = Con.getControlClient();
	if(!client.connected()){
		Con.disconnected();
		LoopManager::removeListener(this);
		return;
	}
}

void RemoteControl::buttonPressed(uint i){
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
	}

	sendCommand();
}

void RemoteControl::buttonReleased(uint i){
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
	WiFiClient& client = Con.getControlClient();
	if(!client.connected()){
		Con.disconnected();
		return;
	}

	if(client.write(&command, 1) != 1){
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

#define FRAME_LEN 8

const uint8_t frameStart[FRAME_LEN] = { 0x18, 0x20, 0x55, 0xf2, 0x5a, 0xc0, 0x4d, 0xaa };
const uint8_t frameEnd[FRAME_LEN] = { 0x42, 0x2c, 0xd9, 0xe3, 0xff, 0xa0, 0x11, 0x01 };

void RemoteControl::feedFunc(Task* task){
	RemoteControl* rc = static_cast<RemoteControl*>(task->arg);
	WiFiClient& client = Con.getFeedClient();

	while(task->running){
		if(!client.connected()){
			Con.disconnected();
			return;
		}

		uint8_t packetFrame[FRAME_LEN];
		if(!rc->read(packetFrame, FRAME_LEN)){
			// handle error
		}

		if(memcmp(packetFrame, frameStart, FRAME_LEN) != 0){
			printf("Frame header mismatch. Searching for frame... ");

			size_t match = 0, bytes = 0;
			while(match != FRAME_LEN){
				bytes++;
				rc->read(packetFrame + match, 1);
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
			// handle error
		}

		auto *feedBuff = static_cast<uint8_t *>(malloc(frameSize));
		if(!feedBuff){
			printf("feedBuff malloc err. frame size: %d B\n", frameSize);
			for(;;) delay(1);
		}

		if(!rc->read(feedBuff, frameSize)){
			// handle error
		}

		if(!rc->read(packetFrame, FRAME_LEN)){
			// handle error
		}

		if(memcmp(packetFrame, frameEnd, FRAME_LEN) != 0){
			printf("Missed frame footer.\n");
		}

		canvas = rc->screen.getSprite();
		TJpgDec.setJpgScale(1);
		TJpgDec.setCallback(tft_output);
		TJpgDec.drawJpg(0, 0, feedBuff, frameSize);
		canvas->fillRect(0, 0, 160, 2, TFT_BLACK);
		canvas->fillRect(118, 0, 160, 2, TFT_BLACK);
		rc->screen.commit();

		free(feedBuff);
	}
}

bool RemoteControl::read(uint8_t* buffer, size_t size){
	WiFiClient& feedClient = Con.getFeedClient();

	if(!feedClient.connected()) return false;

	size_t read = 0;
	while(read < size){
		size_t pread = feedClient.read(buffer + read, size - read);
		if(pread == -1){
			if(!feedClient.connected()) return false;

			delayMicroseconds(500);
		}else{
			read += pread;
		}
		yield();
	}
	return true;
}

void RemoteControl::init(){
	image = static_cast<Color*>(ps_malloc(160 * 120 * 2));
}

void RemoteControl::deinit(){
	free(image);
	image = nullptr;
}

void RemoteControl::connected(){

}

void RemoteControl::disconnected(){
	Context* connecting = new Connecting(*screen.getDisplay());
	connecting->push(this);
}

