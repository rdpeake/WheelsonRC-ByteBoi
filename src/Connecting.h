#ifndef WHEELSONRC_BYTEBOI_CONNECTING_H
#define WHEELSONRC_BYTEBOI_CONNECTING_H

#include <Arduino.h>
#include <Support/Context.h>
#include "Connection.h"

class Connecting : public Context, public InputListener, public ConnectionListener {
public:
	Connecting(Display &display);
	virtual ~Connecting();
	void draw() override;
	void start() override;
	void stop() override;

	void connected() override;
	void disconnected() override;

protected:
	void init() override;
	void deinit() override;

private:
	Color* icon = nullptr;

	void buttonPressed(uint i) override;
};


#endif //WHEELSONRC_BYTEBOI_CONNECTING_H
