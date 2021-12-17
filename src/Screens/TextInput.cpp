#include <Input/Input.h>
#include <FS/CompressedFile.h>
#include "TextInput.h"
#include "../Fonts/adobex11font.h"
#include <SPIFFS.h>
#include <Pins.hpp>

const char TextInput::symbols[] = "!#.,-_/\\:;%&$'\"`*@=|";
const char TextInput::otherSymbols[] = "<()>[]^{}";

#define printCenter(canvas, y, text) do { canvas->setCursor((canvas->width() - canvas->textWidth(text)) / 2, y); canvas->print(text); } while(0)

TextInput::TextInput(Display &display, const String& title, const String& text) : Context(display), title(title), text(text){
	TextInput::pack();
}

TextInput::~TextInput(){
	TextInput::pack();
}

void TextInput::buttonPressed(uint i){
	if(i == BTN_B){
		pop(new String("\r"));
		return;
	}else if(i == BTN_A){
		if(selectedIndex == numElements - 4){
			text = text.substring(0, text.length() - 1);
		}else if(selectedIndex == numElements - 1){
			Context* parent = this->parent;
			String text = this->text;
			stop();
			delete this;

			parent->returned(new String(text));
			parent->start();
			return;
		}else if(selectedIndex == numElements - 3){
			shiftLetters = !shiftLetters;
		}else if(selectedIndex == numElements - 2){
			capitalLetters = !capitalLetters;
		}else if(text.length() > 24){
			return;
		}else if(selectedIndex == numElements - 5){
			text += ' ';
		}else{
			text += getLetter(selectedIndex);
			if(selectedIndex > 25 && selectedIndex < 36 && shiftLetters){
				capitalLetters = false;
			}
			shiftLetters = false;
		}
	}else if(i == BTN_RIGHT){
		selectedIndex = (selectedIndex + 1) % numElements;
	}else if(i == BTN_LEFT){
		selectedIndex = selectedIndex == 0 ? numElements - 1 : selectedIndex - 1;
	}else if(i == BTN_DOWN){
		if(selectedIndex > 35){
			selectedIndex -= 36;
		}else if(selectedIndex > numElements - columns - 1){
			selectedIndex = selectedIndex - 27;
		}else{
			selectedIndex += columns;
		}
	}else if(i == BTN_UP){
		if(selectedIndex < 5){
			selectedIndex += 36;
		}else if(selectedIndex < columns){
			selectedIndex += 27;
		}else{
			selectedIndex -= columns;
		}
	}

	draw();
	screen.commit();
}


void TextInput::start(){
	Input::getInstance()->addListener(this);

	draw();
	screen.commit();
}

void TextInput::stop(){
	Input::getInstance()->removeListener(this);
}

char TextInput::getLetter(int i){
	char letter;

	if(i > 25){
		i -= 26;

		if(i > 9){
			i -= 10;

			if(capitalLetters && shiftLetters){
				letter = otherSymbols[i + 6];
			}else if(shiftLetters){
				letter = otherSymbols[i + 3];
			}else{
				letter = otherSymbols[i];
			}
		}else if(shiftLetters){
			if(capitalLetters){
				letter = symbols[i + 10];
			}else{
				letter = symbols[i];
			}
		}else{
			letter = i + '0';
		}
	}else{
		letter = (capitalLetters || shiftLetters) ? 'A' + i : 'a' + i;
	}

	return letter;
}

void TextInput::draw(){
	extern Color* background;

	Sprite* canvas = screen.getSprite();

	canvas->drawIcon(background, 0, 0, 160, 128, 1);
	canvas->fillRect(2, 39, 156, 1, TFT_WHITE);
	canvas->setTextFont(1);
	canvas->setTextSize(1);
	canvas->setTextColor(TFT_WHITE);
	canvas->setCursor(5, 29);
	canvas->printf("%s", text.c_str());

	const lgfx::U8g2font adobex11font(u8g2_font_helvB08_tr);
	canvas->setFont(&adobex11font);
	printCenter(canvas, 2, title.c_str());

	canvas->setTextFont(1);
	canvas->setTextColor(C_HEX(0x6565ff));
	printCenter(canvas, 15, "Only 2.4GHz is supported.");

	for(int i = 0; i < rows; i++){
		for(int j = 0; j < columns; j++){
			int y = 46 + i * vSpace;
			int x = 9 + j * hSpace;
			int k = i * columns + j;

			if(k == selectedIndex){
				canvas->drawRect(x - 4, y - 3, 15, 15, TFT_WHITE);
			}

			if(k == numElements - 5){
				canvas->drawRect(x - 1, y + 4, 1, 5, TFT_WHITE);
				canvas->drawRect(x - 1, y + 8, 8, 1, TFT_WHITE);
				canvas->drawRect(x + 7, y + 4, 1, 5, TFT_WHITE);
			}else if(k == numElements - 4){
				canvas->fillTriangle(x + 4, y, x, y + 4, x + 4, y + 8, TFT_WHITE);
			}else if(k == numElements - 3){
				canvas->drawTriangle(x - 1, y + 5, x + 3, y + 1, x + 7, y + 5, shiftLetters ? C_HEX(0x6565ff) : TFT_WHITE);
			}else if(k == numElements - 2){
				canvas->fillTriangle(x - 1, y + 5, x + 3, y + 1, x + 7, y + 5, capitalLetters ? C_HEX(0x6565ff) : TFT_WHITE);
			}else if(k == numElements - 1){
				canvas->fillTriangle(x - 1, y + 4, x + 1, y + 6, x - 1, y + 4, TFT_WHITE);
				canvas->fillTriangle(x + 6, y + 3, x + 2, y + 7, x + 7, y + 2, TFT_WHITE);
			}else if(k > numElements - 1){
				return;
			}else{
				char letter = getLetter(k);

				canvas->setTextFont(1);
				canvas->setTextSize(1);
				canvas->setTextColor(TFT_WHITE);
				canvas->setCursor(x + 1, y + 1);
				canvas->printf("%c", letter);
			}
		}
	}
}



