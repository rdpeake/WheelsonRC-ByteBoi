#include <ByteBoi.h>
#include "Connecting.h"
#include "RemoteControl.h"
#include <Loop/LoopManager.h>

const char* Connecting::texts[4] = { "Connecting...", "Connecting to WiFi", "Connecting...", "Connected" };

#define printCenter(canvas, y, text) do { canvas->setCursor((canvas->width() - canvas->textWidth(text)) / 2, y); canvas->print(text); } while(0)

Connecting::Connecting(Display &display) : Context(display){
	Connecting::pack();
}

Connecting::~Connecting(){
	Connecting::deinit();
}

void Connecting::draw(){
	Sprite* canvas = screen.getSprite();
	extern Color* background;
	canvas->drawIcon(background, 0, 0, 160, 120);

	if(crossedIcon && icon){
		canvas->drawIcon(blink ? crossedIcon : icon, (160 - 70) / 2, 15, 70, 70, 1, TFT_TRANSPARENT);
	}

	canvas->setTextFont(0);
	canvas->setTextSize(1);
	canvas->setTextColor(TFT_WHITE);
	printCenter(canvas, 95, texts[Con.getState()]);

	if(Con.getState() == Connection::WAITING){
		printCenter(canvas, 105, "Get close to Wheelson");
	}

	Battery.drawIcon(*canvas,143,3);
}

void Connecting::loop(uint micros){
	if(Con.connected()){
		Context* rc = new RemoteControl(*screen.getDisplay());
		rc->push(parent);
		stop();
		delete this;
		return;
	}

	if(millis() - blinkTime >= 500){
		blink = !blink;
		blinkTime = millis();
	}

	draw();
	screen.commit();
}

void Connecting::start(){
	Con.start();

	Input::getInstance()->addListener(this);
	LoopManager::addListener(this);

	blinkTime = millis();

	draw();
	screen.commit();
}

void Connecting::stop(){
	Input::getInstance()->removeListener(this);
	LoopManager::removeListener(this);

	Advertise.stop();
}

void Connecting::buttonPressed(uint i){
	if(i == BTN_B){
		Con.stop();
		pop();
		return;
	}
}

void Connecting::init(){
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

	icon = read("/wheel.raw");
	crossedIcon = read("/wheel-crossed.raw");

	blink = false;
	blinkTime = 0;
}

void Connecting::deinit(){
	free(icon);
	free(crossedIcon);
	icon = nullptr;
	crossedIcon = nullptr;
}
