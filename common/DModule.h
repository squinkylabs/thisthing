/** Base class for disting modules
currently not used, since virtual functions require malloc
**/

#ifndef INCLUDE_DMODULE
#define INCLUDE_DMODULE

extern "C" void Led_setTempSelectorOverride(int value, int mode);

class ZState
{
public:
	ZState(int val, bool changed) : value(val), changed(changed) {}
	explicit ZState(int val) : value(val), changed(true) {}
	ZState() : value(0), changed(true) {}

	int value;
	bool changed;	// if this is false, should not act on zValue
};


class DModule
{
public:
	virtual void go(bool reset, int x, int y, const ZState& z, volatile int& a, volatile int&b)=0;
protected:
};
#endif
