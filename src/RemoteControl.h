#ifndef WHEELSONRC_BYTEBOI_REMOTECONTROL_H
#define WHEELSONRC_BYTEBOI_REMOTECONTROL_H


#include <Support/Context.h>
#include <Input/InputListener.h>
#include "Connection.h"
#include <Util/Task.h>

class RemoteControl : public Context, public ConnectionListener, public InputListener, public LoopListener {
public:
	RemoteControl(Display& display);
	virtual ~RemoteControl();

	void draw() override;
	void start() override;
	void stop() override;

	void connected() override;
	void disconnected() override;

	static void feedFunc(Task* task);
	void loop(uint micros) override;

protected:
	void init() override;
	void deinit() override;

private:
	Color* image = nullptr;
	Task feedTask;

	uint8_t command = 0;

	void buttonPressed(uint i) override;
	void buttonReleased(uint i) override;

	void sendCommand();

	bool read(uint8_t* buffer, size_t size);

};


#endif //WHEELSONRC_BYTEBOI_REMOTECONTROL_H
