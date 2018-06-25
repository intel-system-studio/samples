This file (vectorlength.cpp) is meant to be used with the "Advantages of Vectorization &
the Effects of Data Size" tutorial for Intel® Advisor XE 2016. It is intended to be compiled
with the Intel C++ Compiler in Microsoft* Visual Studio 2010. The process may be slightly 
different using other compilers or in other environments.

To compile as intended, right click the project name and under "Intel Compiler", select "Use 
Intel C++", and perform the following steps.

1. On the tool bar, select "Build" and then select "Configuration Manager". Change the 
   project's configuration to "Release". If your system is a 64-bit System, change the 
	 project's Platform to "x64". If no such platform exists, select "<New...>" in the dropdown
	 that appeared when you clicked to change the platform, and select "x64" in the "New 
	 platform:" box. Select "Win32" for the "Copy settings from:" box. Check "Create new 
	 solution platforms" and hit "OK". Hit "OK" again to finish setting up the configuration.

2. Right click the project name in the solution explorer and select "Properties". Under 
   Configuration Properties>C/C++, ensure that you use and exclude the following flags as 
	 indicated.

		Compile WITH the following Flags:
	/Zi 					(Under >General>Debug Information Format, set to "Program Database")
	/O2						(Under >Optimization>Optimization, set to "Maximize Speed")
			(Type/copy all of the following into the "Additional Options" box under >Command Line,
			 separated by spaces.):
	/fp:fast 
	/Qopt-report5 
	/QxHOST 
	/Qopenmp 
	/debug:inline-debug-info 

		Compile WITHOUT the following flags:
	/Qi						(Under Optimization>Enable Intrinsic Functions, set to "No")
	/Qip or /Qipo	(Under Optimization [Intel C++]>Interprocedural Optimization, set to "No")
	/Qparallel		(Under Optimization [Intel C++]>Parallelization, set to "No")

3. When all flags are set, select "Build" on the tool bar, and select "Build Solution" or 
	 "Build VectorLength".

See the online video: https://software.intel.com/en-us/videos/advantages-of-vectorization-and-the-effects-of-data-size

Legal Information
------------------------------------------

INFORMATION IN THIS DOCUMENT IS PROVIDED IN CONNECTION WITH INTEL PRODUCTS. NO LICENSE, EXPRESS OR IMPLIED, BY ESTOPPEL OR OTHERWISE, TO ANY INTELLECTUAL PROPERTY RIGHTS IS GRANTED BY THIS DOCUMENT. EXCEPT AS PROVIDED IN INTEL'S TERMS AND CONDITIONS OF SALE FOR SUCH PRODUCTS, INTEL ASSUMES NO LIABILITY WHATSOEVER, AND INTEL DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTY, RELATING TO SALE AND/OR USE OF INTEL PRODUCTS INCLUDING LIABILITY OR WARRANTIES RELATING TO FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABILITY, OR INFRINGEMENT OF ANY PATENT, COPYRIGHT OR OTHER INTELLECTUAL PROPERTY RIGHT. 

UNLESS OTHERWISE AGREED IN WRITING BY INTEL, THE INTEL PRODUCTS ARE NOT DESIGNED NOR INTENDED FOR ANY APPLICATION IN WHICH THE FAILURE OF THE INTEL PRODUCT COULD CREATE A SITUATION WHERE PERSONAL INJURY OR DEATH MAY OCCUR. 

Intel may make changes to specifications and product descriptions at any time, without notice. Designers must not rely on the absence or characteristics of any features or instructions marked "reserved" or "undefined." Intel reserves these for future definition and shall have no responsibility whatsoever for conflicts or incompatibilities arising from future changes to them. The information here is subject to change without notice. Do not finalize a design with this information. 

The products described in this document may contain design defects or errors known as errata which may cause the product to deviate from published specifications. Current characterized errata are available on request. Contact your local Intel sales office or your distributor to obtain the latest specifications and before placing your product order. Copies of documents which have an order number and are referenced in this document, or other Intel literature, may be obtained by calling 1-800-548-4725, or go to: http://www.intel.com/design/literature.htm 

Intel processor numbers are not a measure of performance. Processor numbers differentiate features within each processor family, not across different processor families. Go to: http://www.intel.com/products/processor_number/

BlueMoon, BunnyPeople, Celeron, Celeron Inside, Centrino, Centrino Inside, Cilk, Core Inside, E-GOLD, i960, Intel, the Intel logo, Intel AppUp, Intel Atom, Intel Atom Inside, Intel Core, Intel Inside, Intel Insider, the Intel Inside logo, Intel NetBurst, Intel NetMerge, Intel NetStructure, Intel SingleDriver, Intel SpeedStep, Intel Sponsors of Tomorrow., the Intel Sponsors of Tomorrow. logo, Intel StrataFlash, Intel vPro, Intel XScale, InTru, the InTru logo, the InTru Inside logo, InTru soundmark, Itanium, Itanium Inside, MCS, MMX, Moblin, Pentium, Pentium Inside, Puma, skoool, the skoool logo, SMARTi, Sound Mark, The Creators Project, The Journey Inside, Thunderbolt, Ultrabook, vPro Inside, VTune, Xeon, Xeon Inside, X-GOLD, XMM, X-PMU and XPOSYS are trademarks of Intel Corporation in the U.S. and/or other countries.

*Other names and brands may be claimed as the property of others.

Microsoft, Windows, Visual Studio, Visual C++, and the Windows logo are trademarks, or registered trademarks of Microsoft Corporation in the United States and/or other countries. 

Copyright (C) 2017, Intel Corporation. All rights reserved. 

