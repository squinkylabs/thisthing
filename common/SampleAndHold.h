

#ifndef INCLUDE_SAMPLEANDHOLD
#define INCLUDE_SAMPLEANDHOLD

/* basic sample and hold building block
 */
class SampleAndHold
{
public:
	SampleAndHold() : _track(0), _sample(0) {}

	void go(bool gate, bool trigger, int x)
	{
		if (gate)
		{
			_track = x;
		}
		if (trigger)
		{
			_sample = x;
		}
		//printf("sh.go with g=%d t=%d, now mt=%d mh=%d\n", gate, trigger, _track, _sample);
	}

	void reset()
	{
		//printf("sh.reset setting to zero\n");
		_track = 0;
		_sample = 0;
	}

	int sampleAndHold() const
	{
		return _sample;
	}
	int trackAndHold() const
	{
		return _track;
	}
private:
	int		_track;
	int		_sample;
};

#endif