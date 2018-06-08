/*	for cc65, for NES
 *	simple platformer game
 *	doug fraker 2015
 *	feel free to reuse any code here
 */
#include "DEFINE.c"

void main (void) {
	All_Off(); // turn off screen
	Draw_Background();
	X1 = 0x80;
	Y1 = 0x70; // middle of screen
	count = 0;
	gameState = 0; //0 - title, 1 - menu, 2 - scroll, 3 - game
	buttonChosen = A_BUTTON;
	secondsChosen = 1;
	numPlayers = 1;

	Load_Palette();
	Reset_Scroll();
	Wait_Vblank();
	All_On(); // turn on screen
	hide_sprites();
	while (1){ // infinite loop
		while (NMI_flag == 0); // wait till NMI

		//every_frame();	// moved this to the nmi code in reset.s for greater stability
		Get_Input();

		if(gameState == 0) {
			if ((joypad1 & (START)) != 0){
				gameState = 1;
				drawGameScreen = 1;
				Nametable = 1;
				currentMenuOption = 0; // 0 Players, 1 button choice, 2 seconds, 3 start

				DrawMenuSprites();
				update_Sprites();
			}
		}
		else if(gameState == 1){
			//Menu
			menu_logic();
			update_Sprites();

			if(drawGameScreen == 1) {
				Draw_Gamescreen();
				drawGameScreen = 0;
			}
		}
		else if(gameState == 2) {
			hide_sprites();//TODO only need to do this bc the update_sprites logic above follows menu logic... at least i think thats the reason
			Horiz_scroll += 4;

			if(Horiz_scroll == 0) {
				Nametable = 0;
				gameState = 3;
			}
		}

		NMI_flag = 0;
	}
}

// inside the startup code, the NMI routine will ++NMI_flag and ++Frame_Count at each V-blank

void All_Off (void) {
	PPU_CTRL = 0;
	PPU_MASK = 0;
}

void All_On (void) {
	PPU_CTRL = 0x80; // screen is on, NMI on 1001 0000
	PPU_MASK = 0x1e; // enable all rendering
}

void Reset_Scroll (void) {
	PPU_ADDRESS = 0;
	PPU_ADDRESS = 0;
	SCROLL = Horiz_scroll;
	SCROLL = 0;
}

void Load_Palette (void) {
	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x00;
	PPU_ADDRESS = 0x00;
	for( index = 0; index < sizeof(PALETTE); ++index ){
		PPU_DATA = PALETTE[index];
	}
}

void hide_sprites (void) {
	//TODO don't love this but good enough for now
	for (index = 0 ; index < 64 ; index++) {
		SPRITES[4*index] = 0; //set x to offscreen
		SPRITES[4*index + 3] = 0; //set y to offsreen
	}
}

void DrawMenuSprites(void) {
	hide_sprites(); //TODO don't have to do the ones we're reassigning below
	//Which menu option
	SPRITES[0] = 0x30;
	SPRITES[1] = 0x28;
	SPRITES[2] = 0x00;
	SPRITES[3] = 0x18;
	//NumPlayers arrow
	SPRITES[4] = 0x30;
	SPRITES[5] = 0x28;
	SPRITES[6] = 0x00;
	SPRITES[7] = 0x70;

	//Button chosen 8 - 31 (max 6 letters)
	index4 = 0;
	for(index = 8 ; index < 32 ; ) {
		SPRITES[index] = 0x40;
		index++;
		SPRITES[index] = index; //TODO should go to current button choice? Or let other method take care of it
		index++;
		SPRITES[index] = 0;
		index++;
		SPRITES[index] = 0x68 + index4;
		index++;
		index4 += 8;
	}

	//Seconds
	SPRITES[32] = 0x50;
	SPRITES[33] = secondsChosen / 10;
	SPRITES[34] = 0;
	SPRITES[35] = 0x88;
	SPRITES[36] = SPRITES[32];
	SPRITES[37] = secondsChosen % 10;
	SPRITES[38] = 0;
	SPRITES[39] = SPRITES[35] + 8;
}

/**
 * 0 - 15 is the 4 sprites for the character
 * 16 - 19 for the count
 */
void update_Sprites (void) {
	//Menu drawing
	//Option Arrow
	SPRITES[0] = 0x30 + currentMenuOption*0x10;
	//Player arrow
	// 4-7
	SPRITES[7] = (numPlayers == 1) ? 0x70 : 0x88;
	//Controller Button
	// 8 -31
	for(index = 0; index < sizeof(DOWN_TEXT); ++index){
		SPRITES[9 + (4*index)] = (DOWN_TEXT[index] - 'a') + 10;
	}

	//TODO no clue why index is going one too high
	for(index-- ; index < 6; ++index){
		SPRITES[9 + (4*index)] = 0x24;
	}

	//Seconds
	// 32-39
	SPRITES[33] = secondsChosen / 10;
	SPRITES[37] = secondsChosen % 10;

}

void menu_logic (void) {
	//Menu
	if(((joypad1 & UP) != 0) && ((joypad1old & UP) == 0)) {
		currentMenuOption = (currentMenuOption + 3) % 4; //really -1 mod 4 but no negatives
	}
	else if(((joypad1 & DOWN) != 0) && ((joypad1old & DOWN) == 0)) {
		currentMenuOption = (currentMenuOption + 1) % 4;
	}

	if(((joypad1 & LEFT) != 0) && ((joypad1old & LEFT) == 0)) {
		if(currentMenuOption == 0) {
			numPlayers = (numPlayers == 1) ? 2 : 1; //TODO prolly an xor for this
		}
		else if(currentMenuOption == 1) {
			buttonChosen = buttonChosen >> 1;
		}
		else if(currentMenuOption == 2) {
			secondsChosen--;

			//TODO this and below... should probably go back to the set list of numbers
			if(secondsChosen < 1) {
				secondsChosen = 99;
			}
		}
	}

	if(((joypad1 & RIGHT) != 0) && ((joypad1old & RIGHT) == 0)) {
		if(currentMenuOption == 0) {
			numPlayers = (numPlayers == 1) ? 2 : 1; //TODO prolly an xor for this
		}
		else if(currentMenuOption == 1) {
			buttonChosen = buttonChosen << 1;
		}
		else if(currentMenuOption == 2) {
			secondsChosen++;

			if(secondsChosen > 99) {
				secondsChosen = 1;
			}
		}
	}

	if(((joypad1 & START) != 0) && ((joypad1old & START) == 0)) {
		if(currentMenuOption == 3) {
			hide_sprites();
			gameState = 2;
		}
	}

	//Gameplay
	if ((joypad1 & (buttonChosen)) != 0){ // no L or R
		if((joypad1old & (buttonChosen)) == 0) {
			count = (count + 1) % 100;
		}
	}
}

void Draw_Background(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(TitleScreen);	// uncompresses our data

	PPU_ADDRESS = 0x24; // address of nametable #1 = 0x2400
	PPU_ADDRESS = 0x00;
	UnRLE(MenuScreen);	// uncompresses our data
}

void Draw_Gamescreen(void) {
	All_Off();

	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(GameScreen);	// uncompresses our data

	Wait_Vblank();		// don't turn on screen until in v-blank
	All_On();
}
