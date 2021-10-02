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

	disableCore0WDT();
	disableCore1WDT();

	TJpgDec.initBuffer();

	fs::File bg = CompressedFile::open(ByteBoi.openResource("/background.raw.hs", "r"), 12, 10);
	if(!bg){
		printf("bg not open\n");
		for(;;);
	}
	background = static_cast<Color *>(ps_malloc(160 * 120 * 2));
	bg.read(reinterpret_cast<uint8_t *>(background), 160 * 120 * 2);

	LoopManager::addListener(ByteBoi.getInput());

	Context::setDeleteOnPop(true);

	Context* menu = new Menu(*ByteBoi.getDisplay());
	menu->unpack();
	menu->start();
}

void loop(){
	LoopManager::loop();
}