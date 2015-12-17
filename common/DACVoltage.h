#ifndef INCLUDE_DACVOLTAGE
#define INCLUDE_DACVOLTAGE

#include <stdint.h>
#include "Constants.h"



// version that does not use disting calibration. for this one full scale is 10 volts
class DACVoltage
{
public:

	static const int max_10 = 0x3ff;
	static const int zero_10 = 0x200;



	static const int code_volt =  (1 << 19);
	static int xcodeForMV(int mv)
	{
		// probably don't need 64?
		int64_t temp = (int64_t )mv * code_volt;
		temp /= 1000;
		return (int) temp;
	}
	static int xcodeForuV(int uv)
	{
		int64_t temp = (int64_t )uv * code_volt;
		temp /= (1000 * 1000);
		return (int) temp;
	}

	static int xcodeForGateHi()
	{
		return xcodeForMV(Constants::GATE_OUT_MAX_MV);
	}
	// return the 10 bit integer that will output the given voltage 
	// from the 10 bit z pot/dace
	static int zcodeForMV(int mv)
	{
		int64_t x = max_10;		// code for 10v
		x *= mv;			// code for mv * 10v
		x /= 20000;
		return (int)x + zero_10;
	}

	static int zMVFromCode(int code)
	{
		int x = code;
		x -= zero_10;
		x *= 20000;
		x /= max_10;
		return x;
	}

	static int codeForMV(bool useZ, int mv)
	{
		return useZ ? zcodeForMV(mv) : xcodeForMV(mv);
	}


	// some observed values
	static int zcodeMax() { return 930; }
	static int zcodeMin() { return 157; }

};
#endif

