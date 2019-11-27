
// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG


#pragma config FOSC = RC        // Oscillator Selection bits (RC oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)   


// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 4 000 000


void PICinit(){
  TRISA  = 0b00000000;
  TRISB  = 0b00000000;
  TRISC  = 0b00000000;
  TRISD  = 0b00000000;
  PORTA  = 0b00000000;   
  PORTB  = 0b00000000;         
  PORTC  = 0b00000000;        
  PORTD  = 0b00000000;            
  
  //ADCON0 |= 0x01;
  //ADCON1 |= 0x06;
  
  OPTION_REG |= 0b10000000 ; // デジタルI/Oに内部プルアップ抵抗を使用する
}

int main(void){
  PICinit();      //PICを初期化
  while(1){
    RD1 = 1;            //RA1のピンをHIGH
    __delay_ms(1000);    //200ms遅延
    RD1 = 0;
    __delay_ms(1000);
  }


  return 0;
}