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

#ifndef GLOBAL_H
#define	GLOBAL_H

#ifdef	__cplusplus
extern "C" {
#endif

//typedef long long int64_t;
//typedef unsigned long long uint64_t;

extern int selector;
extern int pot;

extern void delayLoop( int count );

extern void ConfigureCodec();
extern void ErrorHalt();

#define SYS_FREQ 	(40000000L)

/* from the internet:
 Because the RODIV factor only allows even divisors. 

From the data sheet:

0 = No divide, 
1 = /2, 
2 = /4, 
3 = /6, */
// unused #define SAMPLE_RATE     78125

/* These values worked for half rate, but OS says spi_src_div wants to be 4
#define RODIV		1  // half sampe rate
#define REFTRIM		0
#define SPI_SRC_DIV 8
 */

// these are my "final" half rate

#define RODIV		2  // half sampe rate
#define REFTRIM		0
#define SPI_SRC_DIV 4
 

// original values
/*
#define RODIV		0  // half sampe rate
#define REFTRIM		0
#define SPI_SRC_DIV 8
 * */



extern volatile unsigned int time;
extern volatile int inL, inR;
extern int outL, outR;
extern int readADC();

#define DECLARATIONS()              \
    unsigned int thisTime, lastTime = time;

#define IDLEWAIT()                  \
        while ( ( thisTime = time ) == lastTime )  \
            ;

#define IDLE()                      \
        PORTACLR = BIT_2;           \
        IDLEWAIT()                  \
        PORTASET = BIT_2;

#define LOOP_END()                  \
        lastTime = thisTime;        \
        if ( mAD1GetIntFlag() )     \
        {                           \
            if ( readADC() )        \
                return;             \
        }

typedef struct
{
    int err;
 //   int alg_loop;
  //  int alg_enter;
    int isr_ct;
    int adc_ct;
    int adc_val;
    int last_sel;
    int last_pot;
    int pot_changes;
    double lfo;
} dbgs;

extern volatile dbgs _d;


#if 0 // let's not put externs in this file shared with c and c++
extern void runModules();

/* functions in calib.c */
extern int initCal();
extern void getCalibratedInput(int * inLeft, int * inRight, int * inPot );
extern void calibrateAndPutOutput(int left, int right);



extern void assert_fail(int code);
#endif

#define xassert(condition, code) if (!condition) assert_fail(code);
#define assert (void *)

#ifdef	__cplusplus
}
#endif

#endif	/* GLOBAL_H */
