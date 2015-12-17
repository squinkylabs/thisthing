
#ifndef INCLUDE_DMRANDOMSOURCE1
#define INCLUDE_DMRANDOMSOURCE1

#include "LFO.h"
#include "ExpPot.h"
#include "DModule.h"


class DMRandomSource1 : public DModule
{
public:
	DMRandomSource1(int sampleRate) : 
		_lastZ(-1),
		_sr(sampleRate),
		_scale(.2f, 10)
	{
		setz(2);
	}
	void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	 {
		if (z.changed)
		{
			setz(z.value);
		}
		_a.go();
		_b.go();
		_c.go();
		_d.go();
		int aTemp = ( _a.getTri() >> 2) +
			( _b.getTri() >> 2) +
			( _c.getTri() >> 2) +
			( _d.getTri() >> 2);

		int bTemp = - ( _a.getTri() >> 2) - 
			( _b.getTri() >> 2) +
			( _c.getTri() >> 2) -
			( _d.getTri() >> 2);
		a = aTemp >> 10;
		b = bTemp >> 10;
	 }
	 void setFreq(double f)
	 {

#if 1   // values from my old plugin
         _a.set(f / 1.1, _sr);
		_b.set(f * 1, _sr);
		_c.set(f * 1.24, _sr);
		_d.set(f * 1.32, _sr);
#else
		_a.set(f, sr);
		_b.set(f * 1.3, sr);
		_c.set(f * 2.1, sr);
		_d.set(f * 3.8, sr);
#endif
	 }
private:
	double _lastZ;
	const double _sr;
	LFO _a;
	LFO _b;
	LFO _c;
	LFO _d;
	ExpPotLookup _scale;

	// TODO: get rid of this crazy stuff
	void setz(int z)
	{
        if (z == _lastZ) return;
    
        const float f = _scale.map(z);
		setFreq(f);
		_lastZ = z;
	}
};


class DMRandomSource2 : public DModule
{
public:
	DMRandomSource2(int sampleRate) : 
		_lastZ(-1),
		_sr(sampleRate),
		_scale(.1f, 20)
	{
		setz(2);
		_a.set(.44, _sr);
		_b.set(.3, _sr);
		_c.set(.66, _sr);
		_d.set(2, _sr);

	}
	 void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	 {
		 assert(false);
		 setz(z.value);
		_a.go();
		_b.go();
		_c.go();
		_d.go();
		int aTemp = ( _a.getTri() >> 2) +
			( _b.getTri() >> 2) +
			( _c.getTri() >> 2) +
			( _d.getTri() >> 2);

		int bTemp = - ( _a.getTri() >> 2) -
			( _b.getTri() >> 2) +
			( _c.getTri() >> 2) -
			( _d.getTri() >> 2);
		a = aTemp >> 10;
		b = bTemp >> 10;
	 }
	 void setFreq(double f)
	 {
         _d.set(f, _sr);
	 }
private:
	double _lastZ;
	const double _sr;
	LFO _a;
	LFO _b;
	LFO _c;
	LFO _d;
	ExpPotLookup _scale;

	void setz(int z)
	{
		if (z == _lastZ) return;
      
        const float f = _scale.map(z);
		setFreq(f);
		_lastZ = z;
	}
};


class DMLFO : public DModule
{
public:
	DMLFO(int sampleRate) : 
		_lastZ(-1),
		_sr(sampleRate),
		_scale(.5f, 10)
	{
		setz(2);
       // _a.set(1, _sr);
	}
	 void go(int x, int y, int z, volatile int& a, volatile int& b)
	 {
		 setz(z);
		_a.go();
		a = _a.getTri() >> 10;
		b = a;
	 }
	 void setFreq(double f)
	 {
         _a.set(f, _sr);
	 }
private:
	double _lastZ;
	const double _sr;
	LFO _a;
	ExpPotLookup _scale;

	void setz(int z)
	{
		if (z == _lastZ) return;
      
        const float f = _scale.map(z);
		setFreq(f);
		_lastZ = z;
	}
};

#endif
