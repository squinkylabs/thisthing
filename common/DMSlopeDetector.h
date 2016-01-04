
#ifndef INCLUDE_DMSLOPEDETECTOR
#define INCLUDE_DMSLOPEDETECTOR

#include "DModule.h"
#include "SlopeDetector.h"
#include "ExpPot.h"
#include "DACVoltage.h"
#include "TriggerOutput.h"
#include "SampleAndHold.h"
#include "Constants.h"

extern int i_global;


// cv1 -> X
// cv2 -> Y
//trig out 1-> A
// trig out 2 -> B

// detects rising and falling
// this version uses the fast lookup toable to map z to filter coeffs
class DMSlopeDetectorDual : public DModule
{
public:
	static const int quarterToneMV = 1000 / 24;

	// just for legacy tests

	DMSlopeDetectorDual(int tcUS) : 
		_sd1(quarterToneMV, tcUS, Constants::DISTING_SAMPLE_RATE, true),
		_sd2(quarterToneMV, tcUS, Constants::DISTING_SAMPLE_RATE, true)
	{

		_inputFilterx.set(1 , Constants::GIBBS_CUTOFF_RATIO);		// set it really high
		_inputFiltery.set(1 , Constants::GIBBS_CUTOFF_RATIO);		// set it really high
	}


	// 4 millisecond TC
	DMSlopeDetectorDual() : 
		_sd1(quarterToneMV, 4000, Constants::DISTING_SAMPLE_RATE, true),
		_sd2(quarterToneMV, 4000, Constants::DISTING_SAMPLE_RATE, true)
	{
		
		_inputFilterx.set(1 , Constants::GIBBS_CUTOFF_RATIO);		// set it really high
		_inputFiltery.set(1 , Constants::GIBBS_CUTOFF_RATIO);		// set it really high
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		_inputFilterx.go(x);
		x =  _inputFilterx.get();
		_inputFiltery.go(y);
		y =  _inputFiltery.get();

		_sd1.go(x);
		_out1.go(_sd1.getBothSlope());
		_sd2.go(y);
		_out2.go(_sd2.getBothSlope());

		a = _out1.get();
		b = _out2.get();
	}
private:
	SlopeDetector _sd1;
	SlopeDetector _sd2;
	TriggerOutput _out1;
	TriggerOutput _out2;

	TwoPoleLPF_f _inputFilterx;
	TwoPoleLPF_f _inputFiltery;
#ifdef _TESTACDPERF
    int _hack;
#endif
};


/*
x = CV in

a = trigger when rise detected
b = trigger when fall detected
*/

class DMRiseAndFallDetector : public DModule
{
public:
	static const int quarterToneMV = 1000 / 24;


	// 4 millisecond TC
	DMRiseAndFallDetector() : 
		_sd(quarterToneMV, 4000, Constants::DISTING_SAMPLE_RATE, true)
	{
		_inputFilter.set(1 , Constants::GIBBS_CUTOFF_RATIO);		// set it really high
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		_inputFilter.go(x);
		x =  _inputFilter.get();
		_sd.go(x);

		_out1.go(_sd.getPositiveSlope());
		_out2.go(_sd.getNegativeSlope());

		a = _out1.get();
		b = _out2.get();
	}
private:
	SlopeDetector _sd;

	TriggerOutput _out1;
	TriggerOutput _out2;

	TwoPoleLPF_f _inputFilter;

#ifdef _TESTACDPERF
    int _hack;
#endif
};

/*
z = clock or gate
x = CV in
a = s&h (from x)

y = CV in (output of quantizer)
b = slope detector trigger (lag sensitivity preset to 50 ms)
*/
class DMDualFunctionQuantizerHelper  : public DModule
{
public:
	static const int quarterToneMV = 1000 / 24;
	DMDualFunctionQuantizerHelper() : 
		_trig(true),
		_sd(quarterToneMV, 4000, Constants::DISTING_SAMPLE_RATE, true)
	{
		doReset();
		_inputFilter.set(1 , Constants::GIBBS_CUTOFF_RATIO);		// set it really high
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			doReset();
		}
		// A
		_trig.go(z.value);		// ignore the "changed" flag - this is continuous
		_sh.go(_trig.gate(), _trig.trigger(), x);
		a = _sh.sampleAndHold();

		//B
		_inputFilter.go(y);
		y = _inputFilter.get();
		_sd.go(y);
		_trigOut.go(_sd.getBothSlope());
		b = _trigOut.get();
	}
private:
	private:
	SampleAndHold _sh;
	GateTrigger _trig;
	SlopeDetector _sd;
	TriggerOutput _trigOut;
	TwoPoleLPF_f _inputFilter;

	void doReset()
	{
		_sh.reset();
		_trig.reset();
	}
};



// cv1 -> X
// cv2 -> Y
//trig out 1-> A
// trig out 2 -> B
// Z is sensitivity
// detects rising and falling
// this version uses the fast lookup toable to map z to filter coeffs
#if 0
class DMSlopeDetectorDual_old : public DModule
{
public:
	static const int quarterToneMV = 1000 / 24;


	// TODO: what do we want to do about pot range????
	DMSlopeDetectorDual_old(int minTc_US = 1800, int maxTc_US=180000) : 
		_sd1(quarterToneMV, 1 * 1000, Constants::DISTING_SAMPLE_RATE, true),
		_sd2(quarterToneMV, 1 * 1000, Constants::DISTING_SAMPLE_RATE, true),
		_scalePot_to_k((float)minTc_US , (float)maxTc_US),          //1.8 ms to 180 ms
		_lastZ(-1),
        _samplesSinceZUpdate(0),
		_v8(DACVoltage::xcodeForMV(8 * 1000))

	{
	}

	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	//virtual void go(int x, int y, int z, volatile int& a, volatile int&b)
	{
       
#ifdef _TESTACDPERF
        // Force and update at SR/10 (which is close to adc update rate))
        ++_hack;
        if ((_hack > 10) || (_hack < 0))
        {
            z += 1;
            _hack = 0;
        }
#endif
        
        // becuase pot update performance is not so good update at SR /100)
        if (++_samplesSinceZUpdate > 100)
        {
            setz(z);
            _samplesSinceZUpdate = 0;
        }
		_sd1.go(x);
		_sd2.go(y);
		a = _sd1.getBothSlope() ? _v8 : 0;
		b = _sd2.getBothSlope() ? _v8 : 0;
	}
private:
	typedef ExpPotFilterLookup::fp fp;
	// (int thresholdMillivolts, int tcMs, int sampleRate)
	SlopeDetector _sd1;
	SlopeDetector _sd2;
	int _lastZ;
    int _samplesSinceZUpdate;
  
#ifdef _TESTACDPERF
    int _hack;
#endif

	const int _v8 ;
	//ExpPotLookup _scalePot_to_us;		// log taper, ourput micorseocnd tc
	ExpPotFilterLookup _scalePot_to_k;  // log taper, ourput k filter coeff

	// take output of the pos scaler, which happens to be in microseconds
	void setSens(fp sens)
	{
		//assert(sens > 0);
		//printf("DMSlopeDet setting tcus to %d\n", sens);
		_sd1.setTC_coeffDirect(sens);
		_sd2.setTC_coeffDirect(sens);
	//	i_global = sens;
	}

	void setz(int z)
	{
        if (z == _lastZ) return;
    
        const fp k = _scalePot_to_k.lookup(z);
		//printf("in slope det m z=%d f = %f\n", z, f);


		setSens(k);
		_lastZ = z;
	}
};
#endif


#endif
