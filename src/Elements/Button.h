#ifndef WHEELSONRC_BYTEBOI_BUTTON_H
#define WHEELSONRC_BYTEBOI_BUTTON_H

#include <UI/ElementContainer.h>
#include <UI/Element.h>

class Button : public Element {
public:
	Button(ElementContainer *parent, const char* text);
	uint getWidth() override;
	uint getHeight() override;
	void draw() override;

	void setSelected(bool selected);

private:
	const char* text = "";
	bool selected = false;

	const uint borderWidth = 2;
	const uint borderRadius = 3;
};


#endif //WHEELSONRC_BYTEBOI_BUTTON_H
