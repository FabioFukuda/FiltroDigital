#include "menu.h"
#include "menuTemplate.h"

tContext grContext;
int width;
int height;

void displaySetup();
void display_main_menu();
void clearDisplay();

bool prevOn = true;

MenuTemplate* curMenu;
TipoDoFiltroMenu tipoDoFiltroMenu(&grContext,&width,&height,&curMenu);
MainMenu mainMenu(&grContext,&width,&height,&curMenu);
FrequenciaDeCorteMenu frequenciaDeCorteMenu(&grContext,&width,&height,&curMenu);
OrdemDoFiltroMenu ordemDoFiltroMenu(&grContext,&width,&height,&curMenu);
LigaDesligaMenu ligaDesligaMenu(&grContext,&width,&height,&curMenu);

enum menu_state{
	MAIN_MENU, FILTER_TYPE, CUTOFF_FREQ, FILTER_ORDER
};

void menu_init(){
	displaySetup();
	
	mainMenu.setMenus(&tipoDoFiltroMenu,&frequenciaDeCorteMenu,&ordemDoFiltroMenu,&ligaDesligaMenu);
	tipoDoFiltroMenu.setMenus(&mainMenu);
	frequenciaDeCorteMenu.setMenus(&mainMenu);
	ordemDoFiltroMenu.setMenus(&mainMenu);
	ligaDesligaMenu.setMenus(&mainMenu);
		
	curMenu = (MenuTemplate*)&mainMenu;
	
	curMenu->draw();
}

void displaySetup() {
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    GrContextInit(&grContext, &g_sCrystalfontz128x128);
		GrFlush(&grContext);
    GrContextFontSet(&grContext, &g_sFontFixed6x8);

    width = GrContextDpyWidthGet(&grContext);
    height = GrContextDpyHeightGet(&grContext);
}

void clearDisplay(tContext *pContext)
{
    tRectangle sRect;

    // Set the foreground color to the background color (usually black)
    GrContextForegroundSet(pContext, ClrBlack);

    // Define the rectangle that covers the entire screen
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = width - 1;
    sRect.i16YMax = height - 1;

    // Fill the rectangle with the background color
    GrRectFill(pContext, &sRect);
		GrContextForegroundSet(&grContext, ClrWhite);
}

void userInput(int jY,int b1,int b2){
	curMenu->setInput(jY,b1, b2);
}

void updateMenu(){
	curMenu->update();
	
	if(curMenu->getParamModified()){
		curMenu->setParamModified(false);
		tx_event_flags_set(&event_flags, 0x2, TX_OR);
	}
	if(prevOn != curMenu->getOn()){
		prevOn = curMenu->getOn();
		on = curMenu->getOn();
	}
	
	curMenu->clearDisplay();
	curMenu->draw();
}
