/*
 * 
 * [PIC16F87X - Microchip Technology](http://ww1.microchip.com/downloads/jp/devicedoc/30292a_jp.pdf)
 * [PIC16F877 Timer Modules tutorials - Timer0](http://www.microcontrollerboard.com/pic-timer0-tutorial.html)
 * [タイマの使い方](http://www.picfun.com/f1/f03.html)
 * 
 * with 16F87X
 * 
 */


// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
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


/*
 * PSA      プリスケーラをタイマ0に割り当て
 * PS2:0    プリスケーラレート
 * T0IE     タイマ0割り込み許可設定
 * T0IF     タイマ0オーバーフロー割り込みフラグ
 * T0CS     クロックソースの設定
 * TMR0     タイマ0カウンタ
 * 
 */

void interrupt InterReceiver(void)
{
    static int tmr0_counter = 0;
    
    if ( 0x01 & INTCON>>2 ) {           // タイマ0割り込み
        
        ++tmr0_counter;
        
        /*
         * Count = f_clk / ( Prescaler * ( 256 - TMR0  * f_out) )
         * カウント数 = 
         * 
         * 　　　　　　　　　　　　　　クロック周波数(CLK)
         * ――――――――――――――――――――――――――――――――――――― 
         * CLK mod10 * プリスケーラレート * ( 256 - 任意の設定値 ) * 割り込み周波数
         * 
         * 1250 = 20000000 / ( 2 * 16 * 250 * 2 )
         * 
         */
        if ( 500 <= tmr0_counter ) {           
            RD1 = !RD1;
            tmr0_counter = 0;
        }
        
        TMR0 = 6; 
        INTCON &= 0xFB;
        
    }
}


void PICinit(){
  TRISA  = 0b00000000;
  TRISB  = 0b00000000;
  TRISC  = 0b00000000;
  TRISD  = 0b00000000;
  PORTA  = 0b00000000;   
  PORTB  = 0b00000000;         
  PORTC  = 0b00000000;        
  PORTD  = 0b00000000;            
  
  ADCON0 |= 0x01;
  ADCON1 |= 0x06;
  
/*
 * ~RBPU = 1;           //PORTB プルアップを使用しない
 * PSA = 0;         // プリスケーラはタイマ 0 モジュールに割り当て
 * T0CS = 0;            // タイマ0のクロックソースを内部命令サイクルクロックに設定
 * // プリスケーラレート - 1:16
 * PS2 = 0;
 * PS1 = 1;
 * PS0 = 1;
 */
  OPTION_REG = 0x42 ;

/*
 * GIE  = 1;               // グローバル割込み処理を許可
 * T0IE = 1;            // TMR0タイマオーバーフロー割り込み許可
 * T0IF = 0;            // TMR0割り込みフラグ初期化
 */
  INTCON |= 0xA0;
  INTCON &= 0xFB;
  
  TMR0 = 6;            // TMR0カウンタ初期化
}

int main(void){
    
  PICinit();      //PICを初期化
    
  while(1);


  return 0;
}