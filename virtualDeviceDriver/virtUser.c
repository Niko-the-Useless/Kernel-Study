#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

char writeBuffer[1024];
char readBuffer[1024];

int main(){
	int deviceFile;
	unsigned int option;

	deviceFile = open("/dev/virtDevice", O_RDWR); // opening with read write permission
	if(deviceFile<0){
		printf("Cannot open device file\n");
		return -1;
	}
	
	while(1){
		printf("enter option: 1-Write, 2-Read, 3-Exit\n");
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
			default:
				printf("Invalid option X.X");
				break;
		}
	}
	close(deviceFile);
	return 0;
}
