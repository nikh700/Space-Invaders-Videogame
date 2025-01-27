// SpaceInvaders.c
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the ECE319K Lab 10

// Last Modified: 1/2/2023
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php

// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "../inc/ADC.h"
#include "Images.h"
#include "../inc/wave.h"
#include "Timer1.h"
#include "Timer0.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
int32_t Function=0;
static uint8_t language_chooser;
int32_t ADCMail, local_x, local_y, i, k, h, Button_Press_Checker = 0, Score = 0, Screen_Clear = 0;
int32_t i_saver[20], deaths, bottom, dy;
extern void Timer3A_Init(uint32_t period, uint32_t priority);
extern uint16_t index;
void Check_Collision(void);

typedef enum {Easy, Medium, Hard} Difficulty_t;
Difficulty_t Mode;

typedef enum {alive, halfdead, dead} status;
typedef struct Object{
int32_t x;
int32_t y;
int32_t w;
int32_t h;
const unsigned short *image;
status life;
} Object_t;

Object_t Enemies[18] = {
{0,10,16,10, SmallEnemy30pointA, alive},
{20,10,16,10, SmallEnemy30pointA, alive},
{40,10,16,10, SmallEnemy30pointA, alive},
{60,10,16,10, SmallEnemy30pointB, alive},
{80,10,16,10, SmallEnemy30pointB, alive},
{100,10,16,10, SmallEnemy30pointB, alive}, // 5
{0, 25,16,10, SmallEnemy20pointA, alive}, // 6
{20, 25,16,10, SmallEnemy20pointA, alive},// 7
{40, 25,16,10,SmallEnemy20pointA, alive}, // 8
{60, 25,16,10, SmallEnemy20pointB, alive}, // 9
{80, 25,16,10, SmallEnemy20pointB, alive}, // 10
{100, 25, 16,10, SmallEnemy20pointB, alive}, // 11
{0, 40,16,10, SmallEnemy10pointA, alive}, // 12
{20, 40,16,10, SmallEnemy10pointA, alive}, // 13
{40, 40,16,10, SmallEnemy10pointA, alive}, // 14
{60, 40,16,10, SmallEnemy10pointB, alive}, // 15
{80, 40, 16,10, SmallEnemy10pointB, alive}, // 16
{100, 40,16,10, SmallEnemy10pointB, alive} // 17
};

Object_t Enemies_Saver[18] = {
{0,10,16,10, SmallEnemy30pointA, alive},
{20,10,16,10, SmallEnemy30pointA, alive},
{40,10,16,10, SmallEnemy30pointA, alive},
{60,10,16,10, SmallEnemy30pointB, alive},
{80,10,16,10, SmallEnemy30pointB, alive},
{100,10,16,10, SmallEnemy30pointB, alive}, // 5
{0, 25,16,10, SmallEnemy20pointA, alive}, // 6
{20, 25,16,10, SmallEnemy20pointA, alive},// 7
{40, 25,16,10,SmallEnemy20pointA, alive}, // 8
{60, 25,16,10, SmallEnemy20pointB, alive}, // 9
{80, 25,16,10, SmallEnemy20pointB, alive}, // 10
{100, 25, 16,10, SmallEnemy20pointB, alive}, // 11
{0, 40,16,10, SmallEnemy10pointA, alive}, // 12
{20, 40,16,10, SmallEnemy10pointA, alive}, // 13
{40, 40,16,10, SmallEnemy10pointA, alive}, // 14
{60, 40,16,10, SmallEnemy10pointB, alive}, // 15
{80, 40, 16,10, SmallEnemy10pointB, alive}, // 16
{100, 40,16,10, SmallEnemy10pointB, alive} // 17
};

Object_t Player = {80, 159, 15, 10, ufo_meme, alive};

Object_t Bullet[30] = {
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead},
{200,150,10,10, bullet, dead}
};

Object_t Bullet_Saver[1] = {
{200,150,10,10, bullet, dead},
};

void Timer1A_Handler(void){ // can be used to perform tasks in background
if (Mode == Easy) {
	dy = 1;
} else if (Mode == Medium) {
	dy = 2;
} else if (Mode == Hard) {
	dy = 3;
}
	
for (h = 0; h < 18; h++) {
Enemies[h].y += dy;
}
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
   // execute user task
}

int32_t Convert_Function(int32_t input) {
return ((input+300)*128)/4096 ;
}

void Timer0A_Handler(void) {
ADCMail = ADC_In();
TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
}

void SysTick_Handler(void) {
Check_Collision();
for (int32_t j = 0; j < 30; j++) {
if (Bullet[j].life == alive) {
// ST7735_DrawBitmap(Bullet[j].x, Bullet[j].y, Black_Reset_Screen_Vertical, 10, 150);
Bullet[j].y -= 2;
}
}
}

uint16_t past = 0, now = 0, pressed = 0;
void Timer3A_Handler(void) {
//Check_Collision();
now = 0;
if(GPIO_PORTE_DATA_R == 0x08)
now = 1;
if(past == now)//0,0 or 1,1
{}
else if(now) //0,1
{
pressed ++;
}
else //1,0
{}
past = now;
TIMER3_ICR_R = TIMER_ICR_TATOCINT;
}


// You can't use this timer, it is here for starter code only
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
      time--;
    }
    count--;
  }
}

void DelayMs(uint32_t n){
  volatile uint32_t time;
  while(n){
    time = 6665;  // 1msec, tuned at 80 MHz
    while(time){
      time--;
    }
    n--;
  }
}

void Buttons_Init(void) {
SYSCTL_RCGCGPIO_R |= 0x10;
volatile int32_t Delay;
Delay = 1;
GPIO_PORTE_DIR_R &= ~0x0F;
GPIO_PORTE_DEN_R |= 0x0F;
}

void Language_Select_Page(void) {
ST7735_FillScreen(0x0000);
ST7735_SetCursor(0,0);
ST7735_OutString("Hello!\n");
ST7735_OutString("\xADHola!\n\n\n");
ST7735_OutString("Choose your language\n");
ST7735_OutString("Elige tu idioma\n\n\n");
ST7735_OutString("Up Button: English\n");
ST7735_OutString("Down Button: Spanish\n\n\n");
ST7735_OutString("Bot\xA2n Arriba: Ingl\x82s\n");
ST7735_OutString("Bot\xA2n Abajo: Espa\xA4ol");
}

void Opening_Page_English(void) {
ST7735_FillScreen(0x0000);
ST7735_SetCursor(0,0);
ST7735_OutString("Welcome to:\n\n\n\n\n\n\n");
Delay100ms(10);
ST7735_OutString("  SPACE INVADERS!!!\n\n\n\n\n");
for (i = 0; i < 10; i++) {
GPIO_PORTD_DATA_R = 0x03;
Delay100ms(1);
GPIO_PORTD_DATA_R = 0x00;
Delay100ms(1);
}
ST7735_OutString("  Click any button\n  to continue");
}

void Opening_Page_Spanish(void) {
ST7735_FillScreen(0x0000);
ST7735_SetCursor(0,0);
ST7735_OutString("Bienvenido a:\n\n\n\n\n\n\n");
Delay100ms(10);
ST7735_OutString("  SPACE INVADERS!!!\n\n\n\n\n");
for (i = 0; i < 10; i++) {
GPIO_PORTD_DATA_R = 0x03;
Delay100ms(1);
GPIO_PORTD_DATA_R = 0x00;
Delay100ms(1);
}
ST7735_OutString("  Haz clic en\n  cualquier bot\xA2n\n  para continuar");
}

void Instructions_English(void) {
ST7735_FillScreen(0x0000);
ST7735_SetCursor(0,0);
ST7735_OutString("Up: Shoot\n");
ST7735_OutString("Down: Pause\n");
ST7735_OutString("Right: Play\n\n\n");
ST7735_OutString("Don't let the\ninvaders get to the\nbottom!\n\n\n");
ST7735_OutString("  Click any button\n  to continue");
}

void Instructions_Spanish(void) {
ST7735_FillScreen(0x0000);
ST7735_SetCursor(0,0);
ST7735_OutString("Arriba: Disparar\n");
ST7735_OutString("Abajo: Parar\n");
ST7735_OutString("Derecho: Jugar\n\n\n");
ST7735_OutString("\xADNo dejes que los\ninvasores lleguen al\nfondo!\n\n\n");
ST7735_OutString("  Haz clic en\n  cualquier bot\xA2n\n  para continuar");
}

void Mode_Choice_Page_English(void) {
ST7735_FillScreen(0x0000);
ST7735_SetCursor(0,0);
ST7735_OutString("Mode Choice:\n");
ST7735_OutString("Left: Easy\n");
ST7735_OutString("Up: Medium\n");
ST7735_OutString("Right: Hard\n");
}

void Mode_Choice_Page_Spanish(void) {
ST7735_FillScreen(0x0000);
ST7735_SetCursor(0,0);
ST7735_OutString("Opci\xA2n de Modo:\n");
ST7735_OutString("Izquierda: F\xA0 cil\n");
ST7735_OutString("Arriba: Duro\n");
ST7735_OutString("Derecho: Medio\n");
}



int flag;
void Game_Over(void) {
ST7735_FillScreen(0x0000);
ST7735_SetCursor(0,0);
if(Score < 360) {
	flag=0;
GPIO_PORTD_DATA_R = 0x02;
} else if(Score == 360) {
flag = 1;
GPIO_PORTD_DATA_R = 0x01;
}

if(flag==0){
if (language_chooser == 0) {
ST7735_OutString("GAME OVER\n");
ST7735_OutString("Score:\n");
ST7735_OutUDec(Score);
ST7735_OutString("\n");
ST7735_OutString("You died :\x28\nGood luck next time!\n\n\n");
ST7735_OutString("  Press a button\n  to play again");
} else if (language_chooser == 1) {
ST7735_OutString("FIN DEL JUEGO\n");
ST7735_OutString("El Marcador:\n");
ST7735_OutUDec(Score);
ST7735_OutString("\n");
ST7735_OutString("Moriste :\x28\n\xAD Buena suerte la\npr\xA2xima vez!\n\n\n");
ST7735_OutString("  Pulsa un bot\xA2n\n  para jugar de\n  nuevo");

}
}
if(flag==1){
if (language_chooser == 0) {
ST7735_OutString("GAME OVER\n");
ST7735_OutString("Score:\n");
ST7735_OutUDec(Score);
ST7735_OutString("\n");
ST7735_OutString("Congratulations! :\x29\nSee you next time!\n\n\n");
ST7735_OutString("  Press a button\n  to play again");
} else if (language_chooser == 1) {
ST7735_OutString("FIN DEL JUEGO\n");
ST7735_OutString("El Marcador:\n");
ST7735_OutUDec(Score);
ST7735_OutString("\n");
ST7735_OutString("\xAD Felicidades! :\x29\n\xADHasta la pr\xA2xima!\n\n\n");
ST7735_OutString("  Pulsa un bot\xA2n\n  para jugar de\n  nuevo");  
}
}
Score = 0;

}

void SysTick_Init(void) {
NVIC_ST_CTRL_R = 0;
NVIC_ST_CURRENT_R = 0;
}

void SysTick_Start(int32_t period) {
NVIC_ST_RELOAD_R = period;
NVIC_ST_CTRL_R |= 0x07;
}

void SysTick_Stop(void) {
NVIC_ST_RELOAD_R = 0;
}

void Check_Collision(void) {
for (int32_t a = 0; a < 30; a++) {
for (int32_t b = 0; b < 18; b++) {
if ((Enemies[b].life == alive) && (Bullet[a].life == alive) && (Bullet[a].y <= Enemies[b].y + Enemies[b].h) && (Bullet[a].x + Bullet[a].w >= Enemies[b].x) &&
 (Bullet[a].x <= Enemies[b].x + Enemies[b].w)
   ) {

Bullet[a].life = dead;
Bullet[a].x = 0;
Bullet[a].y = 0;
Enemies[b].life = dead;
Enemies[b].x = 127-Enemies[b].w;
Enemies[b].y = 0;
if (index == 0) {
Wave_Killed();
}
}
}
}
}

int32_t Check_Deaths(void) {
int32_t Death_Counter = 0;
for (int32_t a = 0; a < 18; a++) {
if (Enemies[a].life == dead) {
Death_Counter += 1;
}
}
return Death_Counter;
}

int32_t Check_Bottom(void) {
for (int32_t a = 0; a < 18; a++) {
if (Enemies[a].y >= 150) {
return 1;
}
}
return 0;
}

void Reset_Function(void) {
Score = 0;

for (int32_t b = 0; b < 18; b++) {
if (Enemies[b].life == dead) {
if (b >= 12) {
Score += 10;
} else if (b >= 6) {
Score += 20;
} else {
Score += 30;
}
}
}

Enemies[0] = Enemies_Saver[0];
Enemies[1] = Enemies_Saver[1];
Enemies[2] = Enemies_Saver[2];
Enemies[3] = Enemies_Saver[3];
Enemies[4] = Enemies_Saver[4];
Enemies[5] = Enemies_Saver[5];
Enemies[6] = Enemies_Saver[6];
Enemies[7] = Enemies_Saver[7];
Enemies[8] = Enemies_Saver[8];
Enemies[9] = Enemies_Saver[9];
Enemies[10] = Enemies_Saver[10];
Enemies[11] = Enemies_Saver[11];
Enemies[12] = Enemies_Saver[12];
Enemies[13] = Enemies_Saver[13];
Enemies[14] = Enemies_Saver[14];
Enemies[15] = Enemies_Saver[15];
Enemies[16] = Enemies_Saver[16];
Enemies[17] = Enemies_Saver[17];

for (i = 0; i < 30; i++) {
Bullet[i] = Bullet_Saver[0];
}

};
/////ohkyo's code
void PortD_Init(void){
volatile int delay;
SYSCTL_RCGCGPIO_R |= 0x08;
delay = 0;
GPIO_PORTD_DIR_R |= 0x03;
GPIO_PORTD_DEN_R |= 0x03;
}
void Pause_Menu(void) {
ST7735_FillScreen(0x0000);
ST7735_SetCursor(0,0);
if (language_chooser == 0) {
ST7735_OutString("GAME PAUSED\n\n\n\n\n\n\n");
ST7735_OutString("  Press right\n  to continue");
} else if (language_chooser == 1) {
ST7735_OutString("JUEGO PASADO\n\n\n\n\n\n\n");
ST7735_OutString("  Presione hacia\n  derecho para\n  continuar");
}
}

int main(void) {
// Initializations for LCD, ADC, Clock, Buttons, Timer0, Timer1, SysTick, DAC, and Wave. Interrupts Disabled
DisableInterrupts();
TExaS_Init(NONE);
Timer0_Init(80000000/10, 1);
Timer1_Init(80000000/10, 2);
SysTick_Init();
Wave_Init();
PortD_Init();
//Random_Init(1);
Buttons_Init();
ADC_Init();
ST7735_InitR(INITR_REDTAB);
ST7735_FillScreen(0x0000);

// Language Select Page
Language_Select_Page();

// Moving to correct language Space Invaders Page
while(1) {
if(GPIO_PORTE_DATA_R == 0x08) {
Opening_Page_English();
language_chooser = 0;
break;
}
if(GPIO_PORTE_DATA_R == 0x04) {
Opening_Page_Spanish();
language_chooser = 1;
break;
}
}

// Showing Instructions on User Input
while(1) {
if ((language_chooser == 0) && (GPIO_PORTE_DATA_R != 0x00)) {
Instructions_English();
break;
}
if ((language_chooser == 1) && (GPIO_PORTE_DATA_R != 0x00)) {
Instructions_Spanish();
break;
}
}
//Timer3A_Init(8000000, 1);
GPIO_PORTE_DATA_R &= ~0x08;
// Starting game when user presses a button
while(1) {
if (GPIO_PORTE_DATA_R != 0x00) {
break;
}
}


// Showing mode choice
while (1) {
if (language_chooser == 0) {
Mode_Choice_Page_English();
} else if (language_chooser == 1) {
Mode_Choice_Page_Spanish();
}
while(1) {
if (GPIO_PORTE_DATA_R == 0x02) {
	Mode = Easy;
	break;
} else if (GPIO_PORTE_DATA_R == 0x08) {
	Mode = Medium;
	break;
} else if (GPIO_PORTE_DATA_R == 0x01) {
	Mode = Hard;
	break;
}
}
break;
}


Timer3A_Init(8000000, 1);
ST7735_FillScreen(0x0000); // Clears Screen
EnableInterrupts(); // Enables Interrupts
SysTick_Start(80000000/12);

// Game Code
while(1) {
while(1) {

Screen_Clear += 1;

if (Screen_Clear % 20 == 0) {
ST7735_FillScreen(0x0000);
}


// Displaying Player Ship Based on ADC Input
ST7735_DrawBitmap(0, 160, Black_Reset_Screen_Horizontal, 128, 10);
local_x = 128 - Convert_Function(ADCMail);
ST7735_DrawBitmap(local_x, Player.y, Player.image, Player.w, Player.h);

// Displaying Enemy Ships if they are alive
for (k = 0; k < 18; k++) {
if (Enemies[k].life == alive) {
ST7735_DrawBitmap(Enemies[k].x, Enemies[k].y, Enemies[k].image, Enemies[k].w, Enemies[k].h);
}
if (Enemies[k].life == dead){
ST7735_DrawBitmap(Enemies[k].x, Enemies[k].y, Black_Reset_Screen_Enemy, Enemies[k].w, Enemies[k].h);
}
}

// Making a bullet alive and saving location if button is pressed
if (pressed>0) {
Button_Press_Checker++;
pressed--;
Button_Press_Checker %= 30;
Bullet[Button_Press_Checker].x = 2 + local_x;
Bullet[Button_Press_Checker].life = alive;
if (index == 0) {
Wave_Shoot();
}
}

// Displaying bullets if alive and killing them if they reach end of screen
for (i = 0; i < 30; i++) {
if (Bullet[i].life == alive) {
ST7735_DrawBitmap(Bullet[i].x, Bullet[i].y, Bullet[i].image, Bullet[i].w, Bullet[i].h);
}
/*
if (Bullet[i].life == dead) {
ST7735_DrawBitmap(Bullet[i].x, Bullet[i].y, Black_Reset_Screen_Bullet, Bullet[i].w, Bullet[i].h);
Bullet[i].y = 150;
}
*/
if (Bullet[i].y <= 0) {
Bullet[i].life = dead;
}
}

// Checking for bullet-ship collisions and changing statuses if necessary
// Check_Collision();

// Checking for Win-Lose Conditions and Moving to Game End Menu if necessary
deaths = Check_Deaths();
if (deaths == 18) {
break;
}
bottom = Check_Bottom();
if (bottom == 1) {
break;
}

// Replacing dead bullets and ships with black screens if dead
//for (i = 0; i < 30; i++) {
//}

/*
for (i = 0; i < 18; i++) {
if (Enemies[i].life == dead) {
ST7735_DrawBitmap(Enemies[i].x + 5, Enemies[i].y, Black_Reset_Screen_Ship, Enemies[i].w, Enemies[i].h);
}
}
*/

// Pause Screen
if (GPIO_PORTE_DATA_R == 0x04) {

DisableInterrupts();
Pause_Menu();
while (GPIO_PORTE_DATA_R != 0x01) {}
EnableInterrupts();
continue;
}


}
// Restart Screen
DisableInterrupts();
Reset_Function();
Game_Over();
while (GPIO_PORTE_DATA_R == 0) {}
while (1) {
if (language_chooser == 0) {
Mode_Choice_Page_English();
} else if (language_chooser == 1) {
Mode_Choice_Page_Spanish();
}
while(1) {
if (GPIO_PORTE_DATA_R == 0x02) {
	Mode = Easy;
	break;
} else if (GPIO_PORTE_DATA_R == 0x08) {
	Mode = Medium;
	break;
} else if (GPIO_PORTE_DATA_R == 0x01) {
	Mode = Hard;
	break;
}
}
break;
}
GPIO_PORTD_DATA_R &= ~0x0F;
EnableInterrupts();



/*
ST7735_DrawBitmap(64, 80, Bullet[0].image, Bullet[0].w, Bullet[0].h);
ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom

ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);

ST7735_DrawBitmap(0, 118, Black_Reset_Screen, 128, 10);
*/
}

}
