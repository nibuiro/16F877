/*
 * 
 * [PIC16F87X - Microchip Technology](http://ww1.microchip.com/downloads/jp/devicedoc/30292a_jp.pdf)
 * [ＵＳＡＲＴ(非同期)の実験１](http://www.geocities.jp/zattouka/GarageHouse/micon/USART/USART.htm)
 * [シリアル通信（オウム返し）](http://diy.ease-labs.com/?page_id=553)
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
#include <stdbool.h>

#define _XTAL_FREQ 20000000


unsigned char rx_data = 0;
bool rx_flg = false;



void PICinit(){
  TRISA  = 0x00;
  TRISB  = 0x00;
  TRISC  = 0x00;
  TRISD  = 0x00;
  PORTA  = 0x00;   
  PORTB  = 0x00;         
  PORTC  = 0x00;        
  PORTD  = 0x00;            
  
  ADCON0 |= 0x01;
  ADCON1 |= 0x06;
  
  OPTION_REG |= 0b10000000 ; // デジタルI/Oに内部プルアップ抵抗を使用する
}


// シリアル通信の受信処理
// シリアルデータを受信する時に発生
void interrupt InterReceiver(void)
{
    if( 0x20 == (PIR1 & 0x20) ) {
        rx_data = RCREG;    // レジスタからデータ格納
        rx_flg = true;
        PIR1 &= 0xDF;           // 割込受信フラグをリセット
    }
}


int main(int argc, char** argv)
{
    PICinit();
    RD1 = 1;

    /*
     * RXDTSEL, TXCKSEL などはrx, txとして使用するポートを選択可能なPICにおいて
     * それを指定する値として用いられる。
     * よって16F87X系ではポートが定まっているため存在しない
     */
    TRISC  = 0x80 ;   // RC7(RX)を入力とする
    PORTC  = 0x00 ;   // ピン状態初期化
    
    /*
     * TXSTA p.105 
     * TDRに書かれた送信データは、自動的にTSRに転送され、TxDn端子に送出
     */
    TXSTA   |= 0x20;         // 送信イネーブル/ 非同期高速ボーレート
    TXSTA &= 0x0B;
    RCSTA   |= 0x90;         // シリアルポートイネーブル/ 連続受信イネーブル

    /*
     * p.107
     * 希望のボーレート = Fosc / (16 (X + 1))
     * X = value in SPBRG (0 to 255)
     * 
     * 20000000 / ( 38400 * 16 ) - 1 = 31.552083333333336
     */
    SPBRG  = 129;          // ボーレートを38400に設定

    PIR1 &= 0xDF; // RCIF = 0; //  受信バッファが空　とする
    PIE1 |= 0x20; // RCIE = 1; // USART 受信割り込みを使用可能にする
    
    /*
     * PEIE = 1;               // 周辺装置割込みを有効
     * GIE  = 1;               // 全割込み処理を許可
     */
    INTCON |= 0xC0;

    while(1)
    {        
        RD1 = 1;            //RD1のピンをHIGH
        __delay_ms(1000);    //1000ms遅延
        RD1 = 0;
        __delay_ms(1000);
        
        if( rx_flg ) { // データを受信したか？
            /*
             * p.111
             * TXIF: PIR1 Bit4
             * 送信レジスタが空のとき 1
             */
            if( 0x10 == (PIR1 & 0x10) ) {
                TXREG = rx_data;    // 送信レジスタへデータをセットする
                rx_flg = false;
            }
        }
    }

    return 0;
}