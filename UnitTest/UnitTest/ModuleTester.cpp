
#include "stdafx.h"

#include "ModuleTester.h"

bool ModuleTester::run() const
{
	for (const_iterator it = _entries.begin(); it != _entries.end(); ++it)
	{
		bool b = run(*it);
		if (!b) return false;

	}
	return true;
}

bool ModuleTester::run(const Entry& entry) const
{
	int time = entry.in.time;
	assert(time > 0);

	int a=0, b=0;
	for ( ; time > 0; --time)
	{
		_dm.go(false, entry.in.x, entry.in.y, entry.in.z, a, b);
	}

	return entry.cond.eval(a, b);
}


bool MTCond::eval(int a, int b) const
	{
		switch (_cond)
		{
		case C_ABEQ:
			{
				bool ret = a==_a && b==_b;
				if (!ret) printf("eval C_ABEQ failing on %d,%d, expect %d,%d\n", a, b, _a, _b);
				return ret;
			}
			break;
		case C_NONE:
			return true;
		default:
			printf("MTCond has no handler for %d\n", _cond);
			return false;
		}
	}
