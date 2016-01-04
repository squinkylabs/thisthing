
#ifndef INCLUDE_CONSTANTS
#define INCLUDE_CONSTANTS

namespace Constants
{
	const double PI =       3.14159265358979323846;
	const double E = 2.71828182845904523536;

	const int SAMPLE_RATE_DIVISOR = 2;
	const int DISTING_SAMPLE_RATE = 78125 / SAMPLE_RATE_DIVISOR ;  // 78.125kHz
	const int GATE_OUT_MAX_MV = 8000;
	const int TRIGGER_OUT_TIME_MS = 5;
	const int INVERSION_VOLTAGE = 8;		// We will invert inverted outputs around 8v

	const int GIBBS_CUTOFF_RATIO = 128 / SAMPLE_RATE_DIVISOR;		// fc / fs = 1 / GIBBS_CUTOFF_RATIO. was 32 for 3.5 release 
											// at 16 it miss-triggers all the time. (rise and fall)
											// 128 will pass unit tests
											// 256 WILL PASS
											// 512 FAILS


	const int SLOPE_TEST_DELAY = GIBBS_CUTOFF_RATIO / 3;		// was 10 for gibbs = 32
}
#endif 