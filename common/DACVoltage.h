#ifndef INCLUDE_DACVOLTAGE
#define INCLUDE_DACVOLTAGE

#include <stdint.h>
#include "Constants.h"


/* Utilities for converting integers to voltaages and back
 * assumes the disting calibration  convertion for the 24bit (x, y, a, b) i/o,
 * and that is 1V = 1 << 19
 *
 * for 10 bit z, assumes 0x3ff is 10v
 */
class DACVoltage
{
public:

	static const int max_10z = 0x3ff;
	static const int zero_10z = 0x200;

	static const int code_voltx =  (1 << 19);
	static int xcodeForMV(int mv)
	{
		// probably don't need 64?
		int64_t temp = (int64_t )mv * code_voltx;
		temp /= 1000;
		return (int) temp;
	}
	static int xcodeForuV(int uv)
	{
		int64_t temp = (int64_t )uv * code_voltx;
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
		int64_t x = max_10z;		// code for 10v
		x *= mv;			// code for mv * 10v
		x /= 20000;
		return (int)x + zero_10z;
	}

	static int zMVFromCode(int code)
	{
		int x = code;
		x -= zero_10z;
		x *= 20000;
		x /= max_10z;
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

