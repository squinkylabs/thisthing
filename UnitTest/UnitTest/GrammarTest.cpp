
#include "stdafx.h"


class ProductionRule
{
public:
	void evaluate()
	{
	}
};


ProductionRule rules[] =
{
	ProductionRule()
};


void gt0()
{
	rules[0].evaluate();
}
void GrammarTest()
{
	gt0();
}

