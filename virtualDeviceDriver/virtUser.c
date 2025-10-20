#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define readValueIOCTL _IOW('a',0,int32_t*)
#define writeValueIOCTL _IOR('a',1,int32_t*)

char writeBuffer[1024];
char readBuffer[1024];
int deviceFile;

void ioctlWrite(void);
void ioctlRead(void);

int main(){
	unsigned int option;

	deviceFile = open("/dev/virtDevice", O_RDWR); // opening with read write permission
	if(deviceFile<0){
		printf("Cannot open device file\n");
		return -1;
	}
	
	while(1){
		printf("enter option: 1-Write, 2-Read, 3-Exit, 4-ioctl write, 5-ioctl read\n");
		scanf("%d", &option);
		printf("Option %d\n", option);

		switch(option){
			case 1:
				printf("enter string:");
				scanf(" %[^\t\n]", writeBuffer); // writing to writeBuffer while ignoring leading whitespace and ending at tab or newline
				printf("writing data\n");
				write(deviceFile, writeBuffer, strlen(writeBuffer)+1);
				printf("done ^-^\n");
				break;
			case 2:
				printf("Reading data\n");
				read(deviceFile, readBuffer, 1024);
				printf("Done ^-^\n");
				printf("Data = %s\n",readBuffer);
				break;
			case 3:
				close(deviceFile);
				exit(1);
				break;
			case 4:
				ioctlWrite();
				break;
			case 5:
				ioctlRead();
				break;
			default:
				printf("Invalid option X.X");
				break;
		}
	}
	close(deviceFile);
	return 0;
}

void ioctlWrite(){
	int32_t value;
	printf("Enter value to send\n");
	scanf("%d",&value);
	printf("Writing to driver\n");
	ioctl(deviceFile, writeValueIOCTL, (int32_t*) &value);
}

void ioctlRead(){
	int32_t value;
	printf("Reading value from driver\n");
	ioctl(deviceFile, readValueIOCTL, (int32_t*) &value);
	printf("Value =%d\n",value);
}
