#ifndef INCLUDE_DMQUANTIZER
#define INCLUDE_DMQUANTIZER

#include "DiatonicPitchMapper.h"
#include "DModule.h"
// x = cv in
// a = quantized cv out
class DMDiatonicQuantizer : public DModule
{
public:
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		_q.go(x);
		int midi = _q.getMIDI();

		midi = DiatonicPitchMapper::map(midi);

		a = ChromaticQuantizer::midi2CV(midi);
		midi += 3;			// transpose a up minor third
		b = ChromaticQuantizer::midi2CV(midi);		// and quantize to maj/min third
	}
private:
	ChromaticQuantizer _q;
};
#endif
