/*
 * 
 * Software - UART ver.0.9.0
 * 
 * [Tera Term] Communicate speed 1 - with 16F87X
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
#include <stdbool.h>

#define _XTAL_FREQ 20000000

typedef struct softuart {

    // active state
    bool rx;
    bool tx;

    // data
    unsigned char rxd;
    unsigned char txd;

    // counter
    short short int rxc;
    short short int txc;

} softuart;

softuart softuart0;


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
short short unsigned int TMR1_state = 0;
short short unsigned int INT_state = 1;
short short int interval = 0;

#define __TMR1H 0x5D
#define __TMR1L 0x38

void interrupt ISR(void)
{

    if ( 0x01 & PIR1 ) {           // TMR1IF     タイマ1オーバーフロー割り込みフラグ 
        
        if ( -1 == softuart0.txc) {

            RD1 = 1;
            TMR1_state = 0;
            INT_state = 1;
            softuart0.tx =　false;
            softuart0.txc = 0;

        } else if ( softuart0.tx ) {
            
            RD1 = softuart0.txd>>softuart0.txc;
            --softuart0.txc;
            
        } else if ( softuart0.rx ) {

            softuart0.rxd = softuart0.rxd<<1 | RB1;
            ++softuart0.rxc;
        
            if (8 == softuart0.rxc) {
            
                softuart0.rx =　false;
                // TMR1_state = 0;         // タイマ1　オフ
                // INT_state = 1;
            
                softuart0.txd = softuart0.rxd;
                softuart0.txc = softuart0.rxc - 1;
                softuart0.tx =　true;
                
                interval = 1;
                RD1 = 0;
            
            }
        } else {

            softuart0.rx = true;
            
        }

        TMR1H = __TMR1H;
        TMR1L = __TMR1L;
        PIR1 &= 0xFE;
        
    } else if ( ( 0x01 & INTCON>>1 ) ) {           // 外部割込み
       
        INTCON &= 0xFD;         // 外部割込みフラグクリア
        
        if ( 0 == interval ) {
            
            softuart0.rxc = 0;
            softuart0.rxd = 0x00;
        
            TMR1H = __TMR1H;
            TMR1L = __TMR1L;  

            INT_state = 0;
            TMR1_state = 1;         // タイマ1　オン
            
        } else {
            
            --interval;
            
        }
    }
}


void PICinit()
{
    TRISA  = 0x00;
    TRISB  = 0x03;
    TRISC  = 0x00;
    TRISD  = 0x00;
    PORTA  = 0x00;   
    PORTB  = 0x00;         
    PORTC  = 0x00;        
    PORTD  = 0x02;            
    
    INTCON = 0xD1;            // 割り込みを使用可能にする
    
    OPTION_REG &= 0xBF;           // RB0/INTピンの立ち下がりエッジにより割り込み
    
    /*
     * 1:2 プリスケール値
     * オシレータを停止させる
     * 外部クロック入力を同期させる
     * RC0/T1OSO/T1CKI(立ち上がりエッジ）ピンからの外部クロック
     * タイマ1を停止する
     */
    T1CON = 0x10;
    
    PIE1 |= 0x01;         //　TMR1IE     タイマ1割り込み許可設定
    PIR1 = 0x00;
    TMR1H = __TMR1H;
    TMR1L = __TMR1L;
    
    softuart0.rx = false;
    softuart0.tx = false;
    softuart0.rxd = 0x00;
    softuart0.txd = 0x00;
    softuart0.rxc = 0;
    softuart0.txc = 0;
}


int main(int argc, char** argv)
{
    
    PICinit();   
    

    while(1)
    {        
      T1CON = ( T1CON & 0xFE ) | TMR1_state;
      INTCON = (INTCON & 0xEF) | INT_state<<4;
    }

    return 0;
}



