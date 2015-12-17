
#include "stdafx.h"


/* TRY3 - This acutally works



The model:

vin-->X---->+--->ADC
      |     |    
	  k2    k1

DAC--->*--->+----> vout
       |    |
	   k4   k3

The problem: solve for k1, k2, k3, k4

1) ground input, measure R0 = ADC reading

0-->X---->+--->ADC:R0
    |     |    
    k2    k1
->k1 = r0



2) output zero to dac, run output to input, measure R1 = ADC

DAC=0 --->*--->+----> vout/vin -->X--->+----> ADC:R1
         |    |                  |     |
	     k4   k3                 k2    k1
-> k3 * k2 + k1 = R1

3) output half scale, feed back to input

DAC=H --->*--->+----> vout/vin -->X--->+----> ADC:R2
         |    |                  |     |
	     k4   k3                 k2    k1
-> (H * k4 + k3) * k2 + k1 = R2

4) 3V reference into ADC

  3V-->X---->+--->ADC:R3
       |     |    
	   k2    k1
->3.0 * k2 + k1 = R3


form 1: k1=R0
from 4) 
	3 * k2 + k1 = R3
	3k2 = (r3 - k1)
	// subst k1 from 1)
	3k2 = (r3 - r0)
	k2 = (r3 - r0)/3.0
now we know k1 & k2

from 2)
	 k3 * k2 + k1 = R1
	 k3 * k2 = (R1 - k1)
	 k3 = (R1 - k1) / k2
now we know k1, 2, 3

from 3)
	(H * k4 + k3) * k2 + k1 = R2
	(H * k4 + k3) * k2  = (R2 - k1)
	(H * k4 + k3)   = (R2 - k1) / k2
	H * k4    = ((R2 - k1) / k2) - k3
	k4 = (1/H) * (((R2 - k1) / k2) - k3)



*/

/* TRY1 THIS DID NOW WORK

The model:

vin-->X---->+--->ADC
      |     |    
	  k2    k1

DAC--->+--->*----> vout
       |    |
	   k3   k4

The problem: solve for k1, k2, k3, k4

1) ground input, measure R0 = ADC reading

0-->X---->+--->ADC:R0
    |     |    
    k2    k1

2) output zero to dac, run output to input, measure R1 = ADC

DAC=0 --->+--->*----> vout/vin -->X--->+----> ADC:R1
         |    |                  |     |
	     k3   k4                 k2    k1

3) output half scale, feed back to input

DAC=H --->+--->*----> vout/vin -->X--->+----> ADC:R2
         |    |                  |     |
	     k3   k4                 k2    k1

4) 3V reference into ADC

  3V-->X---->+--->ADC:R3
       |     |    
	   k2    k1



from 1) k1 = r0

from 4) 
	3 * k2 + k1 = R3
	3k2 = (r3 - k1)
	// subst k1 from 1)
	3k2 = (r3 - r0)
	k2 = (r3 - r0)/3

now we know k1 and k2

2)
   k3* k4 * k2 + k1 = R1  
   solve for k4:
   k3*k4*k2 = R1 - k1
   k4 = (R1 - k1) / (k3*k2)

3) let H = 0x400000 (half scale postitive)

   (H + k3) * k4 * k2 + k1 = R2
   solve for k3
   (H + k3) * k4 * k2 = (R2 - k1)
   (H + k3) =  (R2 - k1) / ( k4 * k2)
   k3 =  -H + (R2 - k1) / ( k4 * k2)


using 2), substitute for k4 in 3)
	k3 =  -H + (R2 - K1) / ( k4 * k2)
	k3 =  -H + (R2 - K1) / ( k2 * k4)
	k3 =  -H + (R2 - K1) / ( k2 *(R1 - k1) / (k3*k2))
	now solve for k3
	k3 =  -H + (R2 - K1) (k3*k2) / ( k2 *(R1 - k1) / 1)
	k3 =  -H + (R2 - K1) (k3*k2) / ( k2 *(R1 - k1))
	k3 =  -H + (R2 - K1) (k3) / (R1 - k1)
	k3  (R1 - k1) =   -H + (R2 - K1) (k3)
	k3*R1 - k1*k3 = -H + k3*R2 - K1*k3
	k3*R1  = -H + k3*R2 
	k3 (R1 -R2) = -H 
	k3 = -H / (R1 - R2)

Again using 2), substitute for k4 in 3)
	k3 =  -H + (R2 - K1) / ( k4 * k2)
	k3 =  -H + (R2 - K1) / ( k2 * k4)

	k3 =  -H +  (R2 - k1)
	            ---------
				(k2 * k4)

	k3 =  -H +  (R2 - k1)       1
	            --------- * -------
				(k2 )          k4

	  k4 = (R1 - k1) / (k3*k2)
	 
	k3 =  -H +  (R2 - k1)       (k3 * k2)
	            ---------   *  -----------
				(k2 )            (R1 - k1)

  k3 =  -H +  (R2 - k1)       k3 
	            ---------   *  -----------
				           (R1 - k1)

	k3 (R1 - k1) = -H + (R2 - k1) * k3;

	k3* (R1 - k1) = -H + R2 * k3 - k1 * k3

	k3(R1 - k1 - R2 + k1) = -H
	k3(R1 - R2 ) = -H
	k3 = -H / (R1 - R2)



*/

//LEFT channel
const int R[] = { -43221,
 -41412,
 3742338,
 2379383};

#if 0
double k1=0;
double k2=0;
double k3=0;
double k4=0;
#endif

double H = 0x400000;

double oneV = 1<<19;

#if 0
void try1()
{
	printf("try1\n");
	// 1)
	k1 = R[0];

	// 4)
	k2 = (double)(R[3] - R[0]) / 3.0;

	// first try
	k3 = -H / (R[1] - R[2]);

	

	k4 = (R[1] - k1) / (k3*k2);

	printf("k1=%f, k2=%f, k3=%f k4 = %f\n", k1, k2, k3, k4);
	printf("1v=%f 1/k4=%f\n", oneV, 1.0 / k4);

	double fullScale = 0x7fffff;
	printf("full=%f, h=%f h/f = %f\n", fullScale, H, H/fullScale);

	/*
DAC--->+--->*----> vout
       |    |
	   k3   k4
	   */
	printf("out v for oneV = %f\n", (oneV + k3) * k4);

}

#endif

void try2()
{
	double k2 = R[3] / 3.0;
	double k4 = (double)R[2] * 3.0 / (H * (double) R[3]);
	printf("try 2 (no offsets) k2 = %f, k4 = %f 1/k4=%f\n", k2, k4, 1.0 / k4);
}


void try3()
{
	printf("try3\n");
// 1)
	double k1 = R[0];

	// 4)
	double k2 = (double)(R[3] - R[0]) / 3.0;

	// first try
	//k3 = -H / (R[1] - R[2]);
	double k3 = ((double)R[1] - k1) / k2;

	

	//k4 = (R[1] - k1) / (k3*k2);
	double k4 = (1/H) * ((((double)R[2] - k1) / k2) - k3);

	printf("k1=%f, k2=%f, k3=%f k4 = %f\n", k1, k2, k3, k4);
	printf("1/k4 = %f\n", 1.0 / k4);
	
}



class ModelParams
{
public:
	double k1;
	double k2;
	double k3;
	double k4;
};

#if 0 // the floating version (works)
class DacCompensation
{
public:
	DacCompensation(const ModelParams& p) : 
	  _offset( -p.k3),
	  _gain( 1.0 / p.k4)
	{
		printf("in comp dbl offset=%f gain=%f\n", _offset, _gain);
	}
	// TODO: make voltage int.19
	int codeForVoltage(double voltage) const
	{
		double ret =  (voltage + _offset) * _gain;
		return (int) ret;		// dac codes are integers
	}
private:
	const double _offset;
	const double _gain;
};
#else


// the fixed point version
class DacCompensation
{
public:
	static const int n = 10;
	DacCompensation(const ModelParams& p)
	 
	{
		double gainScale = 1 << n;

		double offset =  (-p.k3 * oneV);	// input will we rel 1v.19
		double gain = ( gainScale / p.k4);		// bump up gain for more decimal points
		
		_gain = (int) gain;
		assert(_gain > 0);
		assert(_gain > gain - 1);
		assert(_gain < gain + 1);

		_offset = (int) offset;
		//assert(_offset > 0);
		assert(_offset > offset - 1);
		assert(_offset < offset + 1);

		printf("in comp off=%d gain=%d (was %f) intn=%d\n", _offset, _gain, gain, n);
	}
	// TODO: make voltage int.19
	int codeForVoltage(double voltage) const
	{
		long long v = (int)(voltage * oneV);		// scale up to v.19

		long long x = (v + _offset) * _gain;
		long long ret = x >> (n + 19);

		return (int) ret;		// dac codes are integers
	}
private:
	int _offset;
	int _gain;
};
#endif



void deriveParams(ModelParams& outParams, const int * readings)
{
	outParams.k1 = readings[0];

	// 4)
	outParams.k2 = (double)(readings[3] - readings[0]) / 3.0;

	// first try
	//k3 = -H / (R[1] - R[2]);
	outParams.k3 = ((double)readings[1] - outParams.k1) / outParams.k2;

	

	//k4 = (R[1] - k1) / (k3*k2);
	outParams.k4 = (1/H) * ((((double)readings[2] - outParams.k1) / outParams.k2) - outParams.k3);

	printf("der: k1=%f, k2=%f, k3=%f k4 = %f\n", outParams.k1, outParams.k2, outParams.k3, outParams.k4);
	printf("1/k4 = %f\n", 1.0 / outParams.k4);
}

double dac2Voltage(const ModelParams& params, int rawDac)
{
	return rawDac * params.k4 + params.k3;
}

void testDac(double desiredVoltage, const ModelParams& model, const DacCompensation& dc)
{
	int c = dc.codeForVoltage(desiredVoltage);
	double v2 = dac2Voltage(model, c);
	printf("test dac desired v = %f code = %d finalv=%f error%e\n", 
		desiredVoltage, c, v2, (desiredVoltage - v2));
	//printf("code / 1v = %d\n", c >> 19);
	//assert(v2 == desiredVoltage);

	double tol = 1.0 / (1000.0 * 100.0);	// let's got for hundredth of millivolt

	if ( (v2 > (desiredVoltage + tol )) ||  (v2 < (desiredVoltage - tol )))
		printf("would fail\n");
	assert (v2 < (desiredVoltage + tol ));
	assert (v2 > (desiredVoltage - tol ));
}

void try4()
{
	printf("try 4\n");
	ModelParams model;
	deriveParams(model, R);

	DacCompensation dc(model);

	double v1 = dac2Voltage(model, 0);	// output 0
	printf("v1 = %f\n", v1);
	assert(v1 > -1 && v1 < 1);		// should be close to zero

	testDac(0, model, dc);
	testDac(1, model, dc);
	testDac(8, model, dc);
	testDac(12, model, dc);
}


void Calibrate()
{
	//try1();
	//try2();
	//try3();
	try4();
}


/*
float case:


der: k1=-43221.000000, k2=807534.666667, k3=0.002240 k4 = 0.000001
1/k4 = 895155.832848
in comp dbl offset=-0.002240 gain=895155.832848
v1 = 0.002240
test dac desired v = 0.000000 code = -2005 finalv=0.000000 error-3.180311e-007
test dac desired v = 1.000000 code = 893150 finalv=0.999999 error6.123633e-007
test dac desired v = 8.000000 code = 7159241 finalv=8.000000 error4.223809e-007


int 8: 
der: k1=-43221.000000, k2=807534.666667, k3=0.002240 k4 = 0.000001
1/k4 = 895155.832848
in comp off=-1174 gain=229159893 (was 229159893.209091) intn=8
v1 = 0.002240
test dac desired v = 0.000000 code = -2005 finalv=0.000000 error-3.180311e-007
test dac desired v = 1.000000 code = 893151 finalv=1.000001 error-5.047606e-007
test dac desired v = 8.000000 code = 7159242 finalv=8.000001 error-6.947431e-007

int 1:
test dac desired v = 0.000000 code = -2005 finalv=0.000000 error-3.180311e-007
test dac desired v = 1.000000 code = 893151 finalv=1.000001 error-5.047606e-007
test dac desired v = 8.000000 code = 7159239 finalv=7.999997 error2.656629e-006







*/

