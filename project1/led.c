
/*
 * C functions to manage the state of the 8 LEDs
 */

#include <plib.h>

// mode 0) standard (a-1, b-2)
// mode 1) binary , "1" will light one led, 16 lights none, zero not allowed
// mode 2) binary, 0 is no leds, 8 bits
static void Led_lightSelector(int mode);


/***********************************************************
 * 
 * LED display routines
 * 
 ***********************************************************/

 const int apins = BIT_2 | BIT_3;
 const int bpins = BIT_4 | BIT_6 | BIT_7 | BIT_10 | BIT_11 | BIT_12 | BIT_15;
 
 /* slightly messed up
  * pattern is:
  * 
  *     4  0
  *     5  1
  *     6  2
  *     7  3
  */
void Led_setOne(int index)
{
    static const int sequence[8][2] = {
        { BIT_3, 0 },
        { 0, BIT_15 },
        { 0, BIT_11 },
        { 0, BIT_7 },
        { 0, BIT_4 },
        { 0, BIT_12 },
        { 0, BIT_10 },
        { 0, BIT_6 },
    };  
  
    PORTASET = sequence[index][0];
    PORTBSET = sequence[index][1];
}



// The current algorithm running. Will be displayed unless temporary override
static int cur_selector=0;

// The current override value
static int cur_override_request=-1; 

static int use_override=0;

// the number of clocks remaining to display the override
static int override_count=-1;  

// true if we just changed effects, and hence have a new selector
static int just_set_selector=0;

// if we are unable to immediately set temp override, then put it here
//static int override_count_suppress=-1;
//static int suppressed_override_selector=-1;

// indicate which algorithm is running by putting the hex value in the leds
// also sets system state

void Led_setSelector(int selector)
{
    cur_selector = selector;
    use_override = 0;
    cur_override_request = 0;          // reset and show select
    just_set_selector = 1;
    Led_lightSelector(0);    // light in standard "a1, b2" mode
}

void Led_setTempSelectorOverride(int selector, int mode)
{
    // only process changes in override request
    if (selector == cur_override_request)
        return;
    
    // if algorithm just changed, must show that, not override
    if (just_set_selector)
    {
        just_set_selector = 0;
        cur_override_request = selector;    // now we will ignore this value until it changes
        return;
    }
    
    Nop();
    use_override = 1;
    cur_override_request = selector;
    override_count = 80000 * 5;
    Led_lightSelector(mode);   
}

void Led_clock()
{
    if (override_count < 0)
        return;
    
    Nop();
    
    // here a algorithm has still overridden the selector
    --override_count;
    if (override_count < 0)
    {
        Nop();
        use_override = 0;               //override timed out
        Led_lightSelector(0);           
    }
}
static void Led_lightSelector(int mode)
{
    int selector = (use_override)  ? cur_override_request : cur_selector;
    PORTACLR = apins;
    PORTBCLR = bpins;
 
    switch(mode)
    {
        case 0:
        {
            const int nib_hi = selector >> 2;
            const int nib_lo = selector & 3;
            Led_setOne(nib_lo);      // led 0..3 get least sign nibble
            Led_setOne(nib_hi + 4);
        }
            break;
        case 1:
            if (selector < 16)
            {
                if (selector & 1) Led_setOne(0); 
                if (selector & 2) Led_setOne(1); 
                if (selector & 4) Led_setOne(2); 
                if (selector & 8) Led_setOne(3); 
            }
            break;
         case 2:
            if (selector < 256)
            {
                if (selector & 1) Led_setOne(0); 
                if (selector & 2) Led_setOne(1); 
                if (selector & 4) Led_setOne(2); 
                if (selector & 8) Led_setOne(3); 
                if (selector & 16) Led_setOne(4); 
                if (selector & 32) Led_setOne(5); 
                if (selector & 64) Led_setOne(6); 
                if (selector & 128) Led_setOne(7); 
            }
            break;
    }
            

}




void Led_blank()
{
    PORTACLR = apins;
    PORTBCLR = bpins;
}
