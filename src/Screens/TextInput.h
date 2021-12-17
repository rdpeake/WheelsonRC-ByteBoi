#ifndef WHEELSONRC_BYTEBOI_TEXTINPUT_H
#define WHEELSONRC_BYTEBOI_TEXTINPUT_H


#include <Support/Context.h>
#include <FS.h>
#include <Input/InputListener.h>

class TextInput : public Context, public InputListener {
public:
	TextInput(Display &display, const String& title = "", const String& text = "");
	virtual ~TextInput();

	void start();
	void stop();

	void draw();

private:
	void buttonPressed(uint i) override;

	String title = "";
	String text = "";

	bool capitalLetters = false;
	bool shiftLetters = false;

	int selectedIndex = 0;

	static const char symbols[];
	static const char otherSymbols[];
	const uint8_t numElements = 44;
	const uint8_t rows = 5;
	const uint8_t columns = 9;
	const uint8_t vSpace = 15;
	const uint8_t hSpace = 17;

	char getLetter(int i);
};

#endif //WHEELSONRC_BYTEBOI_TEXTINPUT_H
