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
//		GETTING INSIGHT ABOUT OPEN SOURCE API/TECHNOLOGIES. 
//
///////////////////////////////////////////////////////////////////


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>

#define MY_MAGIC 'K'
#define IOCTL_TASK _IOWR(MY_MAGIC, 0, myIocmd_t)

typedef struct myIocmd {
	int cmd;
	void *buff;
	int value;
}myIocmd_t;

enum cmdMacro {
	SAY_HI = 1,
	INITIALIZE_INFO,
	CONTI_READ_NODE=3,
	REMOVE_NODE,
}cmdMacro_t;

/* Driver Program */
int main()
{
	int my_fd = -1;
	int ret;
	int opt = 0;
	unsigned char buff[11] = {0};
	ssize_t bytes = 0;
	myIocmd_t myCmd;

	my_fd = open("/dev/Trial_Driver", O_RDWR);

	bytes = read(my_fd, buff, 10);

	printf("Buffer returned is %s\n", buff);
	
	printf("Enter your option");
	printf("\t1: SAY_HI\n");
	printf("\t2: INITIALIZE_INFO_RCU\n");
	printf("\t3: Read continuously from link list\n ");
	printf("\t4: Remove Node\n ");
	scanf("%d",&opt);

	switch(opt) {
		case SAY_HI:
			myCmd.cmd = SAY_HI;
			ioctl(my_fd, IOCTL_TASK, &myCmd);
			break;

		case INITIALIZE_INFO:
			myCmd.cmd = INITIALIZE_INFO;
			ioctl(my_fd, IOCTL_TASK, &myCmd);
			break;

		case CONTI_READ_NODE:
			myCmd.cmd = CONTI_READ_NODE;
			ioctl(my_fd, IOCTL_TASK, &myCmd);
			break;

		case REMOVE_NODE:
			while(1) {
			printf("\t\t\tEnter your option for Node Volume:\n");
			printf("\t\t\t\t\t1.Trident\n");
			printf("\t\t\t\t\t2.Clariion\n");
			printf("\t\t\t\t\t3.Unisphere\n");
			printf("\t\t\t\t\t4.Unity\n");
			printf("\t\t\t\t\t5.VMAX\n");
			printf("\t\t\t\t\t6.Symmetric\n");
			printf("\t\t\t\t\t0.Exit\n");
			scanf("%d",&opt);

			if(opt == 0) break;

			myCmd.value = opt;
			myCmd.cmd = REMOVE_NODE;
			ioctl(my_fd, IOCTL_TASK, &myCmd);
			}
			break;

		default:
			printf("Invalid input\n");
	}

	close(my_fd);

	return 0;
}
