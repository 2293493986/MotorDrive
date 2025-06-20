// TI File $Revision: /main/2 $
// Checkin $Date: January 4, 2011   10:10:28 $
//###########################################################################
//
// FILE:	F2806x_I2C.c
//
// TITLE:	F2806x I2C Initialization & Support Functions.
//
//###########################################################################
// $TI Release: 2806x C/C++ Header Files and Peripheral Examples V1.00 $
// $Release Date: January 11, 2011 $
//###########################################################################

#include "F2806x_Device.h"     // F2806x Headerfile Include File
#include "F2806x_Examples.h"   // F2806x Examples Include File

//---------------------------------------------------------------------------
// InitI2C:
//---------------------------------------------------------------------------
// This function initializes the I2C to a known state.
//

#if DSP28_I2CA
void InitI2C(void)
{
	// Initialize I2C-A:

	//tbd...
}
#endif // endif DSP28_I2C

//---------------------------------------------------------------------------
// Example: InitI2CGpio:
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as I2C pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.
//
// Caution:
// Only one GPIO pin should be enabled for SDAA operation.
// Only one GPIO pin shoudl be enabled for SCLA operation.
// Comment out other unwanted lines.

#if DSP28_I2CA
void InitI2CGpio()
{

   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

//	GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up for GPIO28 (SDAA)
//	GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;    // Enable pull-up for GPIO29 (SCLA)

    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   // Enable pull-up for GPIO33 (SCLA)

/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

//    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SDAA)
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 3;  // Asynch input GPIO29 (SCLA)

    GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)

/* Configure I2C pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be I2C functional pins.
// Comment out other unwanted lines.

//	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 2;   // Configure GPIO28 for SDAA operation
//	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 2;   // Configure GPIO29 for SCLA operation

	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // Configure GPIO32 for SDAA operation
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // Configure GPIO33 for SCLA operation

    EDIS;
}
#endif // endif DSP28_I2CA

//===========================================================================
// End of file.
//===========================================================================
