
#ifndef DMSCALEQUANTIZER
#define  DMSCALEQUANTIZER

#include "ScaleQuantizeAndRotate.h"
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
	  _interp(0, OctaveScaleManager::getNumOctaveScales()-1),
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
 * z = shift-broken version
 * 
 * this shift based on a bad reading of the specs
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
			int rotation = _interpShift.interp(z.value);
			Led_setTempSelectorOverride(1 + rotation, 1);
			//printf("shift now %d z=%d\n", shift, z.value);
			_scales.selectAndRotate(2, rotation);
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



/* X = CV
 * y = trigger
 * z = rotate-LC version
 * 
 */
class DMScaleQuantizer3 : public DModule
{
public:
	DMScaleQuantizer3() : 
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
			_rotation = _interpShift.interp(z.value);
			Led_setTempSelectorOverride(1 + _rotation, 1);
		}
		_trigger.go(y);
		if (_trigger.trigger())
		{
			// quantize pitch to midi note number
			_chromaticQuantizer.go(x);
			int pitch = _chromaticQuantizer.getMIDI();

			// qunatize to scale and rotat
			char len;
			const char * expanded = _scales.get(len);
			int q = ScaleQuantizeAndRotate::quantizeAndRotate(len, expanded, _rotation, pitch);

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
	int _rotation;

	void _reset()
	{
		_qv=0;
		_rotation=0;
		_trigger.reset();
		_chromaticQuantizer.reset();
		_scales.select(2);
	}
};
#endif