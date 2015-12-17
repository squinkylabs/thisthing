
#ifndef INCLUDE_DMADDERS
#define INCLUDE_DMADDERS

#include "DiscreteLookup.h"
#include "LinearInterp.h"
#include "DModule.h"
#include "DACVoltage.h"
#include "Quantizer.h"

class DMOctaveAdderSubtractor : public DModule
{
public:
	DMOctaveAdderSubtractor() : _octave(0)
	{
		_lookup.setValue(0, DACVoltage::xcodeForMV(-5000));
		_lookup.setValue(1, DACVoltage::xcodeForMV(-4000));
		_lookup.setValue(2, DACVoltage::xcodeForMV(-3000));
		_lookup.setValue(3, DACVoltage::xcodeForMV(-2000));
		_lookup.setValue(4, DACVoltage::xcodeForMV(-1000));
		_lookup.setValue(5, DACVoltage::xcodeForMV(0));
		_lookup.setValue(6, DACVoltage::xcodeForMV(1000));
		_lookup.setValue(7, DACVoltage::xcodeForMV(2000));
		_lookup.setValue(8, DACVoltage::xcodeForMV(3000));
		_lookup.setValue(9, DACVoltage::xcodeForMV(4000));
		_lookup.setValue(10, DACVoltage::xcodeForMV(5000));
		_lookup.setStepNumber(5);
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			_octave = 0;		// do we need to reset _lookup?
		}
		if (z.changed)
		{
			_octave = _lookup.process(z.value);
			Led_setTempSelectorOverride(1 + _lookup.getStepNumber(), 1);
		}
		//printf("adder: z = %d oct = %d\n", z, oct);
		a = x + y + _octave;
		b = x + _octave - y;
	}
private:
	DiscreteLookup<11> _lookup;
	int _octave;
};


//z knob selects  13 values - 0, 1 semitone, 2 semitones, to 1 octave
//
//x = CV in
//y = CV in
//
//a = x + y + z
//b = x + y - z

class DMSemitoneAdderSubtractor : public DModule
{
public:
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			_offset = 0;
		}
		if (z.changed)
		{
			_offset = _lookup.process(z.value);
			Led_setTempSelectorOverride( _lookup.getStepNumber(), 1);
		}
		//printf("semi adder: z = %d oct = %d\n", z, off);
		a = x + y + _offset;
		b = x + y - _offset;
		
	}
	DMSemitoneAdderSubtractor() : _offset(0)
	{
		for (int i=0; i< 13; ++i)
		{
			_lookup.setValue(i, DACVoltage::xcodeForuV(i * 1000 * 1000 / 12));
		}
		_lookup.setStepNumber(0);
	}
private:
	DiscreteLookup<13> _lookup;
	int _offset;
	
};


/*
z knob smoothly moves (linearly) from 0v to 1v

x = CV in
y = CV in (quantized to scale - tonic, 2nd, minor 3rd, major 3rd, 4th, 5th )

a = x + y + z
b = x + y - z
*/



class FMPitchMapper
{
public:
	static int map(int midiIn)
	{
	//	int octave = (midiIn - middleCMIDI) / 12;
		int octave, semi;
		DiatonicPitchMapper::separate(midiIn, octave, semi);
		
		int ret = midiIn;
		switch(semi)
		{
		case 0:			// c-> c
		case 2:			// d->d
		case 3:			// d# -> e
		case 4:			// e -> e;
		case 5:			// f -> f
		case 7:			// g -> g
			ret = midiIn;
			break;
		case 1:			// c# -> d
		case 6:			// f# -> g
		case 11:		// b->c
			ret = midiIn+1;
			break;
		case 8:			//  g# -> g
			ret = midiIn-1; 
			break;
		case 9:			//  a -> c
			ret = midiIn+3; 
			break;
		case 10:		// a# -> c
			ret = midiIn+2;
			break;
		default:
			assert(0);
		}
		return ret;
	}
};


class DMFPrecisionFMHelper : public DModule
{
public:
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset)
		{
			_offset = 0;
		}
		_q.go(y);
		int midi = _q.getMIDI();


		midi = FMPitchMapper::map(midi);

		int qy = ChromaticQuantizer::midi2CV(midi);

		if (z.changed)
		{
			_offset = (int)(z.value * _a);
			Led_setTempSelectorOverride(_led.interp(z.value), 1);
		}

		a= x + qy + _offset;
		b = x + qy - _offset;
	}

	DMFPrecisionFMHelper() : _a((double)DACVoltage::xcodeForMV(1000) / (double)0x3ff),
		_offset(0)
	{

	}
private:
	// TODO: fp
	const double _a;
	ChromaticQuantizer _q;
	int _offset;

	InterpForLED _led;
};



#endif