
/* Disting calibration code 
 */


#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib.h>       // problems compiling??

#include "global.h"

void calSubFloat(int index);

// This is supposed to be "neutral values" (what we would see on a perfect unit)
// I wasn't too rigorous here - might be wrong.
int fake_cal_table[8] = {
    0, 0, 3728270, 2396745,
    0, 0, 3728270, 2396745
};
// virtual address of the ROM calibrations data (mk1 disting)
//int * cal_rom =  (int *)((void *) 0x1d01fe00); // BAD VERSION

#define CAL_ADDR_MKI  0x1d01fe00     // physical address of calibration data in flash
#define CAL_ADDR_MKII 0x1d03fe00 

// get a pointer in virtual address space
//int * cal_rom = (int * )PA_TO_KVA0(CAL_ADDR);

int * cal_rom = 0;

int itemp;
int isValid(int * p)
{
    Nop();
    itemp = p[0];
    itemp = p[1];
    itemp = p[2];
    itemp = p[3];
    
    if ((p[0] > 0x3ffff) || (p[0] < -0x3ffff) )
        return 0;
    if ((p[1] > 0x3ffff) || (p[1] < -0x3ffff) )
        return 0;
    if ((p[2] > 0x7fffff) || (p[2] < 0x1fffff ))
        return 0;
    if ( (p[3] > p[2]) ||  (p[3] < 0x1fffff))
            return 0;
    return 1;
   
}

int * pp;
int findRom()
{
    Nop();
    int * temp;
    
    // look for mk I table
    temp = (int * )PA_TO_KVA0(CAL_ADDR_MKI);
    if (isValid(temp))
    {
        cal_rom = temp;
        return 1;
    }
    
    //if not found, look for mk II
    temp = (int * )PA_TO_KVA0(CAL_ADDR_MKII);
    pp = temp;
    if (isValid(temp))
    {
        cal_rom = temp;
        return 1;
    }
    
    // If we don't find one, just use fake to we don't crash
    temp = fake_cal_table;
    if (isValid(temp))
    {
        cal_rom = temp;
        return 0;
    }
  
    xassert(0, 1046);
    cal_rom = temp;
    return 0;
}


/* Global variables to hold the calibrations constants
 */

static int A_cal[2];
static long long Br_cal[2];
static int D_cal[2];
static long long Er_cal[2];

const int romOffsetInputGnd =0;
const int romOffsetOutputZero =1;
const int romOffsetOutputHalf =2;
const int romOffsetOutputThree =3;

//long long temp64;
//double dtemp;
//int itemp;

// z pot calibration
int bpot = 0;
int apot =  0;

void calPot()
{
    double x0 = 170;
    double x1 = 875;
    
    // y= 0..1024
    
    double a = 1024.0 / (x1 - x0);
    double b = -a * x0;
    
    apot = a * (1 << 16);
    bpot = b;
}


void calSub(int index)
{
    Nop();
    itemp = index;
    xassert(cal_rom, 1042);
    xassert( (index <= 1 && index >=0), 1022);
    int * rom = cal_rom + (index * 4);
    
    A_cal[index] = rom[romOffsetInputGnd];
    const long long B = (rom[romOffsetOutputThree] - rom[romOffsetInputGnd]) / 3;
    Br_cal[index] =  0x80000000000LL / B;
    
    D_cal[index] = ((long long)(rom[romOffsetOutputZero] - rom[romOffsetInputGnd]) << 19) / B;
    long long E2 = (( 1<< 27) * B) / (rom[romOffsetOutputHalf] - rom[romOffsetOutputZero]);
    Er_cal[index] =  E2;

   // calSubFloat(index);
    Nop();
}

/* test values:
 * 
 * run 1: bruce's first try
 * A = -43211, -66020
 * D =  1809, -177033
 * Br = 10,892,535, 10,661,969
 * Er = 28.644.963, 28.106.710
 * 
 * out voltage (5): 5.02, 5.01
 *    (1) : 1.006, .991
 *    (0) : 1.1 mv , -13.4 mv
 * 
 * run 2
 * A = -43221, -66020
 * D = 1174, -112504
 * Br, Er (same)
 * 
 * (5) : 5.01, 5.01
 * (1) : 1.004, 1.005
 * (0) : .1 mv,  .6mv
 * 
 * inputs:
 * (0) : 2080, -2398  (2080 = 4mv)
 * (1) : 525762, 522879 (errL 1474, -1412)
 * (5) : 2,622,301, 2,620,394 (( err = 861, -1049
 */
int initCal()
{
    const int ret = findRom();
    calSub(0);
    calSub(1);
    calPot();
    Nop();
    return ret;
}



//int _cil, _cir;
void getCalibratedInput(int * inLeft, int * inRight, int * inPot )
{
    *inLeft = ((inL - A_cal[0]) * Br_cal[0]) >> 24;
    *inRight = ((inR - A_cal[1]) * Br_cal[1]) >> 24;
    
    int y = ((apot * pot) >> 16) + bpot;
    if (y < 0) y=0;
    if (y > 0x3ff) y = 0x3ff;
    *inPot = y;
}


const int MAX24 = 0x7fffff;
const int MIN24 = 0xff800000;


void calibrateAndPutOutput(int left, int right)
{

    int toutL =  (((left  -D_cal[0]) * Er_cal[0]) >> 24);
    if (toutL < MIN24) toutL = MIN24;
    if (toutL > MAX24) toutL = MAX24;
    outL = toutL;
    
    int toutR = (((right - D_cal[1]) * Er_cal[1]) >> 24);
    if (toutR < MIN24) toutR = MIN24;
    if (toutR > MAX24) toutR = MAX24;
    outR = toutR;
}

/***** new floating point experiment
 */

#if 0 // new float based way (experiment)
const int gainShift = 10;
int GnO[2];     // normalized DAC output gain comp
int OnO[2];     // normalized DAC output offset comp


const double H = 0x400000;      // half dac range positive
const int gainNormalizationShift = 10;
const double oneV = 1<<19;

void calSubFloat(int index)
{
    int * rom = cal_rom + (index * 4);
    
    double k1 = rom[0];

	// 4)
	double k2 = (double)(rom[3] - rom[0]) / 3.0;

	// first try
	//k3 = -H / (R[1] - R[2]);
	double k3 = ((double)rom[1] - k1) / k2;

	

	//k4 = (R[1] - k1) / (k3*k2);
	double k4 = (1/H) * ((((double)rom[2] - k1) / k2) - k3);


    // Now that we have solved for the device errors, calculate the
    // compensation
    	double gainScale = 1 << gainNormalizationShift;

		double offset =  (-k3 * oneV);	// input will we rel 1v.19
		double gain = ( gainScale / k4);		// bump up gain for more decimal points
		
		int _gain = (int) gain;
		xassert(_gain > 0, 259);
		xassert(_gain > gain - 1, 260);
		xassert(_gain < gain + 1, 261);
        GnO[index] = _gain;

		int _offset = (int) offset;
		//assert(_offset > 0);
		xassert(_offset > offset - 1, 267);
		xassert(_offset < offset + 1, 268);
        OnO[index] = _offset;

}


void calibrateAndPutOutput(int left, int right)
{
    	long long v = left;		
		long long x = (v + OnO[0]) * GnO[0];
		x >>= (gainNormalizationShift + 19);
        int toutL = x;
        if (toutL < MIN24) toutL = MIN24;
        if (toutL > MAX24) toutL = MAX24;
        outL = toutL;
    
        
        v = right;		
		x = (v + OnO[1]) * GnO[1];
		x >>= (gainNormalizationShift + 19);
        int toutR = x;
        if (toutR < MIN24) toutR = MIN24;
        if (toutR > MAX24) toutR = MAX24;
        outR = toutR;
}
#endif