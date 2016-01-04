/*
The MIT License (MIT)

Copyright (c) 2014 Expert Sleepers Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib.h>

#include "global.h"

#define _DOISR

// Configuration Bit settings
// SYSCLK = 40 MHz (8MHz internal OSC / FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 40 MHz (SYSCLK / FPBDIV)
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_2, FWDTEN = OFF
#pragma config POSCMOD = OFF, FNOSC = FRCPLL, FPBDIV = DIV_1, FSOSCEN = OFF

#pragma config JTAGEN = OFF

// 'time' counter. updates once per audio frame.
volatile unsigned int time = 0;
// the left and right inputs from the codec
volatile int inL = 0, inR = 0;
// data to be sent to the codec
int outL = 0, outR = 0;

// the 'selector' pot value (the top one)
int selector = 0x0;
// the Z pot value
int pot = 0;

// dummy variable for delayLoop()
int looper = 0;

#define _BGF 1
#if _BGF
extern void test_cpp();
#endif


volatile dbgs _d;
volatile int i_global;

void Led_setSelector(int selector);
void Modules_setNewSelector(int selector);

/* very simple implementation of assert
 */

int ecode;
void assert_fail(int code)
{
    ecode = code;   // stash off the code so we can see it in the debugger.
                    // note that is will not crash, so it's safe to run
    Nop();
}

/*
 * Wait a while.
 */
#include <math.h>
void delayLoop( int count )
{
    // once code optimisation is enabled,
    // put this line in to retain similar timing
//    count *= 5;
    //
#if 0 // original
    int i;
    for ( i=0; i<count; ++i )
        looper++;
#endif
    int i;
    int loops = count / 10;
    for (i=0; i< loops ; ++i )
    {
        exp(i);
    }
}

/* used to reset the coded from the button via state machine:
 * 0 = norm
 * 1 = button down
 * 2 = button up, waiting for reset
 */
volatile int resetState = 0;

/*
 * Read the ADC channels (the selector pot and the Z pot).
 * Returns '1' if the algorithm processing loop should end.
 */


int readADC()
{
    _d.adc_ct++;
    int ret = 0;

    // Determine which buffer is idle and create an offset
    unsigned int offset = 8 * ((~ReadActiveBufferADC10() & 0x01));

    // Read the result of conversion from the idle buffer
    int s = ReadADC10(offset);
    _d.adc_val = s;
    if ( ( s | selector ) & 0x20 )
    {
        int lastSelector = selector;
        selector = s >> 6;
        selector = selector < 15 ? selector : 15;
        if ( selector != lastSelector )
        {
            //Led_setSelector(selector);
            Modules_setNewSelector(selector);
            // new algorithm chosen - exit
            ret = 1;
        }
    }
    // Read the result of pot conversion from the idle buffer
    pot = ReadADC10(offset + 1);

    // button pressed?
    if ( !PORTAbits.RA4 )
    {
        // do nothing in this project,
        // but this is where some reaction to the button could go
        //
        ret = 1;
        if (resetState == 0)
            resetState = 1;
    }
    else
    {
        if (resetState == 1)
            resetState = 2;
    }

    mAD1ClearIntFlag();
    return ret;
}

/*
 * Flash the LEDs at startup.
 */
void Led_startupSequence()
{
    PORTA = 0;
    PORTB = 0;
    int j;
    
    // count up 0..15 
    for ( j=0; j<16; ++j )
    {
        Led_setSelector(j);
        delayLoop( 10000 * 2  );
    }
    Led_blank();
}

void Led_showC()
{
    Led_setOne(4);
    Led_setOne(5);
    Led_setOne(6);
    Led_setOne(7);
    Led_setOne(0);
    Led_setOne(3);
}

void Led_showX1()
{
    Led_setOne(0);
    Led_setOne(1);
    Led_setOne(6);
    Led_setOne(7);
}

void Led_showX2()
{
    Led_setOne(2);
    Led_setOne(3);
    Led_setOne(4);
    Led_setOne(5);
}

void Led_showCalibration(int ok)
{
    Led_blank();
    if (ok)
    {
        Led_showC();
        delayLoop( 200000 * 15 );
    }
    else
    {
        int i;
        for (i=0; i<10; ++i)
        {
            Led_blank();
            Led_showX1();
            delayLoop( 200000 );
            Led_blank();
            Led_showX2();
            delayLoop( 200000 );  
        }
    }
    Led_blank();
}

extern int initCal();

/*
 * Main program entry point.
 */
int main(int argc, char** argv)
{
    UINT spi_con1 = 0, spi_con2 = 0;

    Nop();
    // basic system config
    SYSTEMConfig( SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE );
    INTEnableSystemMultiVectoredInt();

    // button on the PCB
    PORTSetPinsDigitalIn(IOPORT_A, BIT_4);

    // LEDs
    PORTSetPinsDigitalOut(IOPORT_A, BIT_2 | BIT_3);
    PORTSetPinsDigitalOut(IOPORT_B, BIT_4 | BIT_6 | BIT_7 | BIT_10 | BIT_11 | BIT_12 | BIT_15);

    // flash the LEDs
    Led_startupSequence();

    // setup the codec
    ConfigureCodec();

    // I2C ports are now digital inputs
    PORTSetPinsDigitalIn(IOPORT_B, BIT_8);
    PORTSetPinsDigitalIn(IOPORT_B, BIT_9);
    
    // set up calibration data
    const int calOK = initCal(); 
    Led_showCalibration(calOK);

    // more LED flashing now that we're all configured
#if 0
    int j;
    for ( j=0; j<4; ++j )
    {
        int i;
        PORTAINV = BIT_2 | BIT_3;
        delayLoop( 500000 );
    }
#endif

    // configure the ADC
    //
    // Tpb = 25ns
    // sample rate = 1/( div * ( 12 + sample_time ) * Tpb )
    // div = 2 * ( ADCS + 1 )
    // Tad = Tpb * div  must be > 83.33ns
    // 2 * ( 24+1 ) * ( 12 + 28 ) * 25ns = 1/20kHz
    // sampling two channels so halve it

    CloseADC10();   // Ensure the ADC is off before setting the configuration
    SetChanADC10( ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN5 |  ADC_CH0_NEG_SAMPLEB_NVREF | ADC_CH0_POS_SAMPLEB_AN11 );
    OpenADC10( ADC_MODULE_ON | ADC_FORMAT_INTG | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON,
               ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_2 | ADC_ALT_BUF_ON | ADC_ALT_INPUT_ON,
               ADC_CONV_CLK_PB | ADC_SAMPLE_TIME_28 | 24,
               ENABLE_AN5_ANA | ENABLE_AN11_ANA,
               SKIP_SCAN_ALL );
    EnableADC10(); // Enable the ADC

    // SPI pins
    PORTSetPinsDigitalOut(IOPORT_A, BIT_1);
    PORTSetPinsDigitalOut(IOPORT_B, BIT_7);
    PORTSetPinsDigitalIn(IOPORT_B, BIT_5);
    PORTSetPinsDigitalOut(IOPORT_B, BIT_2);

    PORTSetPinsDigitalOut(IOPORT_B, BIT_15);

    /*------------------------------------------------
    Function	PPS	Pin#	Test Point#	: Comment
    __________________________________________________________________________
    REFOCLK(o)	RPB2	( 6)	TP36		: Reference Clock Output
    SCK1(o)	-	(25)	TP14            : SPI-I2S SCLK Output
    SS(o)	RPA0	( 2)	TP33            : SPI-I2S LRCK Output
    SDO(o)	RPA1	( 3)	TP20            : SPI-I2S SDO Output
    SDI(i)	RPB5	(14)	-               : SPI-I2S SDI Input */
    PPSOutput(3, RPB2, REFCLKO); //REFCLK0: RPB2 - Out
    PPSInput(2, SDI1, RPB5); //SDI: RPB5 - In
    PPSOutput(2, RPA1, SDO1); //SDO: RPA1 - Out
    PPSOutput(1, RPA0, SS1); //SS: RPB4 - Out

    // Configure Reference Clock Output to 12.288MHz.
    mOSCREFOTRIMSet(REFTRIM);
    OSCREFConfig(OSC_REFOCON_SYSCLK, //SYSCLK clock output used as REFCLKO source
            OSC_REFOCON_OE | OSC_REFOCON_ON, //Enable and turn on the REFCLKO
            RODIV);

    // Configure SPI in I2S mode with 24-bit stereo audio.
    spi_con1 = SPI_OPEN_MSTEN | //Master mode enable
            SPI_OPEN_SSEN | //Enable slave select function
            SPI_OPEN_CKP_HIGH | //Clock polarity Idle High Actie Low
            SPI_OPEN_MODE16 | //Data mode: 24b
            SPI_OPEN_MODE32 | //Data mode: 24b
            SPI_OPEN_MCLKSEL | //Clock selected is reference clock
            SPI_OPEN_FSP_HIGH; //Frame Sync Pulse is active high

    spi_con2 = SPI_OPEN2_IGNROV |
            SPI_OPEN2_IGNTUR |
            SPI_OPEN2_AUDEN | //Enable Audio mode
            SPI_OPEN2_AUDMOD_I2S; //Enable I2S mode

    // Configure and turn on the SPI1 module.
    SpiChnOpenEx(SPI_CHANNEL1, spi_con1, spi_con2, SPI_SRC_DIV);

    //Enable SPI1 interrupt.
#ifdef _DOISR
    IPC7bits.SPI1IP = 3;
    IPC7bits.SPI1IS = 1;
    IEC1bits.SPI1TXIE = 1;
#endif

    SpiChnPutC(SPI_CHANNEL1, 0); //Dummy write to start the SPI
    SpiChnPutC(SPI_CHANNEL1, 0); //Dummy write to start the SPI

    // main loop (never quits)
    for ( ;; )
    {
        runModules();
    }

    return (EXIT_SUCCESS);
}

/*
 * SPI1 ISR
 * 
 * Handle SPI interrupts.
 * Read new data from the codec;
 * write new data to the codec.
 */

#ifdef _DOISR

// this is probably right (no, it crashes))
//void __attribute((interrupt(IPL3SOFT))) SPI1InterruptHandler(void)

// this looks really right, but doesn't compile
//void __interrupt(_SPI_1_VECTOR, IPL3SOFT) SPI1InterruptHandler(void)

// this is OK since SRS doesn't work with ipl3, soft is more efficient than auto
void __ISR(_SPI_1_VECTOR, IPL3SOFT) SPI1InterruptHandler(void)
{
    // normal seq: first TX interrupt is right, next is left.
    // first rx interrupt is left, next is right
    // half rate:
    // tx0: rigth, tx1: left/advance time, tx2: right, tx3: left, don't advance
    // rx0: read left, rx1: read right, rx2: read defnul, rx3: read devnul
  
    if ( IFS1bits.SPI1TXIF )
    {
       // static int toggleData = 0;
        static int txstate = 0;
        switch (txstate)
        {
            case 0:
                SPI1BUF = outR;
                txstate = 1;
                break;
            case 1:
                 SPI1BUF = outL;
                 txstate = 2;
                 ++time;
                break;
            case 2:
                SPI1BUF = outR;
                txstate = 3;
                break;
            case 3:
                 SPI1BUF = outL;
                 txstate = 0;
                 break;
                
        }
        IFS1bits.SPI1TXIF = 0;
    }
    if ( IFS1bits.SPI1RXIF )
    {
        //static BOOL toggleData = TRUE;
        static int rxstate=0;

        int raw = SPI1BUF;
        switch(rxstate)
        {
            case 0:
                raw = ( raw << 8 ) >> 8;
                inL = raw;
                rxstate = 1;
                break;
            case 1:
                raw = ( raw << 8 ) >> 8;
                inR = raw;
                rxstate = 2;
                break;
            case 2:
                rxstate = 3;
                break;
            case 3:
                rxstate = 0;
                break;
        }
        IFS1bits.SPI1RXIF = 0;
    }
}

#if 0 // orig
{
    // normal seq: first TX interrupt is right, next is left.
    // first rx interrupt is left, next is right
    _d.isr_ct++;
    if ( IFS1bits.SPI1TXIF )
    {
        static int toggleData = 0;
        time += toggleData;

        SPI1BUF = toggleData ? outL : outR;
        toggleData = 1 - toggleData;
        IFS1bits.SPI1TXIF = 0;
    }
    if ( IFS1bits.SPI1RXIF )
    {
        static BOOL toggleData = TRUE;

        int raw = SPI1BUF;
        raw = ( raw << 8 ) >> 8;
        if ( toggleData )
            inL = raw;
        else
            inR = raw;
        toggleData = !toggleData;
        IFS1bits.SPI1RXIF = 0;
    }
}
#endif
#endif

