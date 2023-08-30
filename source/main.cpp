#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include "flash.h"
#include "misc.h"
#include "sramtest.h"
#include "manage.h"
#include "menu.h"
#include "font.h"

#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#define TM_FREQ_256 0x0002
#define TM_FREQ_1024 0x0003
#define TM_CASCADE 0x0004
#define TM_ENABLE 0x0080
#define REG_WAITCNT	*((vu16 *)(0x4000204))

volatile static char Dummy_SaveType[]="SRAM_V113";//让存档管理器识别为SRAM存档，虽然可能认为只有256K.....

void VBlankCallback()
{
    mmVBlank();
    mmFrame();
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
IWRAM_CODE int main(void) {
//---------------------------------------------------------------------------------

	char DONT_OPT_MY_SAVE_TYPE_STR = Dummy_SaveType[0];	//不让编译器优化掉
	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqSet(IRQ_VBLANK, VBlankCallback);
	irqEnable(IRQ_VBLANK);
	SetMode(MODE_3 | BG2_ON);
	fbInit();

	// REG_IME = 1;
	REG_WAITCNT = 0x46DA;
    REG_TM2CNT_L = 65535-1872; // 1872 ticks = 1/35 secs
    REG_TM2CNT_H = TM_FREQ_256 | TM_ENABLE; // we're using the 256 cycle timer
    // cascade into tm3
    REG_TM3CNT_H = TM_CASCADE | TM_ENABLE;

	backupSramLite();
	//////////////////////

	// Init playing module
	mmInitDefault((mm_addr)soundbank_bin, 4);
	mmSetModuleVolume(500);

	////////////////////
	if(pressedKeyOnBoot(KEY_L | KEY_R)){
		int lastOffset = trySaveGame();
		askMBOffset(lastOffset);
	}

	if(!autoStartGame()){ //试图自动开始游戏
		printf_zh("未找到上次运行的游戏\n");
		pressToContinue(true);
		askMBOffset(-1);
	}

	// printf_zh("Manully boot Offset set = %d MB",offset);

	while (1) {
		VBlankIntrWait();
	}
}
