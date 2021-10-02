#include "Button.h"

#define printCenter(canvas, y, text) do { canvas->setCursor((canvas->width() - canvas->textWidth(text)) / 2, y); canvas->print(text); } while(0)

Button::Button(ElementContainer *parent, const char* text) : Element(parent), text(text){}

uint Button::getWidth(){
	return 130;
}

uint Button::getHeight(){
	return 20;
}

void Button::draw(){
	Sprite* canvas = getSprite();
	int x = getTotalX();
	int y = getTotalY();
	uint w = getWidth();
	uint h = getHeight();

	canvas->fillRoundRect(x, y, w, h, borderRadius, C_HEX(0x010145));
	canvas->fillRoundRect(x + borderWidth, y + borderWidth, w - borderWidth*2, h - borderWidth*2, borderRadius, selected ? C_HEX(0x0000aa) : C_HEX(0x000071));

	canvas->setTextFont(1);
	canvas->setTextSize(1);
	canvas->setTextColor(TFT_WHITE);
	printCenter(canvas, y + 6, text);
}

void Button::setSelected(bool selected){
	Button::selected = selected;
}
