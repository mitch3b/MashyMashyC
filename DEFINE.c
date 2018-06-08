// let's define some stuff

#define PPU_CTRL		*((unsigned char*)0x2000)
#define PPU_MASK		*((unsigned char*)0x2001)
#define PPU_STATUS		*((unsigned char*)0x2002)
#define OAM_ADDRESS		*((unsigned char*)0x2003)
#define SCROLL			*((unsigned char*)0x2005)
#define PPU_ADDRESS		*((unsigned char*)0x2006)
#define PPU_DATA		*((unsigned char*)0x2007)
#define OAM_DMA			*((unsigned char*)0x4014)


#define RIGHT		0x01
#define LEFT		0x02
#define DOWN		0x04
#define UP			0x08
#define START		0x10
#define SELECT		0x20
#define B_BUTTON	0x40
#define A_BUTTON	0x80

const unsigned char BUTTON_TEXT[]= {"up"};
const unsigned char DOWN_TEXT[] = {"down"};
const unsigned char LEFT_TEXT[] = {"left"};
const unsigned char RIGHT_TEXT[] = {"right"};
const unsigned char START_TEXT[] = {"start"};
const unsigned char SELECT_TEXT[] = {"select"};
const unsigned char A_TEXT[] = {"a"};
const unsigned char B_TEXT[] = {"b"};

const unsigned int TIMER_OPTIONS[] = {1, 2, 5, 10};

// Globals
// our startup code initialized all values to zero
#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char index;
unsigned char index4;
unsigned char X1;
unsigned char Y1;
unsigned char state;
unsigned char state4;
unsigned char drawGameScreen;
unsigned char gameState;
unsigned char currentMenuOption;
unsigned char numPlayers;
unsigned char joypad1;
unsigned char joypad1old;
unsigned char joypad1test;
unsigned char joypad2;
unsigned char joypad2old;
unsigned char joypad2test;
unsigned char Horiz_scroll;
unsigned char Horiz_scroll_Old;
unsigned int Scroll_Adjusted_X;
// got rid of Vert_scroll, now always 0
unsigned char Nametable;
unsigned char NametableB;
unsigned char walk_count; // changes the animation
unsigned char buttonChosen;
unsigned char secondsChosen;
unsigned char count;
unsigned char direction; // 0 = R, 1 = L
unsigned char collision_Index;
unsigned char high_byte;
unsigned char collision;
unsigned char temp;


#pragma bss-name(push, "OAM")
unsigned char SPRITES[256]; //64 sprites * 4 bytes
// OAM equals ram addresses 200-2ff


#include "BG/TitleScreen.h"
#include "BG/MenuScreen.h"
#include "BG/GameScreen.h"

// collision maps called C1 and C2
// now their value is 0-11, which will index to this array...

const unsigned char PLATFORM[]={ // which metatiles act like platforms
	0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0};

//.db $0F,$30,$07,$16,$31,$02,$38,$3C,$0F,$1C,$15,$14,$31,$02,$38,$3C ;sprite palette data
const unsigned char PALETTE[]={
0x0F, 0x30, 0x07, 0x16, 0x0F, 0x10, 0x00, 0x16,  0x38, 0x39, 0x3A, 0x3B,  0x3C, 0x3D, 0x3E, 0x0F,
0x0F, 0x10, 0x00, 0x16,  0x38, 0x39, 0x3A, 0x3B,  0x3C, 0x3D, 0x3E, 0x0F, 0x0F, 0x30, 0x07, 0x16};

const unsigned char MetaSprite_Y[] = {0, 0, 8, 8}; // relative y coordinates

const unsigned char MetaSprite_Tile_R[] = { // tile numbers, right
	0, 1, 0x10, 0x11, 	// walk 0, 2
	2, 3, 0x12, 0x13,	// walk 1
	4, 5, 0x14, 0x15, 	// walk 3
	6, 7, 0x16, 0x17};	// jump

const unsigned char MetaSprite_Attrib_R[] = {0, 0, 0, 0}; // attributes = not flipped

const unsigned char MetaSprite_X[] = {0, 8, 0, 8}; // relative x coordinates
// we are using 4 sprites, each one has a relative position from the top left sprite

const unsigned char MetaSprite_Tile_L[] = { // tile numbers, left
	1, 0, 0x11, 0x10, 	// walk 0, 2
	3, 2, 0x13, 0x12,	// walk 1
	5, 4, 0x15, 0x14,	// walk 3
	7, 6, 0x17, 0x16};	// jump

const unsigned char MetaSprite_Attrib_L[] = {0x40, 0x40, 0x40, 0x40}; //attributes = H flipped


// Prototypes
void All_Off(void);
void All_On(void);
void Reset_Scroll (void);
void Load_Palette(void);
void hide_sprites (void);
void DrawMenuSprites(void);
void update_Sprites (void);
void menu_logic(void);
void Draw_Background(void);
void Draw_Gamescreen(void);
void buttonPressed(int playerInput, int playerInputOld, int button);

void __fastcall__ memcpy (void* dest, const void* src, int count);
void Wait_Vblank(void);
void __fastcall__ UnRLE(const unsigned char *data);
void Get_Input(void);
