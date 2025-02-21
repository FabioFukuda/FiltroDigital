#include "menu.h"

tContext grContext;
int16_t width;
int16_t height;

void displaySetup();
void display_main_menu();
void clearDisplay();

enum menu_state{
	MAIN_MENU, FILTER_TYPE, CUTOFF_FREQ, FILTER_ORDER
};

void menu_init(){
	displaySetup();
	display_main_menu();
	
}

void display_main_menu(){
	GrContextForegroundSet(&grContext, ClrWhite);
	GrContextBackgroundSet(&grContext, ClrBlack);
	
	char str[20];
	strcpy(str,"Thred");
	
	tRectangle sRect;

	sRect.i16XMin = 0;
	sRect.i16YMin = 0;
	sRect.i16XMax = width;
	sRect.i16YMax = grContext.psFont->ui8Height;

	GrRectFill(&grContext, &sRect);
	int x_pos = width*0.5-strlen(str)*0.5*grContext.psFont->ui8MaxWidth;
	GrContextForegroundSet(&grContext, ClrBlack);
	GrStringDraw(&grContext,str, -1, x_pos, (grContext.psFont->ui8Height+2)*0, false);
	GrContextForegroundSet(&grContext, ClrWhite);
	GrStringDraw(&grContext,"Tiva + BoosterPack", -1, 0, (grContext.psFont->ui8Height+2)*1, true);
	sprintf(str,"%d",width);
	GrStringDraw(&grContext,str, -1, 0, (grContext.psFont->ui8Height+2)*2, true);
	GrStringDraw(&grContext,"---------------------", -1, 0, (grContext.psFont->ui8Height+2)*3, true);
	GrFlush(&grContext);
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

