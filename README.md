atmega644p
==========
This repository has code files of atmega644p drivers. 
More info on each change can be found in: [http://bugmicrocontrollers.blogspot.in]

Oct 18th 2014:
+ I2C library has been added.
+ Testing done for all 4 modes of I2C:
	Master transmit - demo link [http://youtu.be/utMPuFVPKt0]
	Master receive - demo link [http://youtu.be/lqPqFMEVYwY]
	Slave receive - demo link [http://youtu.be/h8Ehrc5-8lQ]
	Slave transmit - demo link [http://youtu.be/bTXGKSk8M1k]
+ main.c file has been modified for testing I2C library.
+ main.h has been added to include the #defines for each new driver.

Sept 20th 2014:
+ In main.c comments has been added to distinguish between Bare-metal part od code and Driver part of the code.

Dec 13th 2013:
+ redefined version of printf() and scanf() has been added
+ main.c file has been modified for testing the printf() and scanf() code.

Nov 16th 2013:
+ USART library has been added.
+ Testing done for both Normal operation and interrupt driven for receiving data.
+ main.c file has been modified for USART Test. USART test done with bare-metal code as well as with library code.

Aug 8th 2013:
+ GPIO files has been added. It is the very first version of the GPIO driver! Check the main.c file to know how it works!
+ It is the very first example of the Bare-Metal Programming for Sanguino