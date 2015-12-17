

#ifndef INCLUDE_SCHMIDTTRIGGER
#define INCLUDE_SCHMIDTTRIGGER

class SchmidtTrigger
{
public:
	SchmidtTrigger(int thLo, int thHi) : _thLo(thLo), _thHi(thHi), _lastOut(false)
	{
	}
	bool go(int input)
	{
		if (_lastOut)		// if we were true last time
		{
			if (input < _thLo)
			{
				_lastOut = false;
			}
		}
		else
		{
			if (input > _thHi)
			{
				_lastOut = true;
			}
		}
		return _lastOut;
	}

	int thhi() const { return _thHi; }
	int thlo() const { return _thLo; }
private:
	const int _thLo;
	const int _thHi;
	bool	_lastOut;
};

#endif

