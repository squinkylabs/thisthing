#ifndef MODULETESTER
#define MODULETESTER

#include "DModule.h"


// set of inputs for a test
class MTIn
{
public:
	/* Static factories
	 */
	static MTIn xy(int x, int y) { MTIn ret; ret._x = x, ret._y = y; return ret; }
private:
	MTIn() : _x(0), _y(0), _time(1) {}
	int _x;
	int _y;
	ZState _z;
	int _time;		// duration of this input set
};

enum Conds { C_NONE, C_XEQ, C_YEQ, C_XYEQ } ;
// condition to test
class MTCond
{
public:

	/* Static factories
	 */

	// check x and y for equal
	static MTCond xy(int x, int y) { MTCond ret; ret._cond = C_XYEQ; ret._x = x; ret._y=y; return ret; }
	// null constructor
	MTCond() : _cond(C_NONE)
	{}

	bool eval();
private:
	Conds _cond;
	int _x;
	int _y;
};



class ModuleTester
{
public:
	ModuleTester(class DModule& dm) : _dm(dm) {}

	void run();
	/* Add an input with no test
	 */
	void add(MTIn);

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
		Entry(const MTIn& i , const MTCond& c) : _in(i), _cond(c) {}
		MTIn _in;
		MTCond _cond;
	};

	std::vector<Entry> _entries;
	class DModule & _dm;
};


#endif