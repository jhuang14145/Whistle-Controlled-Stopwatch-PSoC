//*****************************************************************************
//*****************************************************************************
//  FILENAME: Timer_100ms.h
//   Version: 2.6, Updated on 2015/3/4 at 22:27:47
//  Generated by PSoC Designer 5.4.3191
//
//  DESCRIPTION: Timer16 User Module C Language interface file
//-----------------------------------------------------------------------------
//  Copyright (c) Cypress Semiconductor 2015. All Rights Reserved.
//*****************************************************************************
//*****************************************************************************
#ifndef Timer_100ms_INCLUDE
#define Timer_100ms_INCLUDE

#include <m8c.h>

#pragma fastcall16 Timer_100ms_EnableInt
#pragma fastcall16 Timer_100ms_DisableInt
#pragma fastcall16 Timer_100ms_Start
#pragma fastcall16 Timer_100ms_Stop
#pragma fastcall16 Timer_100ms_wReadTimer                // Read  DR0
#pragma fastcall16 Timer_100ms_wReadTimerSaveCV          // Read  DR0      
#pragma fastcall16 Timer_100ms_WritePeriod               // Write DR1
#pragma fastcall16 Timer_100ms_wReadCompareValue         // Read  DR2
#pragma fastcall16 Timer_100ms_WriteCompareValue         // Write DR2

// The following symbols are deprecated.
// They may be omitted in future releases
//
#pragma fastcall16 wTimer_100ms_ReadCounter              // Read  DR0 "Obsolete"
#pragma fastcall16 wTimer_100ms_CaptureCounter           // Read  DR0 "Obsolete"
#pragma fastcall16 wTimer_100ms_ReadTimer                // Read  DR0 (Deprecated)
#pragma fastcall16 wTimer_100ms_ReadTimerSaveCV          // Read  DR0 (Deprecated)
#pragma fastcall16 wTimer_100ms_ReadCompareValue         // Read  DR2 (Deprecated)


//-------------------------------------------------
// Prototypes of the Timer_100ms API.
//-------------------------------------------------

extern void Timer_100ms_EnableInt(void);                           // Proxy 1
extern void Timer_100ms_DisableInt(void);                          // Proxy 1
extern void Timer_100ms_Start(void);                               // Proxy 1
extern void Timer_100ms_Stop(void);                                // Proxy 1
extern WORD Timer_100ms_wReadTimer(void);                          // Proxy 1
extern WORD Timer_100ms_wReadTimerSaveCV(void);                    // Proxy 2
extern void Timer_100ms_WritePeriod(WORD wPeriod);                 // Proxy 1
extern WORD Timer_100ms_wReadCompareValue(void);                   // Proxy 1
extern void Timer_100ms_WriteCompareValue(WORD wCompareValue);     // Proxy 1

// The following functions are deprecated.
// They may be omitted in future releases
//
extern WORD wTimer_100ms_ReadCompareValue(void);       // Deprecated
extern WORD wTimer_100ms_ReadTimerSaveCV(void);        // Deprecated
extern WORD wTimer_100ms_ReadCounter(void);            // Obsolete
extern WORD wTimer_100ms_ReadTimer(void);              // Deprecated
extern WORD wTimer_100ms_CaptureCounter(void);         // Obsolete


//--------------------------------------------------
// Constants for Timer_100ms API's.
//--------------------------------------------------

#define Timer_100ms_CONTROL_REG_START_BIT      ( 0x01 )
#define Timer_100ms_INT_REG_ADDR               ( 0x0e1 )
#define Timer_100ms_INT_MASK                   ( 0x02 )


//--------------------------------------------------
// Constants for Timer_100ms user defined values
//--------------------------------------------------

#define Timer_100ms_PERIOD                     ( 0x61a8 )
#define Timer_100ms_COMPARE_VALUE              ( 0x0 )


//-------------------------------------------------
// Register Addresses for Timer_100ms
//-------------------------------------------------

#pragma ioport  Timer_100ms_COUNTER_LSB_REG:    0x020      //Count register LSB
BYTE            Timer_100ms_COUNTER_LSB_REG;
#pragma ioport  Timer_100ms_COUNTER_MSB_REG:    0x024      //Count register MSB
BYTE            Timer_100ms_COUNTER_MSB_REG;
#pragma ioport  Timer_100ms_PERIOD_LSB_REG: 0x021          //Period register LSB
BYTE            Timer_100ms_PERIOD_LSB_REG;
#pragma ioport  Timer_100ms_PERIOD_MSB_REG: 0x025          //Period register MSB
BYTE            Timer_100ms_PERIOD_MSB_REG;
#pragma ioport  Timer_100ms_COMPARE_LSB_REG:    0x022      //Compare register LSB
BYTE            Timer_100ms_COMPARE_LSB_REG;
#pragma ioport  Timer_100ms_COMPARE_MSB_REG:    0x026      //Compare register MSB
BYTE            Timer_100ms_COMPARE_MSB_REG;
#pragma ioport  Timer_100ms_CONTROL_LSB_REG:    0x023      //Control register LSB
BYTE            Timer_100ms_CONTROL_LSB_REG;
#pragma ioport  Timer_100ms_CONTROL_MSB_REG:    0x027      //Control register MSB
BYTE            Timer_100ms_CONTROL_MSB_REG;
#pragma ioport  Timer_100ms_FUNC_LSB_REG:   0x120          //Function register LSB
BYTE            Timer_100ms_FUNC_LSB_REG;
#pragma ioport  Timer_100ms_FUNC_MSB_REG:   0x124          //Function register MSB
BYTE            Timer_100ms_FUNC_MSB_REG;
#pragma ioport  Timer_100ms_INPUT_LSB_REG:  0x121          //Input register LSB
BYTE            Timer_100ms_INPUT_LSB_REG;
#pragma ioport  Timer_100ms_INPUT_MSB_REG:  0x125          //Input register MSB
BYTE            Timer_100ms_INPUT_MSB_REG;
#pragma ioport  Timer_100ms_OUTPUT_LSB_REG: 0x122          //Output register LSB
BYTE            Timer_100ms_OUTPUT_LSB_REG;
#pragma ioport  Timer_100ms_OUTPUT_MSB_REG: 0x126          //Output register MSB
BYTE            Timer_100ms_OUTPUT_MSB_REG;
#pragma ioport  Timer_100ms_INT_REG:       0x0e1           //Interrupt Mask Register
BYTE            Timer_100ms_INT_REG;


//-------------------------------------------------
// Timer_100ms Macro 'Functions'
//-------------------------------------------------

#define Timer_100ms_Start_M \
   ( Timer_100ms_CONTROL_LSB_REG |=  Timer_100ms_CONTROL_REG_START_BIT )

#define Timer_100ms_Stop_M  \
   ( Timer_100ms_CONTROL_LSB_REG &= ~Timer_100ms_CONTROL_REG_START_BIT )

#define Timer_100ms_EnableInt_M   \
   M8C_EnableIntMask(  Timer_100ms_INT_REG, Timer_100ms_INT_MASK )

#define Timer_100ms_DisableInt_M  \
   M8C_DisableIntMask( Timer_100ms_INT_REG, Timer_100ms_INT_MASK )

#endif
// end of file Timer_100ms.h
