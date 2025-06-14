/*
 * uart.c
 *
 *  Created on: 2025年6月5日
 *      Author: 22934
 */

#include "uart.h"

//void Uart_SCIB_Init(Uint32 baud){
//    unsigned char scihbaud=0;
//    unsigned char scilbaud=0;
//    Uint16 scibaud = 0;
//    scibaud = 30000000 / (8 * baud) - 1;  //低速时钟输入是30MHZ
//    scihbaud = scibaud >> 8;
//    scilbaud = scibaud & 0xff;
//    EALLOW;
//    SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;   // SCI-A
//    EDIS;
//    InitScibGpio();
//    ScibRegs.SCIFFTX.all = 0xE040;
//    ScibRegs.SCIFFRX.all = 0x204f;
//    ScibRegs.SCIFFCT.all = 0x0;
//    ScibRegs.SCICCR.all = 0x0007;   // 1 stop bit,  No loopback
//                                   // No parity,8 char bits,
//                                   // async mode, idle-line protocol
//    ScibRegs.SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
//                                   // Disable RX ERR, SLEEP, TXWAKE
//    ScibRegs.SCICTL2.all = 0x0003;
//    ScibRegs.SCICTL2.bit.TXINTENA = 1;
//    ScibRegs.SCICTL2.bit.RXBKINTENA = 1;
//    ScibRegs.SCIHBAUD = scihbaud;  // 9600 baud @LSPCLK = 37.5MHz.
//    ScibRegs.SCILBAUD = scilbaud;
////  ScibRegs.SCICCR.bit.LOOPBKENA =1; // Enable loop back
//    ScibRegs.SCICTL1.all = 0x0023;     // Relinquish SCI from Reset
//}
//

