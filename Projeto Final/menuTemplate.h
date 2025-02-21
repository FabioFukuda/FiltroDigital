#ifdef __cplusplus
extern "C" {
	#include <stdbool.h>
	#include <stdint.h>
	#include <stdio.h>
	#include <string.h>
	#include "grlib.h"
	#include "driverlib/sysctl.h"
	#include "Crystalfontz128x128_ST7735.h"
	#include "utils/uartstdio.h"
	
	extern int filter_order;
	extern int type;
	extern int fc1;
	extern int fc2;
	
}
#endif

class MenuTemplate;
typedef struct option{
	char optionName[20];
	bool selected;
	struct option* next;
	struct option* prev;
	MenuTemplate* goToMenu;
	int value;
}Option;

typedef enum joystickPos{
	UP,DOWN,MID
}joystickPos;

typedef enum buttonState{
	PRESSED,RELEASED
}buttonState;

class MenuTemplate{
	public:
		MenuTemplate(tContext* grContext,int* width,int* height,MenuTemplate** curMenu);
		virtual void draw()=0;
		virtual void update()=0;
		void setInput(int jY,int b1, int b2){this->jY = jY; this->b1 = (b1 == 0x00 ? true:false);this->b2 = (b2 == 0x00 ? true:false);}
		void clearDisplay();
		bool getParamModified(){return this->paramModified;}
		void setParamModified(bool param){this->paramModified = param;}
		
		bool getOn(){return this->on;}
		
	protected:
		
		void drawLine(int lineIndex,char* content,bool selected);	
		MenuTemplate** curMenu;
		tContext* grContext;
		int* width;
		int* height;
	
		int jY;
		bool b1;
		bool b2;
	
		static bool paramModified;
		static bool on;
};

class MainMenu: public MenuTemplate{
	public:
		MainMenu(tContext* grContext,int* width,int* height,MenuTemplate** curMenu);
		void draw();
		void update();
		void setMenus(MenuTemplate* mainMenu,MenuTemplate* frequenciaDeCorteMenu,MenuTemplate* ordemDoFiltroMenu,MenuTemplate* ligaDesligaMenu);
	private:
		Option options[4];
		Option* curOption;
		int numOptions;
		
		joystickPos jPos;
		buttonState b1Pos;
		buttonState b2Pos;
};

class TipoDoFiltroMenu: public MenuTemplate{
	public:
		TipoDoFiltroMenu(tContext* grContext,int* width,int* height,MenuTemplate** curMenu);
		void draw();
		void update();
		void setMenus(MenuTemplate* mainMenu);
	private:
		Option options[3];
		Option* curOption;
		int numOptions;
		
		joystickPos jPos;
		buttonState b1Pos;
		buttonState b2Pos;
};

class FrequenciaDeCorteMenu: public MenuTemplate{
	public:
		FrequenciaDeCorteMenu(tContext* grContext,int* width,int* height,MenuTemplate** curMenu);
		void draw();
		void update();
		void setMenus(MenuTemplate* mainMenu);
	
	private:
		Option options[2];
		Option* curOption;
		int numOptions;
		int jPosUpCont;
		int jPosDownCont;
	
		joystickPos jPos;
		buttonState b1Pos;
		buttonState b2Pos;
};

class OrdemDoFiltroMenu: public MenuTemplate{
	public:
		OrdemDoFiltroMenu(tContext* grContext,int* width,int* height,MenuTemplate** curMenu);
		void draw();
		void update();
		void setMenus(MenuTemplate* mainMenu);
	
	private:
		Option options[1];
		Option* curOption;
		int numOptions;
		int jPosUpCont;
		int jPosDownCont;
	
		joystickPos jPos;
		buttonState b1Pos;
		buttonState b2Pos;
};

class LigaDesligaMenu: public MenuTemplate{
	public:
		LigaDesligaMenu(tContext* grContext,int* width,int* height,MenuTemplate** curMenu);
		void draw();
		void update();
		void setMenus(MenuTemplate* mainMenu);
	
	private:
		Option options[2];
		Option* curOption;
	
		int numOptions;
		int jPosUpCont;
		int jPosDownCont;
	
		joystickPos jPos;
		buttonState b1Pos;
		buttonState b2Pos;
};