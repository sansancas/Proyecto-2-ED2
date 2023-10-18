#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include <SPI.h>
#include <SD.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"


#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
#define SD_CS PA_3
#define SD_MOSI PA_5
#define SD_MISO PA_4
#define SD_SCK PA_1
#define note_g 391
#define note_gs 415
#define note_a 440

int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};
int state = 0;
int start = 0;
int btn = 1;
int btn2 = 1;
int temp1 = 0;
int temp3 = 0;
int t1 = 0;
int t2 = 0;
int coordx = 144;
int coordy = 200;
int astx = 80;
int asty = 30;
int ast2x = 210;
int ast2y = 30;
int ast3x = 145;
int ast3y = 30;
int dir = 0;
int dir2 = 1;
int jugador = 0;
int buzzerPin = PC_7;
int muerte = 0;
int vel = 1;

String nums[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
String tmr1, tmr2;

File myFile;
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  SPI.setModule(0);
  
  pinMode(PF_4, INPUT_PULLUP);
  pinMode(PF_0, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  SD.begin(SD_CS);
  
  Serial.println("Inicio");
  LCD_Init();
}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  switch(state){
    case 0:
      menu();
      for(int x = 0; x <320-28; x++){
          btn = digitalRead(PF_4);
          delay(5);
          if (start == 1 && btn == 1 && state == 0) {    //Verificar boton ha sido presionado
            x = 0;
            state = 1;    //Pasa a siguiente estado
            //start = 0;
            x = 320;
            beep(note_g, 500);
          }
          int anim = (x/20)%2;
          if (start == 0){
            //LCD_Sprite(x, 20, 32, 42, bowser, 2, anim, 0, 0);
            //V_line( x -1, 20, 38, 0x0000);
            if((anim % 2) == 1){
              String text3 = "PRESS START";
              LCD_Print(text3, 50, 210, 2, 0x0000, 0x0000);
              }
            else{
              String text3 = "PRESS START";
              LCD_Print(text3, 50, 210, 2, 0xffff, 0x0000);
              }
          }
          if (btn == 0 && start == 0){
            start = 1;
        }
        }
        break;
     case 1: 
       LCD_Clear(0x00);
       if(jugador == 0){
         LCD_Print("JUGADOR 1", 100, 100, 2, 0xffff, 0x0000);
         delay(1000);
       }
       else{
         LCD_Print("JUGADOR 2", 100, 100, 2, 0xffff, 0x0000);
         delay(1000);
       }
       LCD_Clear(0x00);
       LCD_Print("START!", 120, 100, 2, 0xffff, 0x0000);
       delay(1000);
       LCD_Clear(0x00);
       //FillRect(0, 0, 319, 220, 0x0f00);
       mapeo_SD("fondo.txt",320, 240, 0, 0);
       FillRect(70, 0, 175, 240, 0x0000);
       LCD_Print("JUGADOR 1", 0, 5, 1, 0xffff, 0x0000);
       if(jugador == 0){
       LCD_Sprite(144, 200, 28, 38, enemy, 4, 0, 0, 0);
       }
       else{
       LCD_Sprite(144, 200, 28, 38, enemy, 4, 0, 0, 0);
       }
       tmr1 = nums[(t1/100)] + nums[(t1/10)%10] + nums[t1%10];
       LCD_Print(tmr1, 20, 20, 1, 0xffff, 0x0000);
       tmr2 = nums[(t2/100)] + nums[(t2/10)%10] + nums[t2%10];
       LCD_Print(tmr2, 270, 20, 1, 0xffff, 0x0000);
       LCD_Print("JUGADOR 2", 247, 5, 1, 0xffff, 0x0000);
       temp1 = millis()/1000;
       state++;
       break;
    case 2:
      actualiar_contador(jugador); 
      for(int x = 0; x < 320; x++){
        btn = digitalRead(PF_4);
        btn2 = digitalRead(PF_0);
        actualiar_contador(jugador);  
        mover_ciclista((x/28)%2);
        
        if((t1 > 10 && jugador == 0) || (t2 > 10 && jugador == 1)){
              mover_ciclista2((x/28)%2);
            }
        if((t1 > 20 && jugador == 0) || (t2 > 20 && jugador == 1)){
              mover_ciclista3((x/28)%2);
            }
        int anim = (x/2)%4;
        //if(jugador == 0){
         LCD_Sprite(coordx, coordy, 28, 38, enemy, 4, anim, 0, 0);
        // }
        // else{
         //LCD_Sprite(coordx, coordy, 28, 38, enemy, 4, anim, 0, 0);
         //}
        int cont = anim;
        if(state != 3){
        while(btn == 0){
          btn = digitalRead(PF_4);
          if(coordx>70){
            coordx = coordx - vel;
            LCD_Sprite(coordx, coordy, 28, 38, enemy, 4, (cont/2)%4, 1, 0);
            V_line(coordx + 29 + vel, coordy, 38, 0x00);
            V_line(coordx, coordy, 38, 0x00);
            V_line(coordx - vel, coordy, 38, 0x00);
          }
          else{
            LCD_Sprite(coordx, coordy, 28, 38, enemy, 4, (cont/2)%4, 0, 0);
            }
            actualiar_contador(jugador);
            mover_ciclista((x/28)%2);
            
            if((t1 > 10 && jugador == 0) || (t2 > 10 && jugador == 1)){
              mover_ciclista2((x/28)%2);
            }
            if((t1 > 20 && jugador == 0) || (t2 > 20 && jugador == 1)){
              mover_ciclista3((x/28)%2);
            }
            cont++;
        }  
        while(btn2 == 0){
          btn2 = digitalRead(PF_0);
          if(coordx<245-36){
            coordx = coordx + vel;
            LCD_Sprite(coordx, coordy, 28, 38, enemy, 4, (cont/2)%4, 0, 0);
            V_line(coordx - vel, coordy, 38, 0x00);
            V_line(coordx - vel - 1, coordy, 38, 0x00);
          }
          else{
            LCD_Sprite(coordx, coordy, 28, 38, enemy, 4, (cont/2)%4, 0, 0);
            }
            actualiar_contador(jugador);
            mover_ciclista((x/28)%2);
            
            if((t1 > 10 && jugador == 0) || (t2 > 10 && jugador == 1)){
              mover_ciclista2((x/28)%2);
            }
            if((t1 > 20 && jugador == 0) || (t2 > 20 && jugador == 1)){
              mover_ciclista3((x/28)%2);
            }
            cont++;
        }
        }
        else{
          x = 320;
          beep(note_gs, 500);
          }  
        }
      
    break;
    case 3:
      LCD_Clear(0x00);
      switch(muerte){
      case 0:
      mapeo_SD("mortis.txt",320, 240, 0, 0);
      break;
      case 1:
      mapeo_SD("perro.txt",320, 240, 0, 0);
      break;
      case 2:
      mapeo_SD("pop.txt",320, 240, 0, 0);
      break;  
        }
      LCD_Print("GAME OVER", 100, 100, 2, 0xffff, 0x0000);
      //LCD_Print(nums[muerte], 100, 100, 2, 0xffff, 0x0000);
      delay(1000);
      LCD_Clear(0x00);
      jugador++;
      if(jugador == 2){
        if(t1 > t2){
          LCD_Print("JUGADOR 1", 100, 100, 2, 0xffff, 0x0000);
          LCD_Print("GANA", 100, 120, 2, 0xffff, 0x0000);
          delay(1000);
          LCD_Clear(0x00);
          }
         if(t1 < t2){
          LCD_Print("JUGADOR 2", 100, 100, 2, 0xffff, 0x0000);
          LCD_Print("GANA", 100, 120, 2, 0xffff, 0x0000);
          delay(1000);
          LCD_Clear(0x00);
          } 
          if(t1 == t2){
            LCD_Print("EMPATE", 100, 100, 2, 0xffff, 0x0000);
            delay(1000);
            LCD_Clear(0x00);
            }
         while(1){   
         LCD_Print("PRESIONA START", 50, 100, 2, 0xffff, 0x0000);
         delay(1000);
         btn = digitalRead(PF_4);
         if(btn == 0){
          state = 0;
          coordx = 144;
          coordy = 200;
          astx = 80;
          asty = 30;
          ast2x = 210;
          ast2y = 30;
          ast3x = 145;
          ast3y = 30;
          t1 = 0;
          t2 = 0;
          jugador = 0;
          dir = 0;
          dir2 = 1;
          start = 0;
          muerte = 0;
          vel = 1;
          LCD_Clear(0x00);
          
          break;
         }
         }
        }
      else{  
        coordx = 144;
        coordy = 200;
        astx = 80;
        asty = 30;
        ast2x = 210;
        ast2y = 30;
        ast3x = 145;
        ast3y = 30;
        dir = 20;
        dir2 = 1;
        state = 1;
        muerte = 0;
        vel = 1;
      }
    break;
    }
  }

void menu(void){
  LCD_Clear(0x00);
  
  mapeo_SD("inicio.txt",320, 240, 0, 0);
  
  //LCD_Print("PASOS", 10, 70, 2, 0xffff, 0x0000);
  
  //LCD_Print("Y", 10, 100, 2, 0xffff, 0x0000);

  //LCD_Print("PEDALES", 10, 140, 2, 0xffff, 0x0000);
  //LCD_Print("JUEGO", 10, 140, 2, 0xffff, 0x0000);

  LCD_Print("PRESS START", 50, 210, 2, 0xffff, 0x0000);
  delay(1000);
}
  
void actualiar_contador(int cual){
  int temp2 = (millis()/1000) - temp1;
  if(temp2%20 == 0 && temp2/20 != 0 && ((millis()/1000)-temp3>=1)){
        vel++;
        temp3 = millis()/1000;
        } 
  if(cual == 0){
    tmr1 = nums[(temp2/100)] + nums[(temp2/10)%10] + nums[temp2%10];
    LCD_Print(tmr1, 20, 20, 1, 0xffff, 0x0000);
    t1 = temp2;
  }
  else{
    tmr2 = nums[(temp2/100)] + nums[(temp2/10)%10] + nums[temp2%10];
    LCD_Print(tmr2, 270, 20, 1, 0xffff, 0x0000);
    t2 = temp2;
  }
  }
  

void mover_ciclista(int anim) {
  if (((astx + 28) > coordx) && ((astx + 28) < coordx + 59)) {
    if (((asty + 28) > coordy) && ((asty + 28) < coordy + 59)) {
      state = 3;
      muerte = 0;
    }
  }
  if(state != 3){
  if (asty >= 240) {
    H_line(0, 30, 320, 0x0000);
    astx = random(80, 235-32);
    asty = 30;
  }
  
  LCD_Sprite(astx, asty, 32, 42, bowser, 2, anim, 0, 0);
   H_line(astx, asty, 28, 0x0000);
  H_line(astx, asty+vel, 28, 0x0000);
  V_line(astx, asty, 28, 0x0000);
  V_line(astx+32, asty, 28, 0x0000);
  asty = asty + vel;
}
}

void mover_ciclista2(int anim) {
  if (((ast2x + 28) > coordx) && ((ast2x + 28) < coordx + 59)) {
    if (((ast2y + 28) > coordy) && ((ast2y + 28) < coordy + 59)) {
      muerte = 1;
      state = 3; 
    }
  }
  if(state != 3){
  if (ast2y >= 240) {
    H_line(0, 30, 320, 0x0000);
    ast2x = random(80, 235-32);
    ast2y = 30;
  }
  if(ast2x + 32 >= 245){
      dir2 = 1;
      }
  if(ast2x <= 70){
      dir2 = 0;
    }  

  if (((ast2x + 32) > astx) && ((ast2x + 32) < astx + 59)) {
    if (((ast2y + 36) > asty) && ((ast2y + 36) < asty + 59)) {
      int current = 0;
      if(current != dir){
        dir != 1;
        }
    }
  }  
  LCD_Sprite(ast2x, ast2y, 30, 38, prueba, 2, anim, 0, 0);
  H_line(ast2x, ast2y, 32, 0x0000);
  V_line(ast2x, ast2y, 43, 0x0000);
  V_line(ast2x+32, ast2y, 43, 0x0000);
  switch(dir2){
    case 0:
      ast2x++;
    break;
    case 1:
      ast2x--;
    break;
    }
  ast2y = ast2y + vel;
}
}

void mover_ciclista3(int anim) {
  if (((ast3x + 28) > coordx) && ((ast3x + 28) < coordx + 59)) {
    if (((ast3y + 28) > coordy) && ((ast3y + 28) < coordy + 59)) {
      muerte = 2;
      state = 3;
      
    }
  }
  if(state != 3){
  if (ast3y >= 240) {
    H_line(0, 30, 320, 0x0000);
    ast3x = random(80, 235-32);
    ast3y = 30;
  }
  
  LCD_Sprite(ast3x, ast3y, 26, 25, mario, 1, 0, 0, 0);
  H_line(ast3x, ast3y, 28, 0x0000);
  H_line(ast3x, ast3y+vel, 28, 0x0000);
  V_line(ast3x, ast3y, 28, 0x0000);
  V_line(ast3x+32, ast3y, 28, 0x0000);
  ast3y = ast3y + vel;
}
}

void beep(int note, int duration)
{
  tone(buzzerPin, note, duration / 2);
  delay(duration / 2);
  noTone(buzzerPin);
  delay(duration / 2 + 20);
}

void mapeo_SD(char document[], int width, int height, int x0, int y0){
  //digitalWrite(SD_CS, LOW);
  myFile = SD.open(document);
  int yi = y0;
  int hex1 = 0;
  int val1 = 0;
  int val2 = 0;
  int mapear = 0;
  //int vertical = 0;
  width = width*2;
  unsigned char maps[width];
  if(myFile){
    while((myFile.available()) && (y0 < (height + yi))){
      mapear = 0;
      while(mapear<width){
        hex1 = myFile.read();
        if(hex1 == 120){
          val1 = myFile.read();
          val2 = myFile.read();
          val1 = asciitohex(val1);
          val2 = asciitohex(val2);
          maps[mapear] = val1*16 + val2;
          mapear++;
          }
        }
        LCD_Bitmap(x0, y0, (width/2), 1, maps);
        y0++;
      }
      myFile.close();
    }
  else{
    //Serial.println("No se pudo abrir la imagen");
    myFile.close();
    }
  }

int asciitohex(int val){
  switch(val){
    case(48):
      return 0;
    case(49):
      return 1;
    case(50):
      return 2;
    case(51):
      return 3;
    case(52):
      return 4;
    case(53):
      return 5;
    case(54):
      return 6;
    case(55):
      return 7;
    case(56):
      return 8;
    case(57):
      return 9;
    case(97):
      return 0xa;
    case(98):
      return 0xb;
    case(99):
      return 0xc;
    case(100):
      return 0xd;
    case(101):
      return 0xe;
    case(102):
      return 0xf;
    }
  }
//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}

//***************************************************************************************************************************************
// Función para enviar parámetros y comandos a la LCD
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}

//***************************************************************************************************************************************
// Función para enviar datos a la LCD
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}

//***************************************************************************************************************************************
// Función para definir el rango en las que se trabajará
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}

//***************************************************************************************************************************************
// Función para borrar la pantalla
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}

//***************************************************************************************************************************************
// Función para dibujar una línea horizontal
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}

//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}

//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}

//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y + i, w, c);
  }
}

//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}

//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}

//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}
