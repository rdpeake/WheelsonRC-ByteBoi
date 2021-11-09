#include "Button.h"

#define printCenter(canvas, y, text) do { canvas->setCursor((canvas->width() - canvas->textWidth(text)) / 2, y); canvas->print(text); } while(0)

Button::Button(ElementContainer *parent, const char* text) : Element(parent), text(text){
	nl = this->text.indexOf('\n');
}

uint Button::getWidth(){
	return 128;
}

uint Button::getHeight(){
	return nl == -1 ? 20 : 27;
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

	if(nl != -1){
		printCenter(canvas, y + 5, text.substring(0, nl).c_str());
		printCenter(canvas, y + 15, text.substring(nl + 1).c_str());
	}else{
		printCenter(canvas, y + 6, text.c_str());
	}
}

void Button::setSelected(bool selected){
	Button::selected = selected;
}
