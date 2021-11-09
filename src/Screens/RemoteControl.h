#ifndef WHEELSONRC_BYTEBOI_REMOTECONTROL_H
#define WHEELSONRC_BYTEBOI_REMOTECONTROL_H


#include <Support/Context.h>
#include <Input/InputListener.h>
#include "../Connection.h"
#include <Util/Task.h>
#include "../Advertiser.h"

class RemoteControl : public Context, public ConnectionListener, public InputListener, public LoopListener {
public:
	RemoteControl(Display& display);
	virtual ~RemoteControl();

	void draw() override;
	void start() override;
	void stop() override;

	void connected() override;
	void disconnected() override;

	static void feedTaskFunc(Task* task);
	void loop(uint micros) override;

protected:
	void init() override;
	void deinit() override;

private:
	Task feedTask;

	Color* bbIcon = nullptr;
	Color* whIcon = nullptr;

	uint8_t command = 0;

	void buttonPressed(uint i) override;
	void buttonReleased(uint i) override;

	void sendCommand();
	bool feedFunc();

	bool read(uint8_t* buffer, size_t size);

	uint32_t frameReceiveTime = 0;
};


#endif //WHEELSONRC_BYTEBOI_REMOTECONTROL_H
