/* 
 * File:   PersistentInts.h
 * Author: bruce
 *
 * Created on April 2, 2016, 1:23 PM
 */

#ifndef PERSISTENTINTS_H
#define	PERSISTENTINTS_H

class PersistentInts
{
public:
    PersistentInts()
    {
        for (int i=0; i< numEntries; ++i)
        {
            _cache[i] = 0;
        }
    }
    static const int bankOffset = 0;
    static const int numEntries = bankOffset + 1; 
    void init(void * baseAddress);
    
    int get(int index) // read from persistent memory
    {
        int ret = 0;
        if (index>=0 && index<numEntries)
        {
            ret = _cache[index];
        }
        return ret;
    }
    void write(int index, int value)       // will cause entire page to be re-written
    {
         if (index>=0 && index<numEntries)
        {
            _cache[index] = value;
        }
    }
private:
    int _cache[numEntries];
};


#endif	/* PERSISTENTINTS_H */

