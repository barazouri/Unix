#include<stdio.h>
#include "stdlib.h"
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <fcntl.h>
#include<ctype.h>

#define PORT 8080
#define SIZE 2000
int main(int argc , char *argv[])
{
	int clientSocket, ret, fileNum=0;
	struct sockaddr_in serverAddr;
	char buffer[SIZE];

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-] Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created\n" );

	memset(&serverAddr, '\0', sizeof(serverAddr));;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( PORT );
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0)
	{
		printf("[-]Error in connection\n" );
		exit(1);
	}
	printf("[+]connected to server\n" );
	send(clientSocket, argv[1], 50, 0);
	if(strncmp(argv[1],"list-file",9) == 0)
{
		int size=0;

		if(recv(clientSocket, &fileNum, sizeof(fileNum), 0) < 0)
		 {
		 	printf("[-]Error in receiving data.\n" );
		 }
		else
		{
			if(fileNum == 0){
				return 0;
			}

			for(int i=0 ; i<fileNum ; i++){
				if(recv(clientSocket, buffer, SIZE, 0) < 0)
				{
					printf("[-]Error in receiving data.\n" );
				}

				if(recv(clientSocket, &size, sizeof(int), 0) < 0)
				{
					printf("[-]Error in receiving data.\n" );
				}

				printf("Server:\n%s    %d\n", buffer,size);
}

		}

	}
	else if(strncmp(argv[1],"upload-file",11) == 0){
		send(clientSocket, argv[2], 50,0);
		if(recv(clientSocket, buffer, SIZE, 0) < 0)
		{
			printf("[-]Error in receiving data.\n" );
		}
		if(strncmp(buffer,"exist",5) == 0){
			printf("the file is already exist\n");
			return 0;
		}
		char c;
		int f = open(argv[2],O_RDONLY);
		if(f < 0){
			printf("the file coudnt be open\n");
			return 0;
		}

		int line = 0, ret;
		while((ret = read(f,buffer, SIZE )) > 0 ){
			line++;
		}
		send(clientSocket, &line, sizeof(int),0);
		lseek(f, 0, SEEK_SET);

		while((ret = read(f,buffer, SIZE )) > 0){
			buffer[ret] = '\0';
			send(clientSocket, buffer, SIZE,0);
		}

		printf("The file has been successfully sent.\n" );

}

	else{
		printf("there is no cammand like %s\n",argv[1]);
	}

return 0;

}
