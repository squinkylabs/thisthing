
#ifndef DMSCALEQUANTIZER
#define  DMSCALEQUANTIZER

#include "OctaveScaleManager.h"
#include "DModule.h"
#include "LinearInterp.h"
#include "GateTrigger.h"

/* X = CV
 * y = trigger
 * z = scale selector
 */
class DMScaleQuantizer1 : public DModule
{
public:
	DMScaleQuantizer1() : 
	  _interp(0, OctaveScaleManager::getNumOctaveScales()),
	 _trigger(false)
	{
		_reset();
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset) {
			_reset();
			return;
		}
		if (z.changed)
		{
			int index = _interp.interp(z.value);
			Led_setTempSelectorOverride(1 + index, 1);
			_scales.select(index);
		}
		_trigger.go(y);
		if (_trigger.trigger())
		{
			// quantize pitch to midi note number
			_chromaticQuantizer.go(x);
			int pitch = _chromaticQuantizer.getMIDI();

			// then quantize to our scale
			char len;
			const char * scale = _scales.get(len);
			char q = ScaleQuantizer::quantize_expanded(pitch,scale , len);

			// and back to CV
			_qv = ChromaticQuantizer::midi2CV(q);
		}
		a = b = _qv;
	}

private:
	OctaveScaleManager _scales;
	ChromaticQuantizer _chromaticQuantizer;
	LinearInterp _interp;
	GateTrigger _trigger;
	int _qv;

	void _reset()
	{
		_qv=0;
		_trigger.reset();
		_chromaticQuantizer.reset();
	}
};

/* X = CV
 * y = trigger
 * z = shift
 */
class DMScaleQuantizer2 : public DModule
{
public:
	DMScaleQuantizer2() : 
	  _interpShift(0,11),
	 _trigger(false)
	{
		_reset();
	}
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)
	{
		if (reset) 
		{
			_reset();
			return;
		}
		if (z.changed)
		{
			int shift = _interpShift.interp(z.value);
			Led_setTempSelectorOverride(1 + shift, 1);
			//printf("shift now %d z=%d\n", shift, z.value);
			_scales.selectAndShift(2, shift);
		}
		_trigger.go(y);
		if (_trigger.trigger())
		{
			// quantize pitch to midi note number
			_chromaticQuantizer.go(x);
			int pitch = _chromaticQuantizer.getMIDI();

			// then quantize to our scale
			char len;
			const char * scale = _scales.get(len);
			char q = ScaleQuantizer::quantize_expanded(pitch,scale , len);

			// and back to CV
			_qv = ChromaticQuantizer::midi2CV(q);
		}
		a = b = _qv;
	}

private:
	OctaveScaleManager _scales;
	ChromaticQuantizer _chromaticQuantizer;
	LinearInterp _interpShift;
	GateTrigger _trigger;
	int _qv;

	void _reset()
	{
		_qv=0;
		_trigger.reset();
		_chromaticQuantizer.reset();
	}
};
#endif