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
	TriggerSequencer(const Event * data) : events(data) {}

	// send one clock to the seq, return true if trig should be generated
	bool clock() { return false; }


private:
	const Event * events;
};

#endif
