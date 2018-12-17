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

	if(strncmp(argv[1],"download-file",13) == 0){
		send(clientSocket, argv[2], 50,0);

		int f = open(argv[2], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
		if(f<0){
			printf("error\n");
			exit(1);
		}

		int size = 0;
		if(recv(clientSocket, &size, sizeof(int), 0) < 0)
		{
			printf("[-]Error in receiving data.\n" );
		}
		for(int i=0 ; i<size ; i++){
			if(recv(clientSocket, buffer,SIZE, 0) < 0)
			{
				printf("[-]Error in receiving data.\n" );
			}

			write(f, buffer, strlen(buffer));
		}
		close(f);


}
 else if(strncmp(argv[1],"search",6) == 0){
	 int size = 0;
	 send(clientSocket, argv[2], 50,0);
	 if(recv(clientSocket, &size, sizeof(int), 0) < 0)
	 {
		 printf("[-]Error in receiving data.\n" );
	 }
	 for(int i=0 ; i<size ; i++){
	 if(recv(clientSocket, buffer,50, 0) < 0)
		{
			printf("[-]Error in receiving data.\n" );
		}
		printf("%s\n",buffer );
}

}

	else{
		printf("there is no cammand like %s\n",argv[1]);
	}

return 0;

}
