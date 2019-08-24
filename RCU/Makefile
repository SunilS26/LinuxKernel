///////////////////////////////////////////////////////////////////
//
//      COPYRIGHT (C) SunilS GitHub, 2019
//      ALL RIGHTS RESERVED. UNPUBLISHED RIGHTS RESERVED
//      UNDER THE COPYRIGHT LAWS OF THE UNITED STATES.
//
//      THE SOFTWARE CONTAINED ON THIS MEDIA IS OPEN SOURCE
//      YOU MAY USE, DISTRIBUTE AND MODIFY THIS CODE UNDER THE
//      TERMS OF THE GPL LICENSE.
//      
//      NOTE: THE BELLOW WRITTEN CODE IS THE OUTPUT OF MY 
//		UNDERSTANDING AND CAN BE SOLELY USED FOR THE PURPOSE OF
//		GETTING INSIGHT ABOUT OPEN SOURCE API/TECHNOLOIES. 
//
///////////////////////////////////////////////////////////////////

obj-m += miscDriver.o

all:
	    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
