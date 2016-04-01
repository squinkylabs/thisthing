# thisthing
Alternate firmware for disting

Please read this carefully, otherwise you may damage your Disting, of have other difficulties.

## How to install and use ThisThing

You will need
* A PickIt3 hardware device
* The IPE software
* The disting firmware .hex file

Please read the instructions for installation carefully. They will tell you exactly what tools and software you need, and how to safely install ThisThing.

Important links:
[Instructions for Installing the firmware](docs/flashing_disting.pdf)

[The downloadable releases are here](https://github.com/squinkylabs/thisthing/releases)


## Information for coders

Before doing anything, follow the directions above to back up your Disting's factory firmware. This is critical.

### Required development tools
All of the MPLAB tools are available for free.

MPLAB IPE (free) for programming the flash from, and backing up. See notes above.

MPLAB X IDE (free) programming environment for PIC development. We use version 3.00.02-beta, but more recent versions should be fine.

MPLAB XC32++ C++ compiler (free). We user version 1.34. We have heard that version 1.40 work, too. Be aware the Microchip keeps changing the support for the PLIB support libraries, and at some point newer compilers may not be compatible with our source code. While the compiler is free, you must activate the license explicitly. There should be a link to get the free license in the window at the end of the XC32 installer. 

MPLAB Legacy PLIB support libraries. Following the lead of Expert Sleeper's "Hello Disting", we use the PLIB support libraries to talk to the PIC hardware. Microchip now considers this a "legacy" lbrary. As such you may need to download this separately if you use a recent compiler. We have heard it is best to do this after installing the compiler.

Microsoft Visual Studio 2010 (optional). The unit test suite comes with a visual studio project file. If you want to run the unit tests you will either need this software, or will need to make your own make file for a different C++ compiler and linker.

### Disting mk2 vs. mk1

The MPLAB IDE project file is configured for the mk1 disting. If you wish to develop on the mk2, you will need to do two things:
* change the CPU type from MX150F128B to MK170F256B.
* change the preserve memory range from 0x1d01fe00/0x1d01ffff to 0x1d03fe00/0x1d03ffff  

The CPU type may be set in the IDE from File/Project Properties, conf: . The Preserve Memory settings are in Conf: PICKit 3.
### Other notes

[Notes on the repository structure](docs/repository_structure.md)

<img src="docs/synth.jpeg" alt="alt text" width="300px">


