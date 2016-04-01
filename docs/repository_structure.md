# Repository Structure

## common

C++ code for all the algorithms. This code is cross-platform, pic32 and windows.

A very important file is DModule.h, which describes the C++ interface to an algorithm or module. Any new algorithms must implement this interface to run in our framework.

## Project_thisthing

C code for disting startup, initialization, I/O utilities. Much of this is directly derived from the "hello disting" provided by Expert Sleepers.

The file "modules.cpp" here has the array of Algorithms/Modules that will be available when running the firmware.

The MPLAB project folder lives under here also.

## UnitTest

A Visual Studio C++ for Windows project containing the unit test suite.

No code in this folder tree is ever run on the disting.

 

