
#ifndef INCLUDE_LFO
#define INCLUDE_LFO
class LFO
{
public:
	LFO() : _phase(0), _phaseInc(0) {}
	void set( double fc, double fs) 
	{
		double totalPhase = (double )0xffffffff + 1.0;
		_phaseInc =  (unsigned int)((fc / fs) * totalPhase); 
	}
	void go() 
	{
		_phase += _phaseInc;
	}
	int getTri()		//triangle, full range of int
	{
		int ret = _phase;
		if (ret <0)
		{
			ret = -ret;
		}
		// now tri 0 ... max int
		ret -= 0x40000000;
		ret *= 2;
		return ret;
	}
	// get an attenuated that's less than 2 volts
	int getTri2v()
	{
		return getTri() >> 11;
	}
private:
	unsigned int _phase;
	unsigned int _phaseInc;
	
};
#endif

