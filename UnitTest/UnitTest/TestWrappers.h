
#ifndef TESTWRAPPERS_H
#define  TESTWRAPPERS_H


#include "SlopeDetector.h"
#include "DModule.h"

class I_Test
{
public:
	virtual int doIt(int)=0;
};

class SDWrap : public I_Test
{
public:
	SDWrap(SlopeDetector& s) : _sd(s) {}
	int doIt(int input)
	{
		_sd.go(input);
		return _sd.getBothSlope();
	}
private:
	SlopeDetector _sd;
};

class SDMWrap  : public I_Test
{
public:
	SDMWrap(DModule& s, int zValue=0) : _sd(s), _z(zValue) {}
	int doIt(int input)
	{
		//_sd.go(int x, int y, int z, volatile int& a, volatile int&b)=0;
		int a, b;

		// the modules have an lpf....
		//const int filter_delay = 30;
		//for (int i=0; i<filter_delay; ++i)
		ZState z(_z, true);

		_sd.go(false, input, input, z, a, b);
		assert(a == b);
	
		return a;
	}
private:
	DModule& _sd;
	const int _z;
};
#endif