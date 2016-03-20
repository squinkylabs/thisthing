#ifndef GENERATIVETRIGGERGENERATOR
#define GENERATIVETRIGGERGENERATOR

#include "StochasticGrammar.h"
#include "TriggerSequencer.h"

/* Knows how to generate trigger sequence data
 * when evaluating a grammar
 */
class GTGEvaluator : public ProductionRule::EvaluationState
{
public:
	GTGEvaluator(Random& xr, TriggerSequencer::Event * buf) :
		ProductionRule::EvaluationState(xr),
		_buf(buf),
		_delay(0)
	  {
	  }

	virtual void writeSymbol(GKEY key)
	{
		// first: write out a trigger at "current delay"
		_buf->evt = TriggerSequencer::TRIGGER;
		_buf->delay = _delay;
		++_buf;

		// then set current dealy to duration of key
		_delay =  ProductionRuleKeys::getDuration(key);
	}

	// call this to write final event
	void writeEnd()
	{
		_buf->evt = TriggerSequencer::END;
		_buf->delay = _delay;
	}
private:
	TriggerSequencer::Event * _buf;
	int _delay;
};


/* wraps up some stochastic gnerative grammar stuff feeding
 * a trigger sequencer
 */
class GenerativeTriggerGenerator
{
public:
	GenerativeTriggerGenerator(const ProductionRule * rules, int numRules,  GKEY initialState) :
		_rules(rules),
		_numRules(numRules),
		_initKey(initialState)
	{
		_data[0].delay=0;
		_data[0].evt = TriggerSequencer::END;
		_seq = new TriggerSequencer(_data);
	}
	~GenerativeTriggerGenerator()
	{
		delete _seq;
	}

	// returns true if trigger generated
	bool clock()
	{
		_seq->clock();
		bool ret = _seq->getTrigger();
		if (_seq->getEnd())
		{
			// when we finish playing the seq, generate a new random one
			generate();
			ret |= _seq->getEnd();
		}
		return ret;
	}
private:
	TriggerSequencer * _seq;
	TriggerSequencer::Event _data[33];
	const ProductionRule * _rules;
	Random _r;
	const int _numRules;
	const GKEY _initKey;
	//
	void generate()
	{
		GTGEvaluator es(_r, _data);
		es.rules = _rules;
		es.numRules = _numRules;
		ProductionRule::evaluate(es, _initKey);

		es.writeEnd();
		TriggerSequencer::isValid(_data);
		_seq->reset(_data);
		assert(!_seq->getEnd());
	}
};
#endif