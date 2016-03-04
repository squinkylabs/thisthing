
#include "stdafx.h"
#include "ModuleTester.h"
#include "DMMisc.h"


static void mt0()
{
	DMConst dm(55,66);
	ModuleTester mt(dm);

	MTIn in = MTIn::xy(3,4);
	MTCond co = MTCond::xy(55, 66);

	mt.add(in, co);

	mt.run();
	
	assert(false);
}

void ModuleTesterTest()
{
	mt0();
}