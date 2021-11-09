#ifndef WHEELSONRC_BYTEBOI_CONNECTING_H
#define WHEELSONRC_BYTEBOI_CONNECTING_H

#include <Arduino.h>
#include <Loop/LoopListener.h>
#include <Support/Context.h>
#include "../Connection.h"

class Connecting : public Context, public InputListener, public LoopListener {
public:
	Connecting(Display &display);
	virtual ~Connecting();
	void draw() override;
	void start() override;
	void stop() override;
	void loop(uint micros) override;

protected:
	void init() override;
	void deinit() override;

private:
	Color* icon = nullptr;
	Color* crossedIcon = nullptr;
	bool blink = false;
	uint32_t blinkTime = 0;

	static const char* texts[4];

	void buttonPressed(uint i) override;
};


#endif //WHEELSONRC_BYTEBOI_CONNECTING_H
