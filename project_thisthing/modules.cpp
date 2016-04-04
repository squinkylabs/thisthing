    
/* all 16 modules will live here
 */


#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib.h>

#include "global.h"
#define _CAL 200                // force sig proc code to use calibration

#define printf(a) DONT_DO_THIS()

#include "DM_SGTriggerGenerator.h"
#include "DMMisc.h"
#include "DMSampleAndHold.h"
#include "DMRandomSource1.h"
#include "DMSlopeDetector.h"
#include "DMTest.h"
#include "DacVoltage.h"
#include "DMCounter.h"
#include "DMQuantizer.h"
#include "DMTuner.h"
#include "DMShiftRegister.h"
#include "DMAdders.h"
#include "DMTuningRef.h"
#include "DMTrackAndHoldAla152.h"
#include "DMSwitch.h"
#include "DMBankSelector.h"
#include "DMScaleQuantizer.h"
#include "PersistentInts.h"

// externs
extern "C" void assert_fail(int code);
extern "C" void Led_clock();
extern "C" void Led_setSelectorAndBank(int selector, int bank);
extern "C" void getCalibratedInput(int * inLeft, int * inRight, int * inPot );
extern "C" void calibrateAndPutOutput(int left, int right);
extern "C" void initPersistence(void * calAddress);

// forward declarations
void processZ(ZState * outState, int zValue);

// This is for runtime performance debugging
typedef struct
{
    unsigned int sampleCount0;
    unsigned int elapsedSamples;
    int loops;
} TimeMetrics;

volatile TimeMetrics tm;
static bool resetModulesFlag = false;

PersistentInts persistentInts;

void setBankNumber(int bank)
{
    persistentInts.write(PersistentInts::bankOffset, bank);
}

static int getBankNumber()
{
    const int ret = persistentInts.get(PersistentInts::bankOffset);
    return ret;
}

void initPersistence(void * calAddress)
{
    persistentInts.init(calAddress);
}



DModule * modules_bank0[16] = {
    
    // 1A
    new DMSampleAndHoldDual(), 

    // 1B
    new DMSampleAndHoldDualClock(), 
    
    // 1C
    new DMSampleAndHold(),
    
    // 1D
    new DMTrackAndHoldDualClock(), 

    // 2A
    new DMSampleAndHoldProbabalistic(), 
    
    // 2B
    new DMTrackAndHoldProbabalistic(), 
    
    // 2C
    new DMTrackAndHoldAla152(), 
    
     // 2D
    new DMTrackAndHoldExtendedAla152(), 

    // 3A
    new DMDigitalShiftRegister(),
          
    // 3B
    new DMSlopeDetectorDual(), 
    
    // 3C
    new DMRiseAndFallDetector(),
    
    // 3D
    new DMDualFunctionQuantizerHelper(),

    // 4A
    new DMFPrecisionFMHelper(),        
    
    // 4B
    new DMOctaveAdderSubtractor(),
    
    // 4C
    new DMSemitoneAdderSubtractor(),
 
    // 4D
    //new DMTuningReference()
    // new DMSwitch()
    new DMBankSelector(2)
};




DModule * modules_bank1[16] = {
    
    //1A-II
    new DMTuningReference(),
    
    //1B-II
    new DMSwitch(),
    
    //1C-II
    new DMScaleQuantizer1(),
    
    //1D-II
    new DMScaleQuantizer3(),
    
    //2A-II
    new DMZero(),

    //2B-II
    new DMZero(),
    
    //2C-II
    new DMZero(),
    
    //2D-II
    new DMZero(),
    
    //3A-II
    new DMZero(),

    //3B-II
    new DMZero(),
    
    //3C-II
    new DMZero(),
  
    //3D-II
    new DM_SGTriggerGenerator(),
   //new DMZero(),
    
    //4A-II
    new DMRandomSource1(Constants::DISTING_SAMPLE_RATE),
    
    //4B-II
    new DMRandomSource2(Constants::DISTING_SAMPLE_RATE),

    //4C-II
    new DMScaleQuantizer2(),
    
    // 4D
    new DMBankSelector(2)
};


DModule** modules[2] = {
    modules_bank0,
    modules_bank1
};

/* runs module until it is time to switch.
 * takes input from the globals set up by disting main
 */ 

// don't define BENCHMARK for normal operation
//#define BENCHMARK 2 // 1 = run bench normally, 2 = no z, 3 = z every 10 samples
#define BENCHMULT 1


#if BENCHMARK

int potDiv = 0;
int syntheticPotVal = 200;
static inline void runModuleOnce()
{
    
    for (int i=0; i < BENCHMULT; ++i)
    {
#if BENCHMARK == 1
        int potVal = pot;
#elif BENCHMARK == 2
        int potVal = 512;
#else
        if (++potDiv >= 10)
        {
            // flip between 200 and 600 every 10 samples
            potDiv = 0;
            syntheticPotVal = (syntheticPotVal == 200) ? 600 : 200;
        }
        int potVal = syntheticPotVal;
#endif
        int rawOutL, rawOutR;   // sent output to temp
        int calibratedInL, calibratedInR, calibratedZ;
            
        ZState z(calibratedZ, true);   
        getCalibratedInput(&calibratedInL, &calibratedInR, &calibratedZ);
         //calibratedInL=0; calibratedInR=0; calibratedZ=0;
        modules[selector]->go(false, calibratedInL, calibratedInR, z, rawOutL, rawOutR);
        calibrateAndPutOutput(rawOutL, rawOutR);  // TODO: pass real values 
    }
}
#endif


// normal run loop
#if !BENCHMARK
static inline void runModuleOnce()
{
 #if _CAL == 200
            int rawOutL, rawOutR;   // sent output to temp
            int calibratedInL, calibratedInR, calibratedZ;
            
            getCalibratedInput(&calibratedInL, &calibratedInR, &calibratedZ);
           // calL = calibratedInL;
           // calR = calibratedInR;
          //  ZState z(calibratedZ, true);    // TODO: detect pot change
            ZState z;
            processZ(&z, calibratedZ);
            
            if (resetModulesFlag)
                z.changed = false;
           
            const int bank = getBankNumber();
            modules[bank][selector]->go(resetModulesFlag, calibratedInL, calibratedInR, z, rawOutL, rawOutR);
            calibrateAndPutOutput(rawOutL, rawOutR);  // TODO: pass real values
            resetModulesFlag = false;
            // rawL = rawOutL;
           // rawR = rawOutR;
#endif   
}
#endif

extern volatile int resetState;

extern "C" void runModules()
{
    DECLARATIONS();
    tm.loops = 0;
    tm.sampleCount0 = time;
    _d.pot_changes = 0;
    while (1)
    {
        tm.loops++;
        tm.elapsedSamples = time - tm.sampleCount0;
        IDLE();
        
        if (resetState == 2)
        {
            ConfigureCodec();
            resetState = 0;
        }
        
        runModuleOnce();
        Led_clock();
   
        LOOP_END();
    }
}


/* Z processing state machine */

static int lastOfficialZValue = 0;

static int zstate = 0;  // 0 = just reset, waiting for big change
                        // 1 = normal increasing pot state
                        // 2 = normal decreasing pot state

const int zBigDelta = 1024 / 20;
const int zLittleDelta = 4;
const int zMediumDelta = zLittleDelta * 2;

void processZ(ZState * outState, int zValue)
{
    bool changed = false;
    switch(zstate)
    {
        case 0:
            if (zValue > (lastOfficialZValue+zBigDelta))
            {
                changed = true;
                zstate = 1;
            }
            else if (zValue < (lastOfficialZValue-zBigDelta))
            {
                changed = true;
                zstate = 2;
            }
            break;
        case 1:
            // small changes in current direction are recognized
            if ( zValue > (lastOfficialZValue+zLittleDelta))
            {
                changed = true;
            }
            
            // hysteresis: somewhat larger changes in opposite direction are recognized
            else if ( zValue < (lastOfficialZValue-zMediumDelta))
            {
                changed = true;
                zstate = 2;
            }
            break; 
        case 2:
            // small changes in current direction are recognized
            if ( zValue < (lastOfficialZValue-zLittleDelta))
            {
                changed = true;
            }
            
            // hysteresis: somewhat larger changes in opposite direction are recognized
            else if ( zValue > (lastOfficialZValue+zMediumDelta))
            {
                changed = true;
                zstate = 1;
            }
            break; 
        default:
            xassert(false, 227);
    }
    if (changed)
    {
        lastOfficialZValue = zValue;
    }
    outState->changed = changed;
    outState->value = lastOfficialZValue;        // TODO: detect change
}

void doReset()
{
    resetModulesFlag = true;
    zstate = 0;
}

// called from main when he has new selector
extern "C" void Modules_setNewSelector(int selector)
{
    const int bank = getBankNumber();
    if (bank)
    {
        Nop();
    }
    Led_setSelectorAndBank(selector, bank);  // tell the display system about this
    doReset();      // we got a new selector, so reset the modules
}
  






