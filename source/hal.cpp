#include "hal.h"
#include "gba_video.h"

u16 (*vram)[240] = (u16 (*)[240])VRAM;
EWRAM_BSS u16 vramBuf[SCREEN_HEIGHT][SCREEN_WIDTH];
auto VRAM_SIZE = sizeof(u16)*SCREEN_WIDTH*SCREEN_HEIGHT;
bool sync_enable = true;

int halGetScreenWidth() { return SCREEN_WIDTH; }

int halGetScreenHeight() { return SCREEN_HEIGHT; }

void halDrawPixel(int x, int y, u16 color) { vramBuf[y][x] = color; }

const char *halGetFontDataPtr() {
  return my_font;
}

void syncEnable(){
  sync_enable = true;
}
void syncDisable(){
  sync_enable = false;
}
void syncToScreen(){
  if(sync_enable){
    dmaCopy(vramBuf,vram,VRAM_SIZE);
    // memcpy(vram[isTop],vramBuf[isTop],VRAM_SIZE);
  }
}
void halClearScreen(){
  memset(vramBuf,0,VRAM_SIZE);
}
void halClearChar(int xx, int yy, u16 *BG){
  for (u8 y = 0; y < 14; y++) {
    for (u8 x = 0; x < 7; x++) {
      halDrawPixel(xx + x, yy + y, 0);
      if (BG != 0)
        halDrawPixel(xx + x, yy + y, *(BG+((yy + y) * 240)+xx+x)); // 擦除字符之后需要还原背景色彩
    }
  }
}
void halDrawFullScreenBG(u16 *GFX) {
    memcpy(vramBuf, GFX, sizeof(u16) * 240 * 160);
}
