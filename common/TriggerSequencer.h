#ifndef TRIGGERSEQUENCER
#define TRIGGERSEQUENCER

class TriggerSequencer
{
public:
	enum EVT {
		TRIGGER,
		LOOP,
		END
	};
	class Event
	{
	public:
		short int evt;
		short int delay;
	};

	// data is a buffer that will be bound to the sequencer, although it is "owned"
	// by the caller
	TriggerSequencer(const Event * data) : events(data), curEvent(data)
	{
		delay = data->delay;
	}

	// send one clock to the seq, return true if trig should be generated
	bool clock();


private:
	const Event * const events;
	const Event * curEvent;
	short delay;
};

inline bool TriggerSequencer::clock()
{
	printf("enter clock, curevt =%p, delay = %d\n", curEvent, delay);
	if (!curEvent) return false;	// seq is stopped

	bool ret = false;
	if (delay)
		--delay;

	while (!delay)
	{
		printf("delay went to zero, evt=%d\n", curEvent->evt);
		switch (curEvent->evt)
		{
		case END:
			curEvent = 0;		// stop seq by clering ptr
			return ret;
		case TRIGGER:
			ret = true;
			++curEvent;			// and go to next one
			printf("trigger set true\n");
			break;
		case LOOP:
			curEvent = events;
			break;
		default:
			assert(false);
		}

		delay = curEvent->delay;
	}
	return ret;
};



#endif
