#ifdef __cplusplus
extern "C" {
	#include <stdbool.h>
	#include <stdint.h>
	#include <stdio.h>
	#include <string.h>
	#include "grlib.h"
	#include "driverlib/sysctl.h"
	#include "Crystalfontz128x128_ST7735.h"
	#include "tx_api.h"
	
	
	extern TX_EVENT_FLAGS_GROUP event_flags;
	extern bool on;
	
	void menu_init();
	void updateMenu();
	void userInput(int jY,int b1,int b2);
}
#endif

#define MAX_ORDER 32