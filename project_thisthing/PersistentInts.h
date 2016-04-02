/* 
 * File:   PersistentInts.h
 * Author: bruce
 *
 * Created on April 2, 2016, 1:23 PM
 */

#ifndef PERSISTENTINTS_H
#define	PERSISTENTINTS_H

class PersistentInt
{
public:
    static const int bankOffset = 0;
    static const int numEntries = bankOffset + 1; 
    void init(void * baseAddress);
    
    int get(int index); // read from persistent memory
    void write(int index, int value);       // will cause entire page to be re-written
};


#endif	/* PERSISTENTINTS_H */

