#ifndef INCLUDE_COUNTER
#define INCLUDE_COUNTER

class Counter
{
public:
	Counter() : _period(1), _ct(0) {}
	void clock()
	{
		++_ct;
		if (_ct >= _period)
			_ct = 0;
		//printf("after clock, ct=%d\n", _ct);
	}
	bool get()
	{
		//printf("in get, _ct = %d\n", _ct);
		return _ct == 0;
	}
	void setPeriod(int p)
	{
		_period = p;
	}
private:
	int _period;
	int _ct;
};

#endif