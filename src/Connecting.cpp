#include <ByteBoi.h>
#include "Connecting.h"

#define printCenter(canvas, y, text) do { canvas->setCursor((canvas->width() - canvas->textWidth(text)) / 2, y); canvas->print(text); } while(0)

Connecting::Connecting(Display &display) : Context(display){
	Connecting::pack();
}

Connecting::~Connecting(){
	free(icon);
}

void Connecting::draw(){
	Sprite* canvas = screen.getSprite();
	extern Color* background;
	canvas->drawIcon(background, 0, 0, 160, 120);
	canvas->drawIcon(icon, (160 - 70) / 2, 15, 70, 70, 1, TFT_TRANSPARENT);

	canvas->setTextFont(0);
	canvas->setTextSize(1);
	canvas->setTextColor(TFT_WHITE);
	printCenter(canvas, 95, "Connecting to Wheelson");
}

void Connecting::start(){
	Con.setListener(this);
	Con.start();
	draw();
	screen.commit();
}

void Connecting::stop(){
	Con.setListener(nullptr);
}

void Connecting::init(){
	fs::File file = ByteBoi.openResource("/wheel.raw", "r");
	if(!file){
		printf("Error opening wheel file\n");
		return;
	}

	icon = static_cast<Color *>(ps_malloc(70 * 70 * 2));
	file.read(reinterpret_cast<uint8_t *>(icon), 70 * 70 * 2);
	file.close();
}

void Connecting::deinit(){
	free(icon);
	icon = nullptr;
}

void Connecting::connected(){
	printf("connected\n");
	pop();
}

void Connecting::disconnected(){

}

void Connecting::buttonPressed(uint i){
	if(i == BTN_B){
		Con.stop();
		pop();
		return;
	}
}
