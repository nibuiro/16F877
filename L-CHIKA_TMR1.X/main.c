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
#include <stdbool.h>

#define _XTAL_FREQ 20000000


/*
 * T1CKPS1:0    プリスケーラレート
 * TMR1IE     タイマ1割り込み許可設定
 * TMR1IF     タイマ1オーバーフロー割り込みフラグ
 * TMR1     タイマ1カウンタ
 * 
 */

/*
 * 0.5[Hz] = ( 20000000 / ( 2 * 8 * 62500 * 10) ) / [FOSC]4
 */


void interrupt ISR(void)
{
    static int tmr1_counter = 0;

    if ( 0x01 & PIR1 ) {           // TMR1IF     タイマ1オーバーフロー割り込みフラグ 
        if ( 10 <= tmr1_counter ) {
            RD1 = !RD1;
            tmr1_counter = 0;
        }
        
        ++tmr1_counter;
            
        TMR1H = 0x0b;
        TMR1L = 0xdb;
        PIR1 &= 0xFE;
        
    } 
}


void PICinit()
{
    TRISA  = 0x00;
    TRISB  = 0x02;
    TRISC  = 0x00;
    TRISD  = 0x00;
    PORTA  = 0x00;   
    PORTB  = 0x02;         
    PORTC  = 0x00;        
    PORTD  = 0x00;            
    
    INTCON = 0xC0;            // 割り込みを使用可能にする
    
   // OPTION_REG &= 0xBF;           // RB0/INTピンの立ち下がりエッジにより割り込み
    
    /*
     * 1:2 プリスケール値
     * オシレータを停止させる
     * 外部クロック入力を同期させる
     * RC0/T1OSO/T1CKI(立ち上がりエッジ）ピンからの外部クロック
     * タイマ1を停止する
     */
    T1CON = 0x31;
    
    PIE1 |= 0x01;         //　TMR1IE     タイマ1割り込み許可設定
    PIR1 = 0x00;
    TMR1H = 0x0b;
    TMR1L = 0xdb;
}


int main(int argc, char** argv)
{
    PICinit();   
    
    RD1 = 0;


    while(1);

    return 0;
}



