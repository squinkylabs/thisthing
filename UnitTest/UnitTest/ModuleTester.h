#ifndef MODULETESTER
#define MODULETESTER

#include "DModule.h"
#include "LinearInterp.h"


// set of inputs for a test
class MTIn
{
public:
	/* Static factories
	 */

	// specify x and y, don't care z
	static MTIn xy(int x, int y) { MTIn ret; ret.x = x, ret.y = y; return ret; }

	static MTIn z_interp(int range0, int range1, int step)
	{ 
		MTIn ret;
		//int zval = 0x3ff * step / steps;
		LinearInterp li(range0, range1);
		int zval = li._inverse(step);
		ret.z = ZState(zval, true);
		return ret;
	}
	

	int x;
	int y;
	ZState z;
	int time;		// duration of this input set
private:
	MTIn() : x(0), y(0), time(1), z(0, false) {}
	
};

enum Conds { C_NONE, C_AEQ, C_BEQ, C_ABEQ } ;
// condition to test
class MTCond
{
public:

	/* Static factories
	 */

	// check x and y for equal
	static MTCond ab(int a, int b) { MTCond ret; ret._cond = C_ABEQ; ret._a = a; ret._b=b; return ret; }

	static MTCond none() { return MTCond(); }
	// null constructor
	MTCond() : _cond(C_NONE)
	{}

	bool eval(int a, int b) const;
private:
	Conds _cond;
	int _a;
	int _b;
};



class ModuleTester
{
public:
	ModuleTester(class DModule& dm) : _dm(dm) {}

	bool run() const;
	/* Add an input with no test
	 */
	void add(MTIn in) { add(in, MTCond()); }

	/* Add an input and a test
	 */
	void add(MTIn in, MTCond vc) 
	{
		_entries.push_back( Entry(in, vc));
	}
private:
	
	class Entry
	{
	public:
		Entry(const MTIn& i , const MTCond& c) : in(i), cond(c) {}
		MTIn in;
		MTCond cond;
	};

	typedef std::vector<Entry> container;
	typedef container::iterator iterator;
	typedef container::const_iterator const_iterator;
	container _entries;
	class DModule & _dm;

	bool run(const Entry&) const;
};


#endif