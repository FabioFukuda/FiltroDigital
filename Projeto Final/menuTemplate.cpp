#include "menuTemplate.h"
#include <cstdlib>

#ifndef MAX_ORDER
#define MAX_ORDER 32
#endif

bool MenuTemplate::paramModified = false;
bool MenuTemplate::on = true;

/* MenuTemplate Class*/
MenuTemplate::MenuTemplate(tContext* grContext,int* width,int* height,MenuTemplate** curMenu){
	this->grContext = grContext;
	this->width = width;
	this->height = height;
	this->curMenu = curMenu;
	this->paramModified = false;
}

void MenuTemplate::clearDisplay(){
	tRectangle sRect;

	GrContextForegroundSet(this->grContext, ClrBlack);

	// Define the rectangle that covers the entire screen
	sRect.i16XMin = 0;
	sRect.i16YMin = 0;
	sRect.i16XMax = *this->width - 1;
	sRect.i16YMax = *this->height - 1;

	// Fill the rectangle with the background color
	GrRectFill(this->grContext, &sRect);
	GrContextForegroundSet(this->grContext, ClrWhite);
}
void MenuTemplate::drawLine(int line,char* content,bool selected){
	int y_pos = (this->grContext->psFont->ui8Height+2)*line;
	int x_pos = *this->width*0.5-strlen(content)*0.5*this->grContext->psFont->ui8MaxWidth;
	if(selected){
		tRectangle sRect;
		sRect.i16XMin = 0;
		sRect.i16YMin = (this->grContext->psFont->ui8Height+2)*line;
		sRect.i16XMax = *this->width;
		sRect.i16YMax = sRect.i16YMin + this->grContext->psFont->ui8Height;
		GrRectFill(this->grContext, &sRect);
		GrContextForegroundSet(this->grContext, ClrBlack);
		GrStringDraw(this->grContext,content, -1, x_pos, y_pos, false);
		GrContextForegroundSet(this->grContext, ClrWhite);
	}
	else{
	GrStringDraw(this->grContext,content, -1, x_pos, y_pos, true);}
}

/* MainMenu Class*/


MainMenu::MainMenu(tContext* grContext,int* width,int* height,MenuTemplate** curMenu):MenuTemplate(grContext,width,height,curMenu){
	strcpy(options[0].optionName,"Tipo do Filtro");
	options[0].selected = true;
	
	strcpy(options[1].optionName,"Frequencia de Corte");
	options[1].selected = false;

	strcpy(options[2].optionName,"Ordem do Filtro");
	options[2].selected = false;
	
	strcpy(options[3].optionName,"Liga/Desliga");
	options[3].selected = false;
	
	options[0].next = &options[1];
	options[1].prev = &options[0];
	
	options[1].next = &options[2];
	options[2].prev = &options[1];
	
	options[2].next = &options[3];
	options[3].prev = &options[2];

	
	options[3].next = &options[0];
	options[0].prev = &options[3];

	numOptions = 4;
	curOption = &options[0];
	jPos = MID;
	b1Pos = RELEASED;
	b2Pos = RELEASED;
}

void MainMenu::update(){
	if(this->jY<595){
		if(this->jPos != DOWN){
			curOption->selected = false;
			curOption = curOption->next;
			curOption->selected = true;
			jPos = DOWN;
		}
	jPos = DOWN;
	}else if(this->jY>3500){
		if(jPos != UP){
			curOption->selected = false;
			curOption = curOption->prev;
			curOption->selected = true;
			jPos = UP;
		}
		jPos = UP;
	}else{
		jPos = MID;
	}
	if(this->b1){
		this->b1Pos = PRESSED;
	}else if(this->b1Pos == PRESSED){
		*(this->curMenu) = curOption->goToMenu;
		this->b1Pos = RELEASED;
	}else this->b1Pos = RELEASED;
}

void MainMenu::draw(){
	GrContextForegroundSet(this->grContext, ClrWhite);
	GrContextBackgroundSet(this->grContext, ClrBlack);
	this->drawLine(0,"---------------------",false);
	this->drawLine(1,"|   Menu Principal   |",false);
	this->drawLine(2,"---------------------",false);
	this->drawLine(3,"",false);
	
	for(int i = 0; i < numOptions;i++)
		this->drawLine(i+4,options[i].optionName,options[i].selected);
	
	GrFlush(this->grContext);
}
void MainMenu::setMenus(MenuTemplate* tipoDoFiltroMenu,MenuTemplate* frequenciaDeCorteMenu,MenuTemplate* ordemDoFiltroMenu,MenuTemplate* ligaDesligaMenu){
	options[0].goToMenu = tipoDoFiltroMenu;
	options[1].goToMenu = frequenciaDeCorteMenu;
	options[2].goToMenu = ordemDoFiltroMenu;
	options[3].goToMenu = ligaDesligaMenu;
}

/* TipoDoFiltroMenu Class*/

TipoDoFiltroMenu::TipoDoFiltroMenu(tContext* grContext,int* width,int* height,MenuTemplate** curMenu):MenuTemplate(grContext,width,height,curMenu){
	
	strcpy(options[0].optionName,"Passa-Baixas");
	options[0].selected = true;
	options[0].value = 1;
	
	strcpy(options[1].optionName,"Passa-Altas");
	options[1].selected = false;
	options[1].value = 2;
	
	strcpy(options[2].optionName,"Passa-Faixa");
	options[2].selected = false;
	options[2].value = 3;	
	
	options[0].next = &options[1];
	options[1].prev = &options[0];
	
	options[1].next = &options[2];
	options[2].prev = &options[1];
	
	options[2].next = &options[0];
	options[0].prev = &options[2];

	numOptions = 3;
	curOption = &options[0];
	
	jPos = MID;
	b1Pos = RELEASED;
	b2Pos = RELEASED;
}

void TipoDoFiltroMenu::setMenus(MenuTemplate* mainMenu){
	options[0].goToMenu = mainMenu;
	options[1].goToMenu = mainMenu;
	options[2].goToMenu = mainMenu;
}

void TipoDoFiltroMenu::draw(){
	
	GrContextForegroundSet(this->grContext, ClrWhite);
	GrContextBackgroundSet(this->grContext, ClrBlack);
	this->drawLine(0,"---------------------",false);
	this->drawLine(1,"|   Tipo do Filtro   |",false);
	this->drawLine(2,"---------------------",false);
	this->drawLine(3,"",false);
	
	for(int i = 0; i < numOptions;i++)
		this->drawLine(i+4,options[i].optionName,options[i].selected);
	
	GrFlush(this->grContext);
}
void TipoDoFiltroMenu::update(){
	if(this->jY<595){
		if(this->jPos != DOWN){
			curOption->selected = false;
			curOption = curOption->next;
			curOption->selected = true;
			jPos = DOWN;
		}
	jPos = DOWN;
	}else if(this->jY>3500){
		if(jPos != UP){
			curOption->selected = false;
			curOption = curOption->prev;
			curOption->selected = true;
			jPos = UP;
		}
		jPos = UP;
	}else{
		jPos = MID;
	}
	if(this->b1){
		this->b1Pos = PRESSED;
	}else if(this->b1Pos == PRESSED){
		type = curOption->value;
		paramModified = true;
		*(this->curMenu) = curOption->goToMenu;
		this->b1Pos = RELEASED;
	}else this->b1Pos = RELEASED;
}

/* FrequenciaDeCorteMenu Class*/

FrequenciaDeCorteMenu::FrequenciaDeCorteMenu(tContext* grContext,int* width,int* height,MenuTemplate** curMenu):MenuTemplate(grContext,width,height,curMenu){
	strcpy(options[0].optionName,"Inferior:");
	options[0].selected = true;
	options[0].value = 1000;
	
	strcpy(options[1].optionName,"Superior:");
	options[1].selected = false;
	options[1].value = 10000;
	
	options[0].next = &options[1];
	options[1].prev = &options[0];
	
	options[1].next = &options[0];
	options[0].prev = &options[1];
	
	
	numOptions = 2;
	curOption = &options[0];
	jPos = MID;
	b1Pos = RELEASED;
	b2Pos = RELEASED;
	
	jPosUpCont = 0;
	jPosDownCont = 0;
	
}

void FrequenciaDeCorteMenu::draw(){
	GrContextForegroundSet(this->grContext, ClrWhite);
	GrContextBackgroundSet(this->grContext, ClrBlack);
	this->drawLine(0,"---------------------",false);
	this->drawLine(1,"|Frequencia de Corte |",false);
	this->drawLine(2,"---------------------",false);
	this->drawLine(3,"",false);
	
	this->drawLine(4,options[0].optionName,false);
	char str[20];
	sprintf(str, "%d Hz", options[0].value);
	this->drawLine(5,str,options[0].selected);
	
	this->drawLine(6,options[1].optionName,false);
	sprintf(str, "%d Hz", options[1].value);
	this->drawLine(7,str,options[1].selected);
	
	GrFlush(this->grContext);
}

#define J_POS_CONT_MAX 3
#define FREQ_INCREMENT 100

void FrequenciaDeCorteMenu::update(){
	if(this->jY<595){
		if(jPosDownCont == 0 && curOption->value>0){curOption->value-=FREQ_INCREMENT;jPosDownCont++;}
		else if (jPosDownCont == J_POS_CONT_MAX && curOption->value>0){curOption->value-=FREQ_INCREMENT;jPosDownCont = 1;}
		else{jPosDownCont++;}
	}else if(this->jY>3500){
		if(jPosUpCont == 0 && curOption->value<20000){curOption->value+=FREQ_INCREMENT;jPosUpCont++;}
		else if (jPosUpCont == J_POS_CONT_MAX && curOption->value<20000){curOption->value+=FREQ_INCREMENT;jPosUpCont = 1;}
		else{jPosUpCont++;}
	}else{jPosDownCont = 0;jPosUpCont = 0;}
	
	if(this->b1){
		this->b1Pos = PRESSED;
	}else if(this->b1Pos == PRESSED){
		curOption->selected = false;
		curOption = curOption->prev;
		curOption->selected = true;
		this->b1Pos = RELEASED;
	}else this->b1Pos = RELEASED;
	
	if(this->b2){
		this->b2Pos = PRESSED;
	}else if(this->b2Pos == PRESSED){
		fc1 = options[0].value;
		fc2 = options[1].value;
		paramModified = true;
		*(this->curMenu) = curOption->goToMenu;
		this->b2Pos = RELEASED;
	}else this->b2Pos = RELEASED;
	
}

void FrequenciaDeCorteMenu::setMenus(MenuTemplate* mainMenu){
	options[0].goToMenu = mainMenu;
	options[1].goToMenu = mainMenu;
}

/* OrdemDoFiltroMenu Class*/

OrdemDoFiltroMenu::OrdemDoFiltroMenu(tContext* grContext,int* width,int* height,MenuTemplate** curMenu):MenuTemplate(grContext,width,height,curMenu){
	strcpy(options[0].optionName,"Ordem Do Filtro");
	options[0].selected = true;
	options[0].value = MAX_ORDER;
	
	numOptions = 1;
	curOption = &options[0];
	jPos = MID;
	b1Pos = RELEASED;
	b2Pos = RELEASED;
	
	jPosUpCont = 0;
	jPosDownCont = 0;
	
}

void OrdemDoFiltroMenu::draw(){
	GrContextForegroundSet(this->grContext, ClrWhite);
	GrContextBackgroundSet(this->grContext, ClrBlack);
	this->drawLine(0,"---------------------",false);
	this->drawLine(1,"|   Ordem do Filtro  |",false);
	this->drawLine(2,"---------------------",false);
	this->drawLine(3,"",false);
	
	char str[20];
	sprintf(str, "%d", options[0].value);
	this->drawLine(5,str,options[0].selected);
	
	GrFlush(this->grContext);
}
void OrdemDoFiltroMenu::update(){
	if(this->jY<595){
		if(jPosDownCont == 0 && curOption->value>1){curOption->value-=1;jPosDownCont++;}
		else if (jPosDownCont == J_POS_CONT_MAX && curOption->value>1){curOption->value-=1;jPosDownCont = 1;}
		else{jPosDownCont++;}
	}else if(this->jY>3500){
		if(jPosUpCont == 0 && curOption->value<MAX_ORDER){curOption->value+=1;jPosUpCont++;}
		else if (jPosUpCont == J_POS_CONT_MAX && curOption->value<MAX_ORDER){curOption->value+=1;jPosUpCont = 1;}
		else{jPosUpCont++;}
	}else{jPosDownCont = 0;jPosUpCont = 0;}

	if(this->b1){
		this->b1Pos = PRESSED;
	}else if(this->b1Pos == PRESSED){
		filter_order = options[0].value;
		paramModified = true;
		*(this->curMenu) = curOption->goToMenu;
		this->b1Pos = RELEASED;
	}else this->b1Pos = RELEASED;
	
}

void OrdemDoFiltroMenu::setMenus(MenuTemplate* mainMenu){
	options[0].goToMenu = mainMenu;
}

/* LigaDesligaMenu Class*/

LigaDesligaMenu::LigaDesligaMenu(tContext* grContext,int* width,int* height,MenuTemplate** curMenu):MenuTemplate(grContext,width,height,curMenu){
	strcpy(options[0].optionName,"Liga");
	options[0].selected = true;
	options[0].value = 1;
	
	strcpy(options[1].optionName,"Desliga");
	options[1].selected = false;
	options[1].value = 0;
	
	options[0].next = &options[1];
	options[1].prev = &options[0];
	
	options[1].next = &options[0];
	options[0].prev = &options[1];
	
	numOptions = 2;
	curOption = &options[0];
	jPos = MID;
	b1Pos = RELEASED;
	b2Pos = RELEASED;
	
	jPosUpCont = 0;
	jPosDownCont = 0;
	
}

void LigaDesligaMenu::draw(){
	GrContextForegroundSet(this->grContext, ClrWhite);
	GrContextBackgroundSet(this->grContext, ClrBlack);
	this->drawLine(0,"---------------------",false);
	this->drawLine(1,"|    Liga/Desliga    |",false);
	this->drawLine(2,"---------------------",false);
	this->drawLine(3,"",false);
	
	for(int i = 0; i < numOptions;i++)
		this->drawLine(i+4,options[i].optionName,options[i].selected);
	
	GrFlush(this->grContext);
}

void LigaDesligaMenu::update(){
	if(this->jY<595){
		if(this->jPos != DOWN){
			curOption->selected = false;
			curOption = curOption->next;
			curOption->selected = true;
			jPos = DOWN;
		}
	jPos = DOWN;
	}else if(this->jY>3500){
		if(jPos != UP){
			curOption->selected = false;
			curOption = curOption->prev;
			curOption->selected = true;
			jPos = UP;
		}
		jPos = UP;
	}else{
		jPos = MID;
	}
	if(this->b1){
		this->b1Pos = PRESSED;
	}else if(this->b1Pos == PRESSED){
		on = (bool)curOption->value;
		*(this->curMenu) = curOption->goToMenu;
		this->b1Pos = RELEASED;
	}else this->b1Pos = RELEASED;
}

void LigaDesligaMenu::setMenus(MenuTemplate* mainMenu){
	options[0].goToMenu = mainMenu;
	options[1].goToMenu = mainMenu;
}
