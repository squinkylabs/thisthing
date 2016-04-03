/* 
 * File:   PersistentInts.h
 * Author: bruce
 *
 * Created on April 2, 2016, 1:23 PM
 */

#ifndef PERSISTENTINTS_H
#define	PERSISTENTINTS_H

/*
 * 0x1d01fe00 for mkI, or 0x1d03fe00 for mkII.


 calibration data physical address = 0x1d01fe00
 *                          end = 0x1d01ffff

 * page size = 1024 = 0x400
 * so start cal of mk1 page starts at 1d01fc00
 * so I can try 1d01f800
 */
class PersistentInts
{
public:
    PersistentInts() : _physicalPageAddr(0)
    {
        // clear the cache
        for (int i=0; i< numEntries; ++i)
        {
            _cache[i] = 0;
        }
    }
    static const int bankOffset = 0;
    static const int numEntries = bankOffset + 1; 
    
    // pass in where the calibration data lives
    void init(void * baseAddressCalData)
    {
        Nop();
        unsigned int bai = (unsigned int)baseAddressCalData;
        unsigned int calPageAddress = bai & 0xfffffc00;
        unsigned int persistPageAddress = calPageAddress - 0x400;
        _physicalPageAddr = (void *)persistPageAddress;
        Nop();
    }
    
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
            program();
        }
    }
private:
    int _cache[numEntries];
    
    int _lastErr;
    void * _physicalPageAddr;
    
    void program()
    {
        /*
         * : 0x1d01fe00 for mkI, or 0x1d03fe00 for mkII.
Enter the end address: 0x1d01ffff for mkI, or 0x1d03ffff for mkII.

         calibration data physical address = 0x1d01fe00
         *                          end = 0x1d01ffff
         *   end = 65535
         *  start = 65024
         * page size = 1024 = 0x400
         * so start cal of mk1 page starts at 1d01fc00
         * so I can try 1d01f800
         */ 
        
        Nop();
        if (!_physicalPageAddr)
        {
            return;      // didn't init
        }
      
        _lastErr = NVMErasePage(_physicalPageAddr);
        const int marker = 0xaa55aa77;
        _lastErr =  NVMWriteWord (_physicalPageAddr, marker);
        for (int i=0; i<numEntries; ++i)
        {
            unsigned int addr = (unsigned int)_physicalPageAddr;
            _lastErr = addr;
            addr += sizeof(unsigned int) * (1 + i);
            _lastErr = addr;
            NVMWriteWord ((void *)addr, _cache[i]);
        }
        Nop();
    }
};


#endif	/* PERSISTENTINTS_H */

