//////////////////////////////////////////////////////////////////////
// Demo1 ARM9 Code - Based on an example shipped with NDSLIB.
// Chris Double (chris.double@double.co.nz)
//////////////////////////////////////////////////////////////////////

#include <nds.h>
#include <maxmod9.h>
#include <nds/arm9/console.h>
#include <nds/arm9/sound.h>
#include <stdio.h>
#include <stdlib.h>
#include "gfx.h"
//#include "sound.h"
#include <time.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#define NUM_SPRITES 128
#define AY 16
#define AX 12
#define UP    1
#define RIGHT 2
#define DOWN  3
#define LEFT  4

SpriteEntry OAMCopySub[128];

//keys
uint16 keysPressed = ~(REG_KEYINPUT);      
//uint16 specialKeysPressed = ~IPC->buttons;


//simple sprite struct
typedef struct {
        int x,y;                             //location 
        int dx, dy;                          //speed
        SpriteEntry* oam;        
        int gfxID;                           //graphics lovation
}Sprite;


long score;
u8 level;
u8 lines;
u8 array[AY][AX];
u8 arraycp[AY][AX];

mm_sound_effect sounds[MSL_BANKSIZE];

//---------------------------------------------------------------------------------
void Sleep(int i)
//---------------------------------------------------------------------------------
{
        int y;
        for (y = 0; y < i; y++)
        {
           swiWaitForVBlank();
        }
}


//---------------------------------------------------------------------------------
void SleepSeconds(int i)
//---------------------------------------------------------------------------------
{
   time_t unixTime = time(NULL);
   struct tm* timeStruct = gmtime((const time_t *)&unixTime);

   int wait_until = (timeStruct->tm_sec + i) % 60;

   int cur_time = timeStruct->tm_sec;

   while(cur_time != wait_until){
      cur_time = timeStruct->tm_sec;
   }
   
}

//---------------------------------------------------------------------------------
void drawShapes(u16* map) {
//---------------------------------------------------------------------------------
int x,y;

  for (y=0; y< AY; y++)
    for (x=0; x < AX; x++){ 

    if(array[y][x]==0){                                  //nothing
         map[(23-(x*2))*32+(y*2)] =0|array[y][x]<<12;
     map[(23-(x*2))*32+(y*2)+1]   =0|array[y][x]<<12;
     map[(23-(x*2))*32+(y*2)-32]  =0|array[y][x]<<12;
     map[(23-(x*2))*32+(y*2)-31]  =0|array[y][x]<<12;
     }
    
    else if(array[y][x]==5){                             //black bombs
         map[(23-(x*2))*32+(y*2)] =8;//|array[y][x]<<12;
     map[(23-(x*2))*32+(y*2)+1]   =9;//|array[y][x]<<12;
     map[(23-(x*2))*32+(y*2)-32]  =6;//|array[y][x]<<12;
     map[(23-(x*2))*32+(y*2)-31]  =7;//|array[y][x]<<12;
     }
     
    else if((array[y][x]>=6)&&(array[y][x]<=9)){         //color bombs
     map[(23-(x*2))*32+(y*2)]    =12|(array[y][x]-5)<<12;
     map[(23-(x*2))*32+(y*2)+1]  =13|(array[y][x]-5)<<12;
     map[(23-(x*2))*32+(y*2)-32] =10|(array[y][x]-5)<<12;
     map[(23-(x*2))*32+(y*2)-31] =11|(array[y][x]-5)<<12;
     } 
     
     else if(array[y][x]==10){                           //gray square
     map[(23-(x*2))*32+(y*2)]    =4|6<<12;
     map[(23-(x*2))*32+(y*2)+1]  =5|6<<12;
     map[(23-(x*2))*32+(y*2)-32] =2|6<<12;
     map[(23-(x*2))*32+(y*2)-31] =3|6<<12;
    }
     
    else if(array[y][x]!=0){       
     map[(23-(x*2))*32+(y*2)]    =4|array[y][x]<<12;
     map[(23-(x*2))*32+(y*2)+1]  =5|array[y][x]<<12;
     map[(23-(x*2))*32+(y*2)-32] =2|array[y][x]<<12;
     map[(23-(x*2))*32+(y*2)-31] =3|array[y][x]<<12;
    }
  }
}

//---------------------------------------------------------------------------------
u8 checkShapes(u8 x, u8 y, u8 dir, u8 c, u8 del) {
//---------------------------------------------------------------------------------
int count = 0;


//if((x<0)||(x>=12)||(y<0)||(y>=15))
if((x>=12)||(y>=15))
        return 0;

if((array[y][x]!=c)&&(array[y][x]!=5)&&(array[y][x]-5!=c))
    return 0;

if(arraycp[y][x]==1)
   return 0;

arraycp[y][x]=1;

if(del==1)
array[y][x]=0;        
    
        switch(dir){
        
                case UP: count= 
                                checkShapes(x,y-1,UP,   c, del)+
                                checkShapes(x+1,y,RIGHT,c, del)+
                                checkShapes(x-1,y,LEFT, c, del);
                                
                             break;
                case RIGHT: count= 
                                checkShapes(x,y-1,UP,   c, del)+
                                checkShapes(x+1,y,RIGHT,c, del)+
                                checkShapes(x,y+1,DOWN, c, del); 
                             break;
                case DOWN: count= 
                                checkShapes(x+1,y,RIGHT,c, del)+
                                checkShapes(x,y+1,DOWN, c, del)+
                                checkShapes(x-1,y,LEFT, c, del);
                                
                             break;
                case LEFT: count= 
                                checkShapes(x,y-1,UP,   c, del)+
                                checkShapes(x,y+1,DOWN, c, del)+ 
                                checkShapes(x-1,y,LEFT, c, del);
                             break;
                }
        
        return count+1;

}
//---------------------------------------------------------------------------------
u8 check(u8 x, u8 y, u8 del) {
//---------------------------------------------------------------------------------
u8 c=array[y][x];

//if((x<0)||(x>=12)||(y<0)||(y>=15))
if((x>=12)||(y>=15))
        return 0;

  for (int i=0; i< AY-1; i++)
    for (int j=0; j < AX; j++)
      arraycp[i][j]=0;

  arraycp[y][x]=1;

  if(del==1)
    array[y][x]=0;

return          checkShapes(x+1,y,RIGHT,c, del)+
                checkShapes(x-1,y,LEFT, c, del)+
                checkShapes(x,y-1,UP,   c, del)+
                checkShapes(x,y+1,DOWN, c, del)+1; 
}

//---------------------------------------------------------------------------------
void clearShapes() {
//---------------------------------------------------------------------------------

  for (int i=0; i< AY-1; i++)
    for (int j=0; j < AX; j++){
      arraycp[i][j]=array[i][j];
      array[i][j]=0;
      }
      
int k=14;
 
for(int i=0; i<12; i++){
  k=14;          
  for(int j=14;j>=0; j--)
    if(arraycp[j][i]!=0)
     array[k--][i]=arraycp[j][i];
 }
 
 
 //left
   for (int i=0; i< AY-1; i++)
    for (int j=0; j < AX; j++){
      arraycp[i][j]=array[i][j];
      array[i][j]=0;
      }
      

  k=5;          
  for(int i=5;i>=0; i--)
    if(arraycp[14][i]!=0){
    for(int j=0;j<15;j++)
     array[j][k]=arraycp[j][i];
     k--;
      }
 
 
  k=6;          
  for(int i=6;i<12; i++)
    if(arraycp[14][i]!=0){
    for(int j=0;j<15;j++)
     array[j][k]=arraycp[j][i];
     k++;
     }
  
}

//---------------------------------------------------------------------------------
u8 scrollShapes() {
//---------------------------------------------------------------------------------
for(int i=0;i<12;i++)
 if(array[0][i]!=0)
   return 0;


for (int j=0; j<15; j++)
  for(int i=0; i<12; i++)
  array[j][i]=array[j+1][i];

  for(int i=0; i<12; i++)
   array[15][i]=0;
   
   return 1;

}

//---------------------------------------------------------------------------------
u16 random ( u16 range ){
//---------------------------------------------------------------------------------

   return rand() % range;
}


//---------------------------------------------------------------------------------
u8 blackBomb(u8 x, u8 y) {
//---------------------------------------------------------------------------------

int x1,x2,y1,y2;
u8 c=0;

x1=x-4;  if(x1<0) x1=0;
x2=x+4;  if(x2>11) x2=11;
y1=y-2;  if(y1<0) y1=0;
y2=y+2;  if(y2>14) y2=14;

for(int j=y1; j<=y2; j++)
 for(int i=x1; i<=x2; i++)
   if(array[j][i]!=0){
   array[j][i]=0;
   c++;
   }
   
x1=x-2;  if(x1<0) x1=0;
x2=x+2;  if(x2>11) x2=11;
y1=y-4;  if(y1<0) y1=0;
y2=y+4;  if(y2>14) y2=14;

for(int j=y1; j<=y2; j++)
 for(int i=x1; i<=x2; i++)
   if(array[j][i]!=0){
   array[j][i]=0;
   c++;
   }
   
   return c;
    
}

//---------------------------------------------------------------------------------
u8 colorBomb(u8 color) {
//---------------------------------------------------------------------------------
u8 c=0;


for (int j=0; j<15; j++)
  for(int i=0; i<12; i++)
    if((array[j][i]==color)||(array[j][i]==(color-5))){
      array[j][i]=0; 
      c++;
      }
    return c;
}

//---------------------------------------------------------------------------------
void updateScore(int num) {
//---------------------------------------------------------------------------------
int cmp;
        if(num<3) return;
        
        num=(num-2)*3;
         
        //printf("%d",num);
        
        score=score+num;
        
        
        //bonus for clearing sceen    
  cmp=0;
 for(int i=0; i<12; i++)
  if(array[14][i]!=0)
    cmp=1;
    
  if(cmp==0){
   score=score+20000;
   cmp=1;}        
}

//---------------------------------------------------------------------------------
void bonusScore(u16* map) {
//---------------------------------------------------------------------------------
int cmp;
for(int j=0; j<15; j++){
   cmp=0;
 for(int i=0; i<12; i++){
  if(array[j][i]!=0)
    cmp=1;
  else {array[j][i]=10;
  
  Sleep(1);

  mmEffectEx(&sounds[SFX_BUBBLE]);
  
  }
  drawShapes(map);
  }
  if(cmp==0)
  {
   score=score+100;
    
    
    printf("\x1b[2J");
    printf("\x1b[10;7H%ld",score);
    printf("\x1b[15;7H%d",level);
    printf("\x1b[19;7H%d",lines);
   }
    
   Sleep(6);
    mmEffectEx(&sounds[SFX_BOTTLE]);

   
  }
}


//---------------------------------------------------------------------------------
int levels(u16* map);
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
int main(void){
//---------------------------------------------------------------------------------

//  Sprite sprites[NUM_SPRITES];
  
  u16* tileM = (u16*)CHAR_BASE_BLOCK(5);
  u16* mapM = (u16*)SCREEN_BASE_BLOCK(20);
  
  u16* tile = (u16*)CHAR_BASE_BLOCK_SUB(0);
  u16* map1 = (u16*)SCREEN_BASE_BLOCK_SUB(20);
  
  u16* tile2= (u16*)CHAR_BASE_BLOCK_SUB(5);
  u16* map2 = (u16*)SCREEN_BASE_BLOCK_SUB(25);
  
  u16 *framebuffer =VRAM_A; 
  
  u8 end=1;

  srand(time(NULL));

  mmInitDefaultMem((mm_addr)soundbank_bin);

  for (int i = 0; i < MSL_BANKSIZE; i++) {
      mmLoadEffect(i);
      sounds[i] = {
          { i } ,		// id
          (int)(1.0f * (1<<10)),	// rate
          0,			// handle
          255,	// volume
          255,	// panning
      };
  }
  
//  powerON(POWER_ALL);
  
  
          // initialise the irq dispatcher
        //irqInit();
        // a vblank interrupt is needed to use swiWaitForVBlank()
        // since the dispatcher handles the flags no handler is required
        //irqSet(IRQ_VBLANK, NULL);
        //irqEnable(IRQ_VBLANK);
    
  while(1){
          
//bitmap mode
  scanKeys();
  videoSetMode(MODE_FB0);
  vramSetBankA(VRAM_A_LCD);
  
  dmaCopy((u8*)logoPalette , (u16*)BG_PALETTE , sizeof(logoPalette));
  dmaCopy((u8*)logoData , (u16*) framebuffer, sizeof(logoData));
 
  Sleep(60);  
 
  dmaCopy((u8*)titlePalette , (u16*)BG_PALETTE , sizeof(titlePalette));
  dmaCopy((u8*)titleData , (u16*) framebuffer, sizeof(titleData)); 
   
  end=1;
  while(end==1){
   int pressed = keysDown();   // buttons pressed this loop
   if(pressed & KEY_A) 
      end=0;
   scanKeys();
  }
//bitmap mode end 
  

  
  // Use the main screen for output
  videoSetMode   (DISPLAY_SPR_ACTIVE | 
                  DISPLAY_SPR_1D_LAYOUT |
                  MODE_0_2D | 
                  DISPLAY_BG0_ACTIVE |
                  DISPLAY_BG1_ACTIVE | 
                  DISPLAY_BG2_ACTIVE | 
                  DISPLAY_BG3_ACTIVE );
               
  videoSetModeSub(DISPLAY_SPR_ACTIVE | 
                  DISPLAY_SPR_1D_LAYOUT |
                  MODE_0_2D | 
                  DISPLAY_BG0_ACTIVE | 
                  DISPLAY_BG1_ACTIVE | 
                  DISPLAY_BG2_ACTIVE | 
                  DISPLAY_BG3_ACTIVE );
  
  
  //set video ram memory banks
  vramSetPrimaryBanks(VRAM_A_MAIN_BG,VRAM_B_MAIN_BG,VRAM_C_SUB_BG,VRAM_D_SUB_SPRITE);  
  
  
  REG_BG0CNT = BG_MAP_BASE(31);
  REG_BG1CNT = BG_COLOR_16 | BG_RS_64x64 | BG_TILE_BASE(5) | BG_MAP_BASE(20);
  REG_BG2CNT = BG_COLOR_16 | BG_RS_64x64 | BG_TILE_BASE(0) | BG_MAP_BASE(10);
  REG_BG3CNT = BG_COLOR_16 | BG_RS_64x64 | BG_TILE_BASE(0) | BG_MAP_BASE(10);

  REG_BG0CNT_SUB = BG_COLOR_16 | BG_RS_64x64 | BG_TILE_BASE(5) | BG_MAP_BASE(25);
  REG_BG1CNT_SUB = BG_COLOR_16 | BG_RS_64x64 | BG_TILE_BASE(0) | BG_MAP_BASE(20);
  REG_BG2CNT_SUB = BG_COLOR_16 | BG_RS_64x64 | BG_TILE_BASE(0) | BG_MAP_BASE(10);
  REG_BG3CNT_SUB = BG_COLOR_16 | BG_RS_64x64 | BG_TILE_BASE(0) | BG_MAP_BASE(10);
  
  
  //Load Tiles
  PrintConsole topScreen;
  consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
  
  dmaCopy((u8*)avalData , (u16*)tileM , sizeof(avalData));
  
  dmaCopy((u8*)shapesData , (u16*)tile , sizeof(shapesData));
  dmaCopy((u8*)frameData , (u16*)tile2 , sizeof(frameData));  

    

  //Load Palettes
  BG_PALETTE[0] = RGB15(0,0,0);
  BG_PALETTE[255] = RGB15(31,0,0); //set text color to white
  
  dmaCopy((u8*) avalPalette , (u16*)BG_PALETTE+16 ,  sizeof(avalPalette));
  
  dmaCopy((u8*) whitePalette , (u16*)BG_PALETTE_SUB ,  sizeof(whitePalette));
  dmaCopy((u8*) redPalette , (u16*)(BG_PALETTE_SUB+16) , sizeof(redPalette));
  dmaCopy((u8*) greenPalette , (u16*)(BG_PALETTE_SUB+32) , sizeof(greenPalette));
  dmaCopy((u8*) bluePalette , (u16*)(BG_PALETTE_SUB+48) , sizeof(bluePalette));
  dmaCopy((u8*) whitePalette , (u16*)(BG_PALETTE_SUB+64) , sizeof(whitePalette));
  
  dmaCopy((u8*) grayPalette , (u16*)(BG_PALETTE_SUB+96) , sizeof(grayPalette));
  
  
  dmaCopy((u8*) framePalette , (u16*)(BG_PALETTE_SUB+80) , sizeof(framePalette));
   
//set up background sceen

  for(int j=0;j<24;j++)
   for(int i=0;i<32;i++)
    mapM[j*32+i]=(j*32+i) | 1<<12;
  
//set up bottom frame
  for(int i=0; i<12;i++){
           map2[i*64+30]  =4|5<<12;
     map2[i*64+31]  =5|5<<12;
     map2[i*64 +30 +32]  =6|5<<12;
     map2[i*64 +31 +32]  =7|5<<12; 
    }

/*    
    sprites[0].x=20;
    sprites[0].y=20;
    sprites[0].oam=&OAMCopySub[0];
    sprites[0].gfxID=0;
    
    sprites[0].oam->attribute[0] = ATTR0_COLOR_16 | ATTR0_TALL;  
    sprites[0].oam->attribute[1] = ATTR1_SIZE_32;
        sprites[0].oam->attribute[2] = sprites[i].gfxID;
    
    //sprite palette and data
    dmaCopy((u8*)guyPalette ,(u16*) SPRITE_PALETTE_SUB , sizeof(guyPalette));
    dmaCopy((u8*)guyData , (u16*) SPRITE_GFX_SUB , sizeof(guyData));
*/    
   

    score=0;
    level=1;


    while( levels(map1)==2)         
        level++;
        
//gameover screen        
  videoSetMode(MODE_FB0);
  vramSetBankA(VRAM_A_LCD);
  dmaCopy((u8*)gameoverPalette , (u16*)BG_PALETTE , sizeof(gameoverPalette));
  dmaCopy((u8*)gameoverData , (u16*) framebuffer, sizeof(gameoverData));
   
    mmEffectEx(&sounds[SFX_BOSS]);
    end=1;
  while(end==1){
   int pressed = keysDown();   // buttons pressed this loop
   if(pressed & KEY_A) 
      end=0;
   scanKeys();
  }
        
 }
 return 0;
}
//---------------------------------------------------------------------------------
int levels(u16* map){
//---------------------------------------------------------------------------------
        
  int x,y;
  int end=1;
  int pause;
  int counter, pos;
  u8 speed = 0;
  u8 rnd =0;
  touchPosition touch;
  
   switch(level){
    case 1: speed=30; rnd=196; lines=25; 
      break;
    case 2: speed=25; rnd=196; lines=30; //bonus after level2
      break;
    case 3: speed=20; rnd=196; lines=30;
      break;
    case 4: speed=20; rnd=200; lines=30; //bonus after level4
      break;
    case 5: speed=15; rnd=200; lines=35;
      break;
    case 6: speed=15; rnd=200; lines=50; //make color changing tiles
      break;
    case 7: speed=10; rnd=200; lines=50;
      break;
    case 8: speed= 5; rnd=200; lines=75; //bonus after level8 and level0
      break; 
    case 9: speed=0; rnd=200; lines=200; 
      break;
  }

  
        
  for (int j=0; j< AY; j++)
    for (int i=0; i < AX; i++)
      array[j][i]=0;//rand()%4+1;
      
  // set the generic sound parameters
//        setGenericSound(        11025,        /* sample rate */
//                                127,        /* volume */  //127
//                                64,                /* panning */
//                                1 );        /* sound format*/
        
        
  counter=0;
  pos=0;
  
  //begin with 4 lines
    for (int j=11; j< AY-1; j++)
    for (int i=0; i < AX; i++)
     if(rnd>196)
      array[j][i]=random(4)+1;
     else
      array[j][i]=random(3)+1;
      
  end=1;
      
  while(end==1) {
        
//increase counter  
        if(counter > speed)
         counter=0;
        else counter++;
        
        if(counter==0)
          {
          int r= random(rnd)+1;
          
               if(r==200)
           array[15][pos]=9;
          else if(r==199)
           array[15][pos]=8;
          else if(r==198)
           array[15][pos]=7;
          else if(r==197)
           array[15][pos]=6;
          else if(r==196)
           array[15][pos]=5;
          else if(rnd>196)
           array[15][pos]=random(4)+1;
          else 
           array[15][pos]=random(3)+1;
           
          if (pos==11){
           pos=-1;
           lines--;
           
           if(scrollShapes()==0)
             end=0;
                            
           if(lines == 5)
             mmEffectEx(&sounds[SFX_ALERT1]);
           else
             mmEffectEx(&sounds[SFX_BOTTLE]);
       }
       else mmEffectEx(&sounds[SFX_BUBBLE]);
           pos++;          
      }
     
    if(lines==0)
      end=2;
      
    swiWaitForVBlank();

    scanKeys();
   
    touchRead(&touch);     //sceen

    int pressed = keysDown();   // buttons pressed this loop
    int held = keysHeld();      // buttons currently held

    iprintf("\x1b[10;0HTouch x = %04i, %04i\n", touch.rawx, touch.px);
    iprintf("Touch y = %04i, %04i\n", touch.rawy, touch.py);
    
    x=(192-touch.py)/16;
    y=touch.px/16;
        
    if(pressed & KEY_START){
            
        for(int j=0; j<32;j++)
         for(int i=0; i<24;i++)
           map[32*i+j]=0;
         
    pause=1;
    while(pause==1){
         scanKeys();
    if(pressed & KEY_START) 
      pause=0;
     }
    }

    if(pressed & KEY_TOUCH) {
        if(y!=15 && array[y][x]==5){
            updateScore(blackBomb(x,y));
            clearShapes();
            mmEffectEx(&sounds[SFX_EXP]);
        } else if(y!=15 && array[y][x]>5 && array[y][x]<10){
            updateScore(colorBomb(array[y][x]));
            clearShapes();
            mmEffectEx(&sounds[SFX_EXP]);
        } else if(array[y][x]==0){
            mmEffectEx(&sounds[SFX_MISS]);
        } else if(check(x,y,0)>=3){
            updateScore(check(x,y,1)); 
            clearShapes(); 
            mmEffectEx(&sounds[SFX_DESTROY]);
        } else {
            mmEffectEx(&sounds[SFX_MISS]);
        }
    }
    
    printf("\x1b[2J");
    printf("\x1b[10;7H%ld",score);
    printf("\x1b[15;7H%d",level);
    printf("\x1b[19;7H%d",lines);
        
    drawShapes(map);
    
  }
  
  bonusScore(map);
  return end;
  
}
