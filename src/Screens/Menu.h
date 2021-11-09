#ifndef WHEELSONRC_BYTEBOI_MENU_H
#define WHEELSONRC_BYTEBOI_MENU_H

#include <Support/Context.h>
#include <UI/LinearLayout.h>
#include <Input/InputListener.h>
#include "../Elements/Button.h"

class Menu : public Context, public InputListener, public LoopListener {
public:
	Menu(Display &display);
	void draw() override;

	void start() override;
	void stop() override;
	void returned(void* data) override;

	void loop(uint micros) override;

private:
	String ssid = "";
	String pass = "";
	bool ssidEntered = false;
	bool passEntered = false;

	int8_t quality = 0;

	LinearLayout* layout = nullptr;
	std::vector<Button*> buttons;

	uint8_t selectedElement = 0;

	void buildUI();
	void selectElement(uint8_t i);
	void buttonPressed(uint i) override;
};


#endif //WHEELSONRC_BYTEBOI_MENU_H
