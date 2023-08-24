#include "menu.h"
#include "gba_input.h"
#include "gba_systemcalls.h"
#include "misc.h"
#include "font.h"


Menu::Menu(const char* title){
	clear();
	setTitle(title);
}

Menu::Menu(){
	clear();
}

void Menu::clear(){
	title = "";
	height = CONSOLE_HEIGHT;
	selected = 0;
	offset = 0;
	options.clear();	
}

void Menu::addOption(std::string name){
	options.push_back(name);
}

int Menu::printList(){
    VBlankIntrWait();
    printSelection();
    return -1;
}

int Menu::getDecision(){
	if(options.size() < 1){return -1;}
	bool hasChosen = false;
	VBlankIntrWait();
	setPos(0, 28);
	printf_zh(">");
	do {
		scanKeys();
		auto key = keysDown();
		if(key & KEY_UP){
			moveUp();
		}else if(key & KEY_DOWN){
			moveDown();
		}

		if(key & KEY_A){
			hasChosen = 1;
		}
		if(key & KEY_B){
			hasChosen = 1; selected = -1;
		}

		if(keysHeld() & KEY_DOWN && timer > -20){
			timer--;
		}else if(keysHeld() & KEY_UP && timer < 20){
			timer++;
		}else timer = 0;

		if(timer == 20 || timer == -20){
			while(keysHeld() & KEY_UP){
				moveUp();
				VBlankIntrWait();
				printCursor(2);
				scanKeys();
			}
			while(keysHeld() & KEY_DOWN){
				moveDown();
				VBlankIntrWait();
				printCursor(1);
				scanKeys();
			}
		}

		VBlankIntrWait();
		printCursor();
	} while(hasChosen == false);
    return selected;
}


void Menu::printSelection(){
	syncDisable();
	clearConsole();
	printf_zh("%s\n\n", title);
	//offset:int selected:int 
	for(int i=0;i<height-2;i++){
		//if((int)selected == (int)(i+offset)){
			//printf_zh(">");
		//}
		if((int)(i+offset)<(int)options.size()){
			printf_zh(" %s\n", options.at(i+offset).c_str());
		}
	}

	syncEnable();
	VBlankIntrWait();
	syncToScreen();
}

void Menu::printCursor(int move_type) {
    syncDisable();

    // selected: int
    if (move_type == 0)
    {
        halClearChar(0, (selected - offset + 2) * 14 + 14);
        halClearChar(0, (selected - offset + 2) * 14 - 14);
    } else if (move_type == 1) {
        halClearChar(0, (selected - offset + 2) * 14 - 14);
    } else if (move_type == 2) {
        halClearChar(0, (selected - offset + 2) * 14 + 14);
    }
    setPos(0, (selected - offset + 2) * 14);
    printf_zh(">");

    syncEnable();
    VBlankIntrWait();
    syncToScreen();
}

void Menu::setTitle(const char* t){
	title = t;
}

int Menu::size(){
	return options.size();
}



void Menu::moveUp(){
	if(selected == 0){
		return;
	}
	if(selected-offset == 0){
		selected--;
		offset-=9;
		VBlankIntrWait();
		printSelection();
	}else{
		selected--;
	}
}

void Menu::moveDown(){
	if(selected >= (int)options.size()-1){
		return;
	}
	if((selected-offset) && ((selected-offset) % (height-3) == 0)){
		selected++;
		offset+=9;
		VBlankIntrWait();
		printSelection();
	}else{
		selected++;
	}
}



int Menu::getNumerical(){
	
	selected = numerical_min;
	bool hasChosen = false;
	
	do{
	
		scanKeys();
		auto key = keysDown();
		if(key & KEY_UP && selected < numerical_max){
			selected++;

		}else if(key & KEY_DOWN && selected > numerical_min){
			selected--;
		}
		
		if(key & KEY_A){
			hasChosen = 1;
		}
		if(key & KEY_B){
			clearConsole();
			return -1;
		}

		if(keysHeld() & KEY_DOWN && timer > -20){
			timer--;
		}else if(keysHeld() & KEY_UP && timer < 20){
			timer++;
		}else timer = 0;

		if(timer == 20 || timer == -20){
			while(keysHeld() & KEY_UP && selected < numerical_max){
				selected++;
				VBlankIntrWait();
				printNumericalSelection();
				scanKeys();
			}
			while(keysHeld() & KEY_DOWN && selected > numerical_min){
				selected--;
				VBlankIntrWait();
				printNumericalSelection();
				scanKeys();
			}
		}

		VBlankIntrWait();
		printNumericalSelection();

	}while(hasChosen == false);
	clearConsole();
	return selected*numerical_increment;
}



void Menu::printNumericalSelection(){
	clearConsole();
	printf_zh("%s %02X\n\nUp/Down - Select\nA - Confirm\nB - Back", title, selected*numerical_increment);	
}




Menu numericalMenu(const char* title, int min, int max, int increment){
	Menu menu;
	menu.setTitle(title);
	menu.numerical_min = min/increment;
	menu.numerical_max = max/increment;
	menu.numerical_increment = increment;
	return menu;
}
