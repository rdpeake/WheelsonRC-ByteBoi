#include <Arduino.h>
#include <CircuitOS.h>
#include <ByteBoi.h>
#include <Loop/LoopManager.h>
#include "src/Screens/Menu.h"
#include <FS/CompressedFile.h>
#include <TJpg_Decoder.h>

Color* background = nullptr;

void setup(){
	Serial.begin(115200);
	ByteBoi.begin();
	ByteBoi.setGameID("WheRC");
	BatteryPopup.enablePopups(true);
	Sleep.begin();

	TJpgDec.initBuffer();

	fs::File bg = CompressedFile::open(ByteBoi.openResource("/background.raw.hs", "r"), 12, 10);
	background = static_cast<Color *>(ps_malloc(160 * 120 * 2));
	if(bg){
		bg.read(reinterpret_cast<uint8_t *>(background), 160 * 120 * 2);
		bg.close();
	}else{
		for(int i = 0; i < 160 * 120; i++){
			background[i] = C_HEX(000071);
		}
	}

	Context::setDeleteOnPop(true);

	Context* menu = new Menu(*ByteBoi.getDisplay());
	menu->unpack();
	ByteBoi.splash();
	menu->start();
}

void loop(){
	LoopManager::loop();
}