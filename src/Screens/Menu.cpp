#include <Pins.hpp>
#include "Menu.h"
#include "../Elements/Button.h"
#include "Connecting.h"
#include "RemoteControl.h"
#include "TextInput.h"
#include <Input/Input.h>
#include <ByteBoi.h>
#include <Loop/LoopManager.h>

#define printCenter(canvas, y, text) do { canvas->setCursor((canvas->width() - canvas->textWidth(text)) / 2, y); canvas->print(text); } while(0)

Menu::Menu(Display &display) : Context(display){
	buildUI();
	Menu::pack();
}

void Menu::draw(){
	Sprite* canvas = screen.getSprite();
	extern Color* background;
	canvas->drawIcon(background, 0, 0, 160, 120);
	screen.draw();

	Battery.drawIcon(*canvas,143,3);

	const char* qualities[] = { "Low quality image\nFast response time", "Medium", "High quality image\nSlow response time" };
	//printCenter(canvas, 80, qualities[quality]);
}

void Menu::start(){
	if(ssidEntered){
		if(!passEntered){
			stop();
			pack();

			Context* input = new TextInput(*screen.getDisplay(), "Enter WiFi password", pass);
			input->setParent(this);
			input->unpack();
			input->start();
			return;
		}else{
			Con.setInfo(quality, ssid, pass);

			ssid = pass = "";
			ssidEntered = passEntered = false;

			Context* connecting = new Connecting(*screen.getDisplay());
			connecting->push(this);
		}

		return;
	}

	Input::getInstance()->addListener(this);
	LoopManager::addListener(this);
	draw();
	screen.commit();
}

void Menu::stop(){
	Input::getInstance()->removeListener(this);
	LoopManager::removeListener(this);
}

void Menu::buttonPressed(uint i){
	if(i != BTN_A){
		if(i == BTN_UP){
			selectElement(selectedElement == 0 ? buttons.size() - 1 : selectedElement - 1);
		}else if(i == BTN_DOWN){
			selectElement((selectedElement + 1) % buttons.size());
		}/*else if(i == BTN_RIGHT){
			quality = (quality + 1) % 3;
		}else if(i == BTN_LEFT){
			quality = quality == 0 ? 2 : quality - 1;
		}*/

		draw();
		screen.commit();
	}if(i == BTN_A){
		if(selectedElement == 0){
			Advertise.setInfo();
			Con.setInfo(quality);

			Context* connecting = new Connecting(*screen.getDisplay());
			connecting->push(this);
			return;
		}else if(selectedElement == 1){
			ssid = pass = "";
			ssidEntered = passEntered = false;

			fs::File saveFile = ByteBoi.openData("/wifi", "r");
			if(saveFile){
				ssid = saveFile.readStringUntil('\n');
				pass = saveFile.readString();
				saveFile.close();
			}

			Context* input = new TextInput(*screen.getDisplay(), "Enter WiFi network", ssid);
			input->push(this);
			return;
		}
	}else if(selectedElement == 2){
		ByteBoi.backToLauncher();
	}
}

void Menu::returned(void* data){
	String* s = static_cast<String*>(data);

	if(s->charAt(0) == '\r'){
		ssid = pass = "";
		ssidEntered = passEntered = false;
		delete s;
		return;
	}

	if(!ssidEntered){
		ssidEntered = true;
		ssid = *s;
	}else{
		passEntered = true;
		pass = *s;

		ByteBoi.removeData("/wifi");
		fs::File saveFile = ByteBoi.openData("/wifi", "w");
		if(saveFile){
			String saveData = ssid + "\n" + pass;
			saveFile.write(reinterpret_cast<const uint8_t*>(saveData.c_str()), saveData.length());
			saveFile.close();
		}
	}

	delete s;
}

void Menu::selectElement(uint8_t i){
	buttons[selectedElement]->setSelected(false);
	selectedElement = i;
	buttons[selectedElement]->setSelected(true);
}

void Menu::buildUI(){
	if(layout != nullptr) return;

	layout = new LinearLayout(&getScreen(), VERTICAL);
	layout->setWHType(PARENT, PARENT);
	layout->setGutter(10);
	layout->setPadding(16);

	buttons.push_back(new Button(layout, "Direct connection"));
	buttons.push_back(new Button(layout, "External WiFi\nconnection"));
	buttons.push_back(new Button(layout, "Exit"));
	buttons[0]->setSelected(true);

	for(const auto& button : buttons){
		layout->addChild(button);
	}

	layout->reflow();
	layout->repos();

	screen.addChild(layout);
}

void Menu::loop(uint micros){
	draw();
	screen.commit();
}
