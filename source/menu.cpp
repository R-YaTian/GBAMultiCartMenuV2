#include "menu.h"
#include "gba_input.h"
#include "gba_systemcalls.h"
#include "misc.h"
#include "font.h"
#include "splash.h"

Menu::Menu(const char* title){
	clear();
	setTitle(title);

	snd_launch = {
		{ SFX_LAUNCH } ,		// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		128,	// panning
	};

	snd_back = {
		{ SFX_BACK } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		128,	// panning
	};

	snd_switch = {
		{ SFX_SWITCH } ,		// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		128,	// panning
	};

	mmStart(MOD_FLATOUTLIES, MM_PLAY_LOOP);
}

Menu::Menu(){
	clear();
}

void Menu::clear(){
	title = "";
	height = CONSOLE_HEIGHT;
	selected = 0;
	offset = 0;
	move_type = 0;
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

int Menu::getDecision(int cur_x, int cur_y){
	if(options.size() < 1){return -1;}
	bool hasChosen = false;
	VBlankIntrWait();
	setPos(cur_x, cur_y);
	printf_zh(">");
	do {
		move_type = 0;
		scanKeys();
		auto key = keysDown();
		if(key & KEY_UP){
			mmEffectEx(&snd_switch);
			moveUp();
		}else if(key & KEY_DOWN){
			mmEffectEx(&snd_switch);
			move_type = 1;
			moveDown();
		}

		if(key & KEY_A){
			launch = mmEffectEx(&snd_launch);
			hasChosen = 1;
		}

		if(key & KEY_B){
			mmEffectEx(&snd_back);
			halClearChar(cur_x, (selected - offset) * 14 + cur_y, (u16*)gImage_splash);
			selected -= selected % 10;
		}

		if((keysHeld() & (KEY_DOWN | KEY_UP)) && timer < 20)
			timer++;
		else
			timer = 0;

		if (timer == 20) {
			while((keysHeld() & KEY_UP) && !(keysHeld() & KEY_DOWN)){
				moveUp();
				VBlankIntrWait();
				printCursor(cur_x, cur_y);
				scanKeys();
			}
			while((keysHeld() & KEY_DOWN) && !(keysHeld() & KEY_UP)){
				move_type = 1;
				moveDown();
				VBlankIntrWait();
				printCursor(cur_x, cur_y);
				scanKeys();
			}
			timer = 0;
		}

		VBlankIntrWait();
		printCursor(cur_x, cur_y);
	} while(hasChosen == false);
	mmStop();
	return selected;
}

void Menu::printSelection(){
	syncDisable();
	clearConsole();
	halDrawFullScreenBG((u16*)gImage_splash);
	printf_zh("%s\n", title);
	//offset:int selected:int 
	for(int i=0;i<height-1;i++){
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

void Menu::printCursor(int cur_x, int cur_y) {
    syncDisable();
    int index_fix = move_type ? -14 : 14;

    // selected: int
    halClearChar(cur_x, (selected - offset) * 14 + index_fix + cur_y, (u16*)gImage_splash);
    setPos(cur_x, (selected - offset) * 14 + cur_y);
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
		offset-=10;
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
	if((selected-offset) && ((selected-offset) % (height-2) == 0)){
		selected++;
		offset+=10;
		VBlankIntrWait();
		printSelection();
		move_type = 0; // 如果翻到下一页，则重置光标移动类型
	}else{
		selected++;
	}
}
/*
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
*/
