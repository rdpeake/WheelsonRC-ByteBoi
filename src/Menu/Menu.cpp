#include <Pins.hpp>
#include "Menu.h"
#include "Button.h"
#include "../Connecting.h"
#include "../RemoteControl.h"
#include <Input/Input.h>

Menu::Menu(Display &display) : Context(display){
	buildUI();
	Menu::pack();
}

void Menu::draw(){
	Sprite* canvas = screen.getSprite();
	extern Color* background;
	canvas->drawIcon(background, 0, 0, 160, 120);
	screen.draw();
}

void Menu::start(){
	Input::getInstance()->addListener(this);
	draw();
	screen.commit();
}

void Menu::stop(){
	Input::getInstance()->removeListener(this);
}

void Menu::buttonPressed(uint i){
	if(i == BTN_UP || i == BTN_DOWN){
		if(i == BTN_UP){
			selectElement(selectedElement == 0 ? buttons.size() - 1 : selectedElement - 1);
		}else if(i == BTN_DOWN){
			selectElement((selectedElement + 1) % buttons.size());
		}

		draw();
		screen.commit();
	}

	if(i == BTN_A){
		if(selectedElement == 0){
			Context* connecting = new Connecting(*screen.getDisplay());
			connecting->push(this);
			connecting->setParent(new RemoteControl(*screen.getDisplay()));
			return;
		}
	}
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
	layout->setPadding(15);

	buttons.push_back(new Button(layout, "Direct connection"));
	buttons.push_back(new Button(layout, "WiFi connection"));
	buttons[0]->setSelected(true);

	for(const auto& button : buttons){
		layout->addChild(button);
	}

	layout->reflow();
	layout->repos();

	screen.addChild(layout);
}
